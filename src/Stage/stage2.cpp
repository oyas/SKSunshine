//stage2.cpp

// 迷路のステージ。javascriptで書いた迷路の移植版的なやつ。


#include "../assist/object.h"	//基本オブジェクト
#include "stage2.h"
#include "stage0.h"	//次のステージ


///////////////////////
// 設定

#define DOSEI_ON 0	//1:どせいを使う 0:くねくねを使う
#define DOSEI_IDORYOU (0.2 + Add_ido)	//どせい操作時の移動量
#define DOSEI_CAMRA 0.05	//カメラ操作時の移動量

#define DOSEI_JAMP_BACKJAMP 1	//バックジャンプの有無
#define DOSEI_JAMP_DOUBLE 0	//空中ジャンプの有無
#define DOSEI_JAMP_3STEP 0	//３段ジャンプの有無 (有効にすると別ゲーになります)
#define DOSEI_JAMP_MUGEN 0	//無限ジャンプの有無(チート)

#define JAMP_B_SETTIMER 7	//バックジャンプの判定時間（1〜7フレーム）
#define JAMP_3_SETTIMER 3	//３段階ジャンプの判定時間（2〜7フレーム）実際はこれの-1フレーム

#define JAMP_SPEED (0.4 + Add_jump)	//通常ジャンプ量
#define JAMP_B_SPEED 0.8	//バックジャンプのジャンプ量
#define JAMP_D_SPEED 0.5	//空中ジャンプのジャンプ量
#define JAMP_3_SPEED_2 0.5	//３段ジャンプの２段階目ジャンプ量
#define JAMP_3_SPEED_Y 2.6	//３段ジャンプのジャンプ量(上方向最大値)
#define JAMP_3_SPEED_Z 3.0	//３段ジャンプのジャンプ量(前方向最大値)

#define GAME_ZIKI 5		//初期自機数
#define GAME_GRAVITY 0.02	//重力

//////////////////////
// 定数定義
#define GAME_CLEAR 100	//クリア定数

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
static int Keys[] = {	//チェックするボタン
		'a',	//1
#define KEY_A 1
		's',	//2
#define KEY_S 2
		'w',	//4
#define KEY_W 4
		'z',	//8
#define KEY_Z 8
		'b',	//16
#define KEY_B 16
		'n',	//32
#define KEY_N 32
		' ',	//64
#define KEY_SPACE 64
		'x',	//128
#define KEY_X 128
		'4',	//256 left
#define KEY_LEFT 256
		'6',	//512 right
#define KEY_RIGHT 512
		'8',	//1024 up
#define KEY_UP 1024
		'5',	//2048 down
#define KEY_DOWN 2048
		'q',	//4096
#define KEY_Q 4096
		'e',	//8192
#define KEY_E 8192
		't',	//16384
#define KEY_T 16384
};
static int Keys_sk[] = {
		GLUT_KEY_LEFT,	//1
#define KEY_SK_LEFT 1
		GLUT_KEY_RIGHT,	//2
#define KEY_SK_RIGHT 2
		GLUT_KEY_UP,	//4
#define KEY_SK_UP 4
		GLUT_KEY_DOWN	//8
#define KEY_SK_DOWN 8
};

//移動量、ジャンプ量の変化用
static float Add_ido = 0.0;
static float Add_jump = 0.0;


/*-----------------------------------------------------------------------------------*
	ステージ2 クラス
 *-----------------------------------------------------------------------------------*/

//コンストラクタ（初期化）
Stage2::Stage2() : sound("whistle.wav")
	//板のコンストラクタの引数を渡してやる。メンバーイニシャライザ
	//(このコンストラクタが実行される前に、板のコンストラクタは実行される)
{
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
	dosei.pos.y = 10.0;
	dosei.pos.z = 0.0;
	//どせい向き
	dosei.angle = 180;
	dosei.ang.y = 1.0;
	//どせいに関するもの初期化
	dash_vec.x=0.0; dash_vec.y=0.0; dash_vec.z=1.0;	//どせいの前方向ベクトル
	dash_svec.x=1.0; dash_svec.y=0.0; dash_svec.z=0.0;	//どせいの横方向ベクトル

	//ライトの位置セット
	light0pos[0] = 1.0;
	light0pos[1] = 10.0;
	light0pos[2] = 5.0;
	light0pos[3] = 1.0;
	//カメラ角度
	cam_z=20.0; cam_r=M_PI; cam_rx=M_PI_4/2;
	//キー
	key_on=0; yaziru=0;
		
	//スコア用文字列
	str_a[0] = '\0';

	//当たり判定があったか
	onface = 0;
	spacekey = 0;

	//バックジャンプ用
	backjamp = 0;
	
	//////////////////////////
	//背景作成
	//背景画像読み込み
	float pos[4][3] = { {2.3,1.3,0.0}, {2.3,-1.3,0.0}, {-2.3,-1.3,0.0}, {-2.3,1.3,0.0} };
//	DisplayList_BACK = CreatePNGDisplayList("yama.png", &textureBACK, pos, false);//, image[0]);
	textureBACK = png_back.load("yama.png");
	DisplayList_BACK = png_back.CreateDisplayList(pos, false);
	//////////////////////////////
	
	//////////////////////////
	// MISS作成
	//画像読み込み
//	DisplayList_MISS = CreatePNGDisplayList("miss.png", &textureMISS, NULL, true);//, image[1]);
	textureMISS = png_miss.load("miss.png");
	DisplayList_MISS = png_miss.CreateDisplayList(NULL, true);
	//////////////////////////////
	
	//////////////////////////
	// CLEAR作成
	//画像読み込み
//	DisplayList_CLEAR = CreatePNGDisplayList("clear.png", &textureCLEAR, NULL, true);//, image[2]);
	textureCLEAR = png_clear.load("clear.png");
	DisplayList_CLEAR = png_clear.CreateDisplayList(NULL, true);
	//////////////////////////////
	
	//影
	shadow.Set("shadow.png", 0.8f);

	//ゲーム関連
	game = GAME_ZIKI;
	game_timer = 0;
	sprintf(str_a,"x%d",game);

	//どせい高速化用
	turbo = true;
	
	printf("stage2 初期化完了。\n");	//デバッグ用
}


//画面再描画時によばれる(1フレーム毎に呼ばれる)
void Stage2::Disp(){
	
	//背景描画
	glCallList(DisplayList_BACK);
	
	glLoadIdentity();
	
	//ライトの位置セット
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	
	//カメラの位置計算
	float _x,_y,_z;
	_x = sin(cam_r) * cos(cam_rx) * cam_z;
	_y = sin(cam_rx) * cam_z;
	_z = cos(cam_r) * cos(cam_rx) * cam_z;
	//カメラの位置セット
	gluLookAt(dosei.pos.x+_x, dosei.pos.y+_y, dosei.pos.z+_z, 
		dosei.pos.x,dosei.pos.y,dosei.pos.z, 0.0,1.0,0.0);
	
	//キー処理(tキーでどせい高速化)
	if( key_on & KEY_T ){
		if( turbo )
			if( Add_ido > 0.1 ){
				printf("移動高速化!!\n");
				Add_ido = 0.0;
				Add_jump = 0.0;
			}else{
				printf("移動高速化終了。\n");
				Add_ido = +0.3;
				Add_jump = -0.3;
			}
		turbo = false;
	}else{
		turbo = true;
	}

	//キー処理(どせいの移動)
	if(key_on & KEY_B){	//bキー
		dosei.angle += 1.0;
		//どせいさんの向いてる方向計算
		dash_vec.x = -sin(dosei.angle/180 * M_PI);
		dash_vec.z = -cos(dosei.angle/180 * M_PI);
		dash_svec.x = dash_vec.z;
		dash_svec.z = -dash_vec.x;
	}
	if(key_on & KEY_N){	//nキー
		dosei.angle -= 1.0;
		dash_vec.x = -sin(dosei.angle/180 * M_PI);
		dash_vec.z = -cos(dosei.angle/180 * M_PI);
		dash_svec.x = dash_vec.z;
		dash_svec.z = -dash_vec.x;
	}
	if(key_on & KEY_SPACE){	//space
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
				dosei.speed.y += JAMP_3_SPEED_Y * sin(cam_rx);	//上方向
				dosei.speed += dash_vec * JAMP_3_SPEED_Z * cos(cam_rx);	//前方向
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
	if( yaziru & KEY_SK_LEFT || key_on & KEY_LEFT ){	//left
		dosei.force += dash_svec * DOSEI_IDORYOU;
	}
	if(yaziru & KEY_SK_RIGHT || key_on & KEY_RIGHT ){	//right
		dosei.force -= dash_svec * DOSEI_IDORYOU;
	}
	if(yaziru & KEY_SK_UP || key_on & KEY_UP){	//up
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
	if(yaziru & KEY_SK_DOWN || key_on & KEY_DOWN ){	//down
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
		
	//カメラ移動
	if( key_on & KEY_A ){
		cam_r += DOSEI_CAMRA;
		if(cam_r>2*M_PI) cam_r -= 2*M_PI;
		//どせいも回転
		dosei.angle += DOSEI_CAMRA*180/M_PI;
		dash_vec.x = -sin(dosei.angle/180 * M_PI);
		dash_vec.z = -cos(dosei.angle/180 * M_PI);
		dash_svec.x = dash_vec.z;
		dash_svec.z = -dash_vec.x;
	}
	if( key_on & KEY_S ){
		cam_r -= DOSEI_CAMRA;
		if(cam_r<-2*M_PI) cam_r += 2*M_PI;
		//どせいも回転
		dosei.angle -= DOSEI_CAMRA*180/M_PI;
		dash_vec.x = -sin(dosei.angle/180 * M_PI);
		dash_vec.z = -cos(dosei.angle/180 * M_PI);
		dash_svec.x = dash_vec.z;
		dash_svec.z = -dash_vec.x;
	}
	if(key_on & KEY_Q && cam_z>5.0){
		cam_z -= DOSEI_CAMRA;
	}
	if(key_on & KEY_E && cam_z<30.0){
		cam_z += DOSEI_CAMRA;
	}
	if(key_on & KEY_W && cam_rx > -(M_PI_2-DOSEI_CAMRA-0.01)){
		cam_rx -= DOSEI_CAMRA;
	}
	if(key_on & KEY_Z && cam_rx < M_PI_2-DOSEI_CAMRA-0.01){
		cam_rx += DOSEI_CAMRA;
	}
	

	//救出(どせいが下に落ちた場合の処理)
	if(dosei.pos.y<-50.0){
		if(game > 0 && game < GAME_CLEAR){	//残機あり、クリアじゃない時
			game--;
			sprintf(str_a,"x%d",game);
		}
		if(game>0){
			//初期位置へ戻す
			dosei.pos.x = 0.0;
			dosei.pos.y = 10.0;
			dosei.pos.z = 0.0;
			dosei.speed = 0.0;
		}
	}
		
		//当たり判定 描画・物理処理等を各オブジェクトごとに行う
	{		
		dosei.speed.y -= GAME_GRAVITY;	//重力
		Vector3 speed = dosei.speed;	//現在のどせいのスピード＋力
		if( (dosei.force.x<0 && speed.x>dosei.force.x)||(dosei.force.x>0 && speed.x<dosei.force.x) ) speed.x = dosei.force.x;
		if( (dosei.force.y<0 && speed.y>dosei.force.y)||(dosei.force.y>0 && speed.y<dosei.force.y) ) speed.y = dosei.force.y;
		if( (dosei.force.z<0 && speed.z>dosei.force.z)||(dosei.force.z>0 && speed.z<dosei.force.z) ) speed.z = dosei.force.z;
		Vector3 bspeed = speed;	//当たり判定前のspeedを保存

		//影用
		shadow.pos = dosei.pos;
		shadow.speed = 0.0;
		shadow.speed.y = -10000.0; //スピードのリセット
	
		
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
		
		//オブジェクトグループ１
		objg1.Disp_Colli( dosei.pos, speed, shadow );
		
		//オブジェクトグループ迷路
		objg_meiro.Disp_Colli( dosei.pos, speed, shadow );
		
		//摩擦 接地していたら止まる
		if( speed.y-bspeed.y != 0.0){
			dosei.speed = 0.0;
			onface = 1;
#if DOSEI_JAMP_3STEP
			if( (backjamp & JAMP_3_TIMER) && ( ( backjamp -= JAMP_3_T ) & JAMP_3_TIMER) == 0 ){	//3段階ジャンプ用タイマーを進める
				backjamp &= ~( JAMP_3_1STEP + JAMP_3_2STEP );
				if( backjamp & JAMP_3_SUCCEED ){	//3段階ジャンプが成功していた場合
					if( game != GAME_CLEAR ){	
						dosei.pos.y = -45.0;
						dosei.pos.z = 0.0;
						game++;
						if( key_on & KEY_X ) game++;
					}
					backjamp &= ~JAMP_3_SUCCEED;	
				}
			}
#endif			
		}else if( onface > 0 ){
			onface = 0;
		}
		dosei.pos += speed;	//適用

		shadow.pos += shadow.speed;	//適用
		shadow.pos.y += 0.001;
		
	
	//どせいさん描画
	if( !(key_on & KEY_X) ){
		dosei.Render();
		shadow.Render(); //影表示
	}
	
	//テクスチャマッピング無効
	//glDisable(GL_BLEND);  //ブレンド
	glDisable(GL_TEXTURE_2D);
	}
	
	//FPS出力
	fps(true);
/*	fpstxt = fps();
	if(strcmp(fpstxt, fpstxtb)){
		strcpy(fpstxtb, fpstxt);
		printf("%s\n",fpstxt);
	}
*/
	//サウンド
	sound.stream();
	
	
	//クリア判定
	if( objg_meiro.onCLEAR ){
		game = GAME_CLEAR;
	}
	//MISS または CLEAR の時の処理
	if( game == GAME_CLEAR ){	//CLEAR
		glCallList(DisplayList_CLEAR);	//クリア表示
		if(game_timer++ > 120){
			ChangeStage(new Stage0);	//Stage0に戻る
		}
	}else if( game <= 0 ){	//MISS
		glCallList(DisplayList_MISS);	//ミス表示
		if(game_timer++ > 60){
			ChangeStage(new Stage0);
		}
	}
}

//2Dの描画
void Stage2::Disp2D(int Width, int Height){
	glColor4f(0.0,0.0,0.0, 1.0);	//カラー
	DRAW_STRING(10, 10, str_a, GLUT_BITMAP_TIMES_ROMAN_24);
}

//入力処理
void Stage2::Input(char event, int key, int x, int y){
	//ボタン状態チェック
	if( event==SC_INPUT_KEY_DOWN ){
		SetKeyState(&key_on, key, true, Keys, sizeof(Keys) );
	}else if( event==SC_INPUT_KEY_UP ){
		SetKeyState(&key_on, key, false, Keys, sizeof(Keys) );
	}else if( event==SC_INPUT_SPECIALKEY_DOWN ){
		SetKeyState(&yaziru, key, true, Keys_sk, sizeof(Keys_sk) );
	}else if( event==SC_INPUT_SPECIALKEY_UP ){
		SetKeyState(&yaziru, key, false, Keys_sk, sizeof(Keys_sk) );
	}
}

//デストラクタ（終了処理）
Stage2::~Stage2(){
	glDeleteTextures(1, &textureBACK);
	glDeleteLists(DisplayList_BACK, 1);
	glDeleteTextures(1, &textureMISS);
	glDeleteLists(DisplayList_MISS, 1);
	glDeleteTextures(1, &textureCLEAR);
	glDeleteLists(DisplayList_CLEAR, 1);
	printf("~Stage2\n");
}



/*-----------------------------------------------------------------------------------*
	迷路
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
OBJgroup_Meiro::OBJgroup_Meiro()
{
	//乱数初期化
	srand( (unsigned)time(NULL) );

	onCLEAR = false;
	
	DisplayList = 0;
	
	//迷路用配列
	for(int x=0; x<X_RETU; x++ ){
		for(int y=0; y<Y_GYO; y++ ){	
			miti[x][y] = 0;
			if( y < Y_GYO-1 ) yokoba[x][y] = 0;
			if( x < X_RETU-1 ) tateba[x][y] = 0;
		}
	}
	
	//ゴール関係
	miti_max=0;	//スタートからの最大の距離
	miti_x=0; miti_y=0;	//↑そのポイントの座標(ゴールの座標)
	
	//前回チェックした場所（そこから道をのばした）
	check_x = 0; check_y = 0;
	
	//現在道をのばすか検討中の場所
	now_x = 0; now_y = 0;
	
	end_f = false; //終わったかどうか
	
	//スタート位置指定
	miti[X_RETU/2][0] = 1;
	
	//ボックス作成
	Vector3 v( BOX_SIZE/2.0, BOX_SIZE/2.0, BOX_SIZE/2.0 );
	
	box.Set(&v,"box2.png");
	box.pos = 0.0;
	box_goal.Set(&v,"box3.png");
	box_goal.pos = 0.0;
	
	//位置セット
	pos = 0.0;
	pos.x = -((float)X_RETU * BOX_SIZE );
	pos.z = +50.0 ;
	
	//初期位置合わせ
	check();
}

void OBJgroup_Meiro::Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow)
{
	Vector3 v( _pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z );	
	
	//続きの迷路作成
	if( !end_f && nobasu() ) check();
	
	
	glPushMatrix();
	
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	
	if( !end_f ){
		//迷路作成途中
		
		//描画
		for(int x=0; x<X_RETU; x++ ){
			for(int y=0; y<Y_GYO; y++ ){
				//道	
				if( miti[x][y] ){
					box.pos.x = x * BOX_SIZE * 2.0;	
					box.pos.z = y * BOX_SIZE * 2.0;	
					box.Render();
				}
				//横つなぎ
				if( y < Y_GYO-1 && yokoba[x][y] == 1 ){
					box.pos.x = x * BOX_SIZE * 2.0;
					box.pos.z = y * BOX_SIZE * 2.0 + BOX_SIZE;
					box.Render();
				}
				//縦つなぎ
				if( x < X_RETU-1 && tateba[x][y] == 1 ){
					box.pos.x = x * BOX_SIZE * 2.0 + BOX_SIZE;
					box.pos.z = y * BOX_SIZE * 2.0;
					box.Render();
				}
			}
		}
		
	}else if( !DisplayList ){
		//迷路完成後
		
		DisplayList = glGenLists(1);//ディスプレイリストを作成
		glNewList(DisplayList, GL_COMPILE_AND_EXECUTE); //保存と実行
		
		//描画
		for(int x=0; x<X_RETU; x++ ){
			for(int y=0; y<Y_GYO; y++ ){
				//道	
				if( miti[x][y] ){
					box.pos.x = x * BOX_SIZE * 2.0;	
					box.pos.z = y * BOX_SIZE * 2.0;	
					box.Render();
				}
				//横つなぎ
				if( y < Y_GYO-1 && yokoba[x][y] == 1 ){
					box.pos.x = x * BOX_SIZE * 2.0;
					box.pos.z = y * BOX_SIZE * 2.0 + BOX_SIZE;
					box.Render();
				}
				//縦つなぎ
				if( x < X_RETU-1 && tateba[x][y] == 1 ){
					box.pos.x = x * BOX_SIZE * 2.0 + BOX_SIZE;
					box.pos.z = y * BOX_SIZE * 2.0;
					box.Render();
				}
			}
		}
	
		box_goal.Render();	//ゴールマス

		glEndList();

		printf("迷路用ディスプレイリスト作成完了！\n");
	}else{
		//迷路・ディスプレイリストともに完成後
		glCallList( DisplayList );	//ディスプレイリストで描画
	}
	
	glPopMatrix();
	
	//当たり判定
	int x = (int)(v.x/BOX_SIZE/2.0);
	int y = (int)(v.z/BOX_SIZE/2.0);
	if( end_f && 
			( x == miti_x || x == miti_x-1 ) &&
			( y == miti_y || y == miti_y-1 ) ){	//ゴールマスに乗ったか判定
		box_goal.NormalForce( v, _speed );
		if( box_goal.laston & 1 )
			onCLEAR = true;
	}
	if( x>=0 && y>=0 && x<X_RETU && y<Y_GYO ){
		box_colli(x, y, box, v, _speed, shadow);	//中央
		if( x > 0 )
			box_colli(x-1, y, box, v, _speed, shadow);	//中央-x
		if( x < X_RETU-1 )
			box_colli(x+1, y, box, v, _speed, shadow);	//中央+x
		if( y > 0 )
			box_colli(x, y-1, box, v, _speed, shadow);	//中央-y
		if( y < Y_GYO-1 )
			box_colli(x, y+1, box, v, _speed, shadow);	//中央-y
		if( x > 0 && y > 0 )
			box_colli(x-1, y-1, box, v, _speed, shadow);	//中央-x-y
		if( x > 0 && y < Y_GYO-1 )
			box_colli(x-1, y+1, box, v, _speed, shadow);	//中央-x+y
		if( x < X_RETU-1 && y > 0)
			box_colli(x+1, y-1, box, v, _speed, shadow);	//中央+x-y
		if( x < X_RETU-1 && y < Y_GYO-1)
			box_colli(x+1, y+1, box, v, _speed, shadow);	//中央+x+y
	}
		
}

//伸ばし始めを決める
void OBJgroup_Meiro::check()
{
	int check_sx = check_x, check_sy = check_y;	//はじまり
	bool suru = false;
	while(1){
		//次へ
		check_x++;
		if( check_x >= X_RETU ){
			check_y++;
			check_x=0;
			if( check_y >= Y_GYO ) check_y=0;
		}
		//道じゃなかったらcontinue
		if( miti[check_x][check_y] == 0 ){
			suru = true;
			continue;
		}
		//つながってないとこあるかチェック
		if( tuna( check_x, check_y ) ){
			if( rand() % 5 == 0 ){	//ここからにするかどうか
				now_x = check_x;
				now_y = check_y;
				break;
			}else{
				suru = true;
			}
		}
		if( check_sx==check_x && check_sy==check_y ){	//一周したら
			if( suru ){
				suru = false;	//suruをfalseにしてもう一周
			} else {
				end_f = true;
				break;	//全部埋まったらbreak
			}
		}
	}	//伸ばしはじめを決めるまでループ
	
	//エンドフラグが立っていたら終わり。ゴール位置セット
	if( end_f ){
		miti[miti_x][miti_y] = 0;
		box_goal.pos.x = miti_x * BOX_SIZE * 2.0;
		box_goal.pos.z = miti_y * BOX_SIZE * 2.0;
	}
}

//伸ばす
bool OBJgroup_Meiro::nobasu()
{
	if( rand() % 30 == 0 ){	//30分の１の確率で伸ばすの終わり
		return true;
	}else{
		//方向決定
		int a = tuna(now_x,now_y);
		if( !a ) return true;
		int b = rand() % 4;
		while( !( a & (1<<b) ) ){
			b = rand() % 4;
		}
		int houkou_x = 0;
		int houkou_y = 0;
		switch(b){
			case 0:	//左
				houkou_x = -1;
				break;
			case 1:	//右
				houkou_x = 1;
				break;
			case 2:	//上
				houkou_y = -1;
				break;
			case 3:	//した
				houkou_y = 1;
				break;
		}
		
		//距離決定
		int out;
		b = 0;
		for( a=0; a<ST_MAX; a++){
			if((houkou_x != 0)&&((now_x+houkou_x*a<0)||(now_x+houkou_x*a>=X_RETU))) break;
			if((houkou_y != 0)&&((now_y+houkou_y*a<0)||(now_y+houkou_y*a>=Y_GYO))) break;
			out = tuna(now_x+houkou_x*a, now_y+houkou_y*a);
			if((houkou_y==-1&&(out&4))||(houkou_y==1&&(out&8))||(houkou_x==-1&&(out&1))||(houkou_x==1&&(out&2))){
				b++;
			}else{
				break;
			}
		}
		int kyori = rand()%b + 1;
		
		//道のばす
		for( int aa=0; aa<kyori; aa++){
			//伸ばす
			now_x += houkou_x;
			now_y += houkou_y;
			
			//壁をなくす
			if(houkou_x==1){

				tateba[now_x-1][now_y] = 1;

			}else if(houkou_x==-1){

				tateba[now_x][now_y] = 1;

			}else if(houkou_y==1){

				yokoba[now_x][now_y-1] = 1;

			}else if(houkou_y==-1){

				yokoba[now_x][now_y] = 1;

			}

			//ゴール関係
			a = miti[now_x-houkou_x][now_y-houkou_y]+1;	//スタートからの距離を計算
			miti[now_x][now_y] = a;	//記録
			if( miti_max<a ){
				miti_max=a;
				miti_x=now_x;
				miti_y=now_y;
			}	//ゴール位置更新
			
		}
	}
	return false;
}

//つながってないとこあるかチェック
//1:left  2:right  4:up  8:down
int OBJgroup_Meiro::tuna(int x,int y)
{
	int out = 0;
	if(x-1 >= 0) if(miti[x-1][y] == 0) out = out|1;
	if(x+1 < X_RETU) if(miti[x+1][y] == 0) out = out|2;
	if(y-1 >= 0) if(miti[x][y-1] == 0) out = out|4;
	if(y+1 < Y_GYO) if(miti[x][y+1] == 0) out = out|8;
	return out;
}

//当たり判定用（１マスごと）
void OBJgroup_Meiro::box_colli(int x, int y, boxOBJ &box, Vector3 &v, Vector3 &_speed, ShadowOBJ &shadow )
{	
	//道	
	box.pos.x = x * BOX_SIZE * 2.0;	
	box.pos.z = y * BOX_SIZE * 2.0;	
	box.NormalForce(v, _speed);
	box.NormalForceShadow(v, shadow);
	//横つなぎ
	if( y < Y_GYO-1 && yokoba[x][y] == 1 ){
		box.pos.x = x * BOX_SIZE * 2.0;
		box.pos.z = y * BOX_SIZE * 2.0 + BOX_SIZE;
		box.NormalForce(v, _speed);
		box.NormalForceShadow(v, shadow);
	}
	//縦つなぎ
	if( x < X_RETU-1 && tateba[x][y] == 1 ){
		box.pos.x = x * BOX_SIZE * 2.0 + BOX_SIZE;
		box.pos.z = y * BOX_SIZE * 2.0;
		box.NormalForce(v, _speed);
		box.NormalForceShadow(v, shadow);
	}
}



