//player.cpp
/*-----------------------------------------------------------------------------------*
 *-----------------------------------------------------------------------------------*/

#include "player.h"
#include "../assist/assist.h"

// 設定
#define DOSEI_ON 0	//1:どせいを使う 0:くねくねを使う
#define DOSEI_IDORYOU 0.2	//どせい操作時の移動量

#define DOSEI_JAMP_BACKJAMP 1	//バックジャンプの有無
#define DOSEI_JAMP_DOUBLE 0	//空中ジャンプの有無
#define DOSEI_JAMP_3STEP 0	//３段ジャンプの有無 (有効にすると別ゲーになります)
#define DOSEI_JAMP_MUGEN 0	//無限ジャンプの有無(チート)

#define JAMP_B_SETTIMER 7	//バックジャンプの判定時間（1〜7フレーム）
#define JAMP_3_SETTIMER 3	//３段階ジャンプの判定時間（2〜7フレーム）実際はこれの-1フレーム

#define JAMP_SPEED 0.4	//通常ジャンプ量
#define JAMP_B_SPEED 0.8	//バックジャンプのジャンプ量
#define JAMP_D_SPEED 0.5	//空中ジャンプのジャンプ量
#define JAMP_3_SPEED_2 0.5	//３段ジャンプの２段階目ジャンプ量
#define JAMP_3_SPEED_Y 2.6	//３段ジャンプのジャンプ量(上方向最大値)
#define JAMP_3_SPEED_Z 3.0	//３段ジャンプのジャンプ量(前方向最大値)


// 定数定義
// backjamp用定数
#define JAMP_B_TIMER 7	//バックジャンプ用タイマー（フィルター）
#define JAMP_B_FORWORD 16	//前回前方へすすんでいたフラグ
#define JAMP_B_BACK	32	//前回後方へすすんでいたフラグ
#define JAMP_B_SUCCEED 64	//バックジャンプ成功フラグ
#define JAMP_3_1STEP 128	//１段目ジャンプ
#define JAMP_3_2STEP 256	//２段目ジャンプ
#define JAMP_3_TIMER (512+1024+2048)	//３段ジャンプ用タイマー（フィルター）
#define JAMP_3_T 512	//３段ジャンプ用タイマーの最小単位
#define JAMP_3_SUCCEED 4096	//３段ジャンプ成功フラグ

//////////////////////
//チェックするキー
//テンキー用
#define KEY_LEFT '4'
#define KEY_RIGHT '6'
#define KEY_UP '8'	
#define KEY_DOWN '5'



/*-----------------------------------------------------------------------------------*
	影オブジェクト
	当たり判定なしスプライト
 *-----------------------------------------------------------------------------------*/
ShadowOBJ::ShadowOBJ( const char *texName, float scale){
	DisplayList = 0;
	pos = 0.0;
	speed = 0.0;
	speed.y = -10000.0;
	if( texName ){
		png.load( texName );
		SetRender(scale);
	}
}

//テクスチャとサイズの登録
void ShadowOBJ::Set(const char *texName, float scale){
	if( !DisplayList && texName ){
		png.load( texName );
		if( scale < 0.0 )
			scale = 1.0;
		SetRender(scale);
	}else if( scale >= 0.0 ){
		SetRender(scale);
	}
}

//ディスプレイリストを作る
void ShadowOBJ::SetRender(float scale){
	//UV
	const static float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	const float vpos[4][3] = { {scale,0.0,scale}, {scale,0.0,-scale}, {-scale,0.0,-scale}, {-scale,0.0,scale} };


	//以前作っていたら削除
	if( DisplayList )
		glDeleteLists(DisplayList, 1);


	DisplayList = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList, GL_COMPILE); //コンパイルのみ

	//マテリアルをデフォルトにもどす。
	setDefaultMaterial();
	
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, png.texture); //テクスチャのバインド

	//ブレンド有効
	glEnable(GL_BLEND);
	
	//描画開始
	glBegin(GL_QUADS);
	
	glNormal3d(0.0, 1.0, 0.0);
	
	for(int i=0; i<4; i++){
		glTexCoord2fv( uv[i] );
		glVertex3fv( vpos[ i ] );
	}

	glEnd();
	
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND); //GL_ALPHA_TEST);// アルファテスト無効
	
	glEndList();	//ディスプレイリストおわり
}

//描画
void ShadowOBJ::Render(){
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	glCallList(DisplayList);	//描画
	glPopMatrix();
}

//デストラクタ
ShadowOBJ::~ShadowOBJ(){
	glDeleteLists(DisplayList, 1);
}


/*-----------------------------------------------------------------------------------*
	操作キャラクター用のクラス
 *-----------------------------------------------------------------------------------*/
PlayerClass::PlayerClass(){

	//どせい関連の初期化	
	//Xファイル読み込み
#if DOSEI_ON
	char a[]="Models/dosei.x";
	dosei.offset.y=1.0;
#else
	char a[]="Models/anim2.x";
	dosei.offset.y=-0.2;
#endif
	model.Load(a, 0);
	//Xオブジェクト作成
	dosei.setXModel(&model);
	ResetPos();
	ResetAng();

	//当たり判定があったか
	onface = 0;
	spacekey = 0;

	//バックジャンプ用
	backjamp = 0;
	
	//影
	shadow.Set("shadow.png", 0.8f);

}

//初期位置にもどす
void PlayerClass::ResetPos(float x, float y, float z ){
	dosei.pos.x = x;
	dosei.pos.y = y;
	dosei.pos.z = z;
	dosei.speed = 0.0;
}

//初期角度にもどす
void PlayerClass::ResetAng(){
	dosei.angle = 180;
	dosei.ang.y = 1.0;
	//どせいに関するもの初期化
	dash_vec.Set(0.0, 0.0, 1.0);	//どせいの前方向ベクトル
	dash_svec.Set(1.0, 0.0, 0.0);	//どせいの横方向ベクトル
}

//キー状態に応じて位置更新
void PlayerClass::Move(bool keystate[256]){
	//キー処理(どせいの移動)
	if( keystate['b'] ){	//bキー
		addAngle( 1.0 );
	}
	if( keystate['n'] ){	//nキー
		addAngle( -1.0);
	}
	if( keystate[' '] ){	//space
		if( onface > 0 && spacekey < 1 ){	//床に接地していて、スペースキーが押された瞬間
#if DOSEI_JAMP_BACKJAMP
			if( (backjamp & JAMP_B_SUCCEED) && (backjamp &= ~JAMP_B_SUCCEED, backjamp & JAMP_B_TIMER) ){
				dosei.speed.y += JAMP_B_SPEED;	//バックジャンプ
				printf("バックジャンプ！\n");
			}else
#endif
#if DOSEI_JAMP_3STEP
			if( (backjamp & JAMP_3_1STEP) && (backjamp &= ~JAMP_3_1STEP, backjamp & JAMP_3_TIMER ) ){	//7ビット目(=128)は１段階目ジャンプのフラグ
				dosei.speed.y += JAMP_3_SPEED_2;
				backjamp |= JAMP_3_2STEP + JAMP_3_T * JAMP_3_SETTIMER;	//２段階目ジャンプのフラグ + タイマーセット
				printf("2段階目!\n");
			}else if( (backjamp & JAMP_3_2STEP) && (backjamp &= ~JAMP_3_2STEP, backjamp & JAMP_3_TIMER ) ){
				//３段階ジャンプ	
				dosei.speed.y += JAMP_3_SPEED_Y ;//* sin(cam_rx);	//上方向
				dosei.speed += dash_vec * JAMP_3_SPEED_Z ;//* cos(cam_rx);	//前方向
				backjamp |= JAMP_3_SUCCEED;	//３段階ジャンプ成功フラグ
			}else
#endif
			{
				dosei.speed.y += JAMP_SPEED;
#if DOSEI_JAMP_3STEP				
				backjamp |= JAMP_3_1STEP + JAMP_3_T * JAMP_3_SETTIMER;	//１段階ジャンプフラグ + タイマーセット
#endif				
			}
		}
#if DOSEI_JAMP_DOUBLE
		else if( onface==0 && spacekey<1 ){
#if !DOSEI_JAMP_MUGEN
			onface--;
#endif
			dosei.speed.y += JAMP_D_SPEED;
		}
#endif		
		spacekey++;	//スペースキーが押されている間はspacekey>0
	}else{
		spacekey=0;
	}

#if DOSEI_JAMP_BACKJAMP	
	if( ( backjamp & JAMP_B_TIMER ) && ( ( backjamp-- ) & JAMP_B_TIMER ) == 0 ) backjamp &= ~JAMP_B_SUCCEED;	//バックジャンプ用タイマーを進める。タイマーが0になったらフラグ消去
#endif	
	
	//どせい移動
	dosei.force = 0.0;
	if( keystate[ KEY_SK_LEFT ] || keystate[ KEY_LEFT ] ){	//left
		dosei.force += dash_svec * DOSEI_IDORYOU;
	}
	if( keystate[ KEY_SK_RIGHT ] || keystate[ KEY_RIGHT ] ){	//right
		dosei.force -= dash_svec * DOSEI_IDORYOU;
	}
	if( keystate[ KEY_SK_UP ] || keystate[ KEY_UP ] ){	//up
		dosei.force += dash_vec * DOSEI_IDORYOU;
#if DOSEI_JAMP_BACKJAMP		
		if( (backjamp & JAMP_B_BACK) && ( backjamp &= ~JAMP_B_BACK, backjamp & JAMP_B_TIMER ) ){	//下キーが押されていたかのフラグが立っていて、タイマーが正の場合バックジャンプ成功
			backjamp |= JAMP_B_SUCCEED + JAMP_B_SETTIMER;	//バックジャンブ成功フラグ、タイマーセット
		}else{
			backjamp |= JAMP_B_FORWORD;	//上キーが押されていたかのフラグ
			if( !( backjamp & JAMP_B_SUCCEED ) ) backjamp |= JAMP_B_SETTIMER;	//タイマーセット (バックジャンプ成功フラグが立っていなかった場合のみ)	
		}
#endif		
	}
	if( keystate[ KEY_SK_DOWN ] || keystate[ KEY_DOWN ] ){	//down
		dosei.force -= dash_vec * DOSEI_IDORYOU;
#if DOSEI_JAMP_BACKJAMP		
		if( (backjamp & JAMP_B_FORWORD) && (backjamp &= ~JAMP_B_FORWORD, backjamp & JAMP_B_TIMER) ){	//上キーが押されていたかのフラグが立っていて、タイマーが正の場合バックジャンプ成功
			backjamp |= JAMP_B_SUCCEED + JAMP_B_SETTIMER;	//バックジャンブ成功フラグ、タイマーセット
		}else{
			backjamp |= JAMP_B_BACK;	//下キーが押されていたかのフラグ
			if( !( backjamp & JAMP_B_SUCCEED ) ) backjamp |= JAMP_B_SETTIMER;	//タイマーセット
		}
#endif		
	}

	return;
}

//回転させる
void PlayerClass::addAngle(float rot){
	dosei.angle += rot;
	//どせいさんの向いてる方向計算
	dash_vec.x = -sin(dosei.angle/180 * M_PI);
	dash_vec.z = -cos(dosei.angle/180 * M_PI);
	dash_svec.x = dash_vec.z;
	dash_svec.z = -dash_vec.x;
}

//当たり判定前のスピードの更新
Vector3 PlayerClass::UpdateSpeed( float gravity ){
		dosei.speed.y -= gravity;	//重力
		Vector3 speed = dosei.speed;	//現在のどせいのスピード＋力
		if( (dosei.force.x<0 && speed.x>dosei.force.x)||(dosei.force.x>0 && speed.x<dosei.force.x) ) speed.x = dosei.force.x;
		if( (dosei.force.y<0 && speed.y>dosei.force.y)||(dosei.force.y>0 && speed.y<dosei.force.y) ) speed.y = dosei.force.y;
		if( (dosei.force.z<0 && speed.z>dosei.force.z)||(dosei.force.z>0 && speed.z<dosei.force.z) ) speed.z = dosei.force.z;

		//影用
		shadow.pos = dosei.pos;
		shadow.speed = 0.0;
		shadow.speed.y = -10000.0; //スピードのリセット

		return speed;	//当たり判定に使うspeedを返す
}

//着地時の処理
void PlayerClass::OnFace(bool on){
		if( on ){
			dosei.speed = 0.0;
			onface = 1;
#if DOSEI_JAMP_3STEP
			if( (backjamp & JAMP_3_TIMER) && ( ( backjamp -= JAMP_3_T ) & JAMP_3_TIMER) == 0 ){	//3段階ジャンプ用タイマーを進める
				backjamp &= ~( JAMP_3_1STEP + JAMP_3_2STEP );
				if( backjamp & JAMP_3_SUCCEED ){	//3段階ジャンプが成功していた場合
				//	if( game != GAME_CLEAR ){	
						dosei.pos.y = -45.0;
						dosei.pos.z = 0.0;
				//		game++;
				//		if( keystate['x'] ) game++;
				//	}
					backjamp &= ~JAMP_3_SUCCEED;	
				}
			}
#endif			
		}else if( onface > 0 ){
			onface = 0;
		}

}

void PlayerClass::Render(Vector3 speed){
	// 適用
	dosei.pos += speed;	//適用
	shadow.pos += shadow.speed;	//適用
	shadow.pos.y += 0.001;
	//どせいさん描画
	if( !PublicData->Key.state['x'] ){
		dosei.Render();
		shadow.Render(); //影表示
	}

}

