//stage1.cpp


#include "stage1.h"
#include "stage0.h"	//次のステージ


///////////////////////
// 設定

#define DOSEI_ON 0	//1:どせいを使う 0:くねくねを使う
#define DOSEI_IDORYOU 0.2	//どせい操作時の移動量
#define DOSEI_CAMRA 0.05	//カメラ操作時の移動量

#define DOSEI_JAMP_BACKJAMP 1	//バックジャンプの有無
#define DOSEI_JAMP_DOUBLE 1	//空中ジャンプの有無
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

#define GAME_ZIKI 5		//初期自機数
#define GAME_GRAVITY 0.02	//重力

//////////////////////
// 定数定義
#define GAME_CLEAR 100	//クリア

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
		'e'		//8192
#define KEY_E 8192
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



/*-----------------------------------------------------------------------------------*
	ステージ1 クラス
 *-----------------------------------------------------------------------------------*/

//コンストラクタ（初期化）
Stage1::Stage1() : sound("whistle.wav")
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
		
	//スコア初期化
	//total=0;
	//スコア用文字列
//	str[0]='\0'; str_t[0]='\0'; 
	str_a[0] = '\0';
	//情報表示
	std::cout << "Vendor :"<<glGetString(GL_VENDOR)<<'\n';
	std::cout << "GPU : "<<glGetString(GL_RENDERER) <<'\n';
	std::cout << "OpenGL ver. " <<glGetString(GL_VERSION)<<'\n';

	//当たり判定があったか
	onface = 0;
	spacekey = 0;

	//バックジャンプ用
	backjamp = 0;
	
	//////////////////////////
	//背景作成
	//背景画像読み込み
	float pos[4][3] = { {2.2,1.3,0.0}, {2.2,-1.3,0.0}, {-2.2,-1.3,0.0}, {-2.2,1.3,0.0} };
	DisplayList_BACK = CreatePNGDisplayList("yama.png", &textureBACK, pos, false);
	//////////////////////////////
	
	//////////////////////////
	// MISS作成
	//画像読み込み
	DisplayList_MISS = CreatePNGDisplayList("miss.png", &textureMISS, NULL, true);
	//////////////////////////////
	
	//////////////////////////
	// CLEAR作成
	//画像読み込み
	DisplayList_CLEAR = CreatePNGDisplayList("clear.png", &textureCLEAR, NULL, true);
	//////////////////////////////
	
	//ゲーム関連
	game = GAME_ZIKI;
	game_timer = 0;
	sprintf(str_a,"x%d",game);
	
	printf("stage1 初期化完了。\n");	//デバッグ用
}


//画面再描画時によばれる(1フレーム毎に呼ばれる)
void Stage1::Disp(){
	
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
			dosei.pos.z = (dosei.pos.z>210.0 ? 210.0 : 0.0);	//アイスブロックまでいってたらその直前から再開
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
		
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
		
		//オブジェクトグループ１
		objg1.Disp_Colli( dosei.pos, speed );
		
		//オブジェクトグループ2
		objg2.Disp_Colli( dosei.pos, speed );
		
		//オブジェクトグループ3
		objg3.Disp_Colli( dosei.pos, speed );
		
		//オブジェクトグループ4
		objg4.Disp_Colli( dosei.pos, speed );
		
		//オブジェクトグループ5
		objg5.Disp_Colli( dosei.pos, speed );
		
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
		
	
	//どせいさん描画
	if( !(key_on & KEY_X) ) dosei.Render();
	
	//テクスチャマッピング無効
	//glDisable(GL_BLEND);  //ブレンド
	glDisable(GL_TEXTURE_2D);
	}
	
	//FPS出力
	fpstxt = fps();
	if(strcmp(fpstxt, fpstxtb)){
		strcpy(fpstxtb, fpstxt);
		printf("%s\n",fpstxt);
	}

// backjamp デバッグ用
/*
printf("backjamp\tタイマー:%d\t%o\t",backjamp&7,backjamp&(128+256));
for(int b=0; b<16; b++){
	int k=1;
	printf("%x",(backjamp&(k << b)) >> b );
}
printf("\n");
*/

	//サウンド
	sound.stream();
	
	
	//クリア判定
	if( objg5.onCLEAR ){
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
void Stage1::Disp2D(int Width, int Height){
	glColor4f(0.0,0.0,0.0, 1.0);	//カラー
	DRAW_STRING(10, 10, str_a, GLUT_BITMAP_TIMES_ROMAN_24);
//	DRAW_STRING(10, 75, str_t, GLUT_BITMAP_TIMES_ROMAN_24);
//	DRAW_STRING(10, 50, str, GLUT_BITMAP_TIMES_ROMAN_24);
}

//入力処理
void Stage1::Input(char event, int key, int x, int y){
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
Stage1::~Stage1(){
	glDeleteTextures(1, &textureBACK);
	glDeleteLists(DisplayList_BACK, 1);
	glDeleteTextures(1, &textureMISS);
	glDeleteLists(DisplayList_MISS, 1);
	glDeleteTextures(1, &textureCLEAR);
	glDeleteLists(DisplayList_CLEAR, 1);
	printf("~Stage1\n");
}


/*-----------------------------------------------------------------------------------*
	オブジェクトグループ1
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
OBJgroup1::OBJgroup1() : ita(24.0, 24.0)
{
	//初期位置セット
	Vector3 v = { 3.0, 3.0, 3.0 };
	
	box[0].Set(&v,"box1.png");
	box[0].pos.z = 15.0;//-6.0;
//	box[0].pos.y = -0.002; //colli.cppを修正したため、ずらす必要がなくなった。
	
	box[1].Set(&v,"box1.png");
	box[1].pos.z = 25.0;//5.0;
	
	v.z = 4.0;
	box[2].Set(&v,"box1.png");
	box[2].pos.z = 31.9;
	box[2].pos.y = 3.0;
	
	//Ita初期化
	ita.pos.y = 3.0;
	//当たり判定用、初期化
	colliIta.Set_xz4(ita.vertex);
	//直方体OBJ登録
	Vector3 v1[3] = { {4.8, 2.2, 2.2},
						{-0.2, 7.0, -6.0},
						{-1.6, 1.8, 2.0}  };
	box1.Set(v1, "box2.png");	//直方体
	box1.pos.z = 42.0;
	box1.pos.y = 3.0;
		
	//バウンディングスフィア計算
	Vector3 Max, min;
	Max = 0.0; min = 0.0;
	min.z = -24.0;
	//Vector3 Max=box[0].pos, min=box[0].pos;
	for(int i=0; i<3; i++){
		float r = box[i].radius();
		//最大値比較
		if( Max.x < box[i].pos.x + r ) Max.x = box[i].pos.x + r;
		if( Max.y < box[i].pos.y + r ) Max.y = box[i].pos.y + r;
		if( Max.z < box[i].pos.z + r ) Max.z = box[i].pos.z + r;
		//最小値比較
		if( min.x > box[i].pos.x - r ) min.x = box[i].pos.x - r;
		if( min.y > box[i].pos.y - r ) min.y = box[i].pos.y - r;
		if( min.z > box[i].pos.z - r ) min.z = box[i].pos.z - r;
	}
	Max.z = box1.pos.z + box1.radius() + 10.0;
	
	//中心と半径を求める
	Max -= min; Max = Max * 0.5;	//Maxとminの中間点が中央
	radius = sqrt( Max.x*Max.x + Max.x*Max.x + Max.y*Max.y + Max.z*Max.z );	//半径
	Vector3 offset = min + Max;	//中心へのベクトル
	for(int i=0; i<3; i++){
		box[i].pos -= offset;
	}
	ita.pos -= offset;
	box1.pos -= offset;
	
	//位置セット
	pos = 0.0; pos += offset;
	//pos.z += 20.99;
	pos.y -= 3.0;
}

void OBJgroup1::Disp_Colli(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z };	
	
	//1番目の緑の箱の回転
	MATRIX4x4 m;
	float rot=0.01;
	MatrixIdentity(m.m);
	m.m[0][0] = cos(rot);
	m.m[1][0] = sin(rot);
	m.m[0][1] = -m.m[1][0];
	m.m[1][1] = m.m[0][0];
	box1.addrot(m);
		
		//箱OBJ
		Vector3 _v;	//box1.NormalForce()でvが破壊されるため、一時的な変数を用意
		_v = v;
		box1.NormalForce( _v, _speed);
		_pos += _v - v;	//変化分を適用
		v = _v;
		
	glPushMatrix();
	
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	
	for(int i=0; i<3; i++){
		box[i].Render();
	}
	//床描画
	ita.Render();
	//直方体描画
	box1.Render();
	
	glPopMatrix();
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > radius+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		return;
	}

	//当たり判定
	for(int i=0; i<3; i++){
		box[i].NormalForce(v, _speed);
	}
		//床１
		Vector3 nvec;
		nvec = colliIta.GetNormalForce(_pos, _speed);	//垂直抗力計算
		_speed.y += nvec.y;
}


/*-----------------------------------------------------------------------------------*
	オブジェクトグループ2
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
OBJgroup2::OBJgroup2()
{
	//初期位置セット
	Vector3 v = { 3.0, 3.0, 3.0 };
	
	box[0].Set(&v, "box2.png");
	box[0].pos.z = -15.0;
	box_pos[0].z = -9.0;
	
	box[1].Set(&v, "box2.png");
	box_pos[1].z = 9.0;
	
	box[2].Set(&v, "box2.png");
	box_pos[2].z = 28.0;
	
	v.x = 5.0;
	v.z = 5.0;
	box[3].Set(&v, "box.png");
	box[3].pos.z = 43.0;

	//バウンディングスフィア計算
	Vector3 Max=box[0].pos, min=box[0].pos;
	for(int i=0; i<4; i++){
		float r = box[i].radius();
		//最大値比較
		if( Max.x < box[i].pos.x + r ) Max.x = box[i].pos.x + r;
		if( Max.y < box[i].pos.y + r ) Max.y = box[i].pos.y + r;
		if( Max.z < box[i].pos.z + r ) Max.z = box[i].pos.z + r;
		//最小値比較
		if( min.x > box[i].pos.x - r ) min.x = box[i].pos.x - r;
		if( min.y > box[i].pos.y - r ) min.y = box[i].pos.y - r;
		if( min.z > box[i].pos.z - r ) min.z = box[i].pos.z - r;
	}
	
	//中心と半径を求める
	Max -= min; Max = Max * 0.5;	//Maxとminの中間点が中央
	radius = sqrt( Max.x*Max.x + Max.x*Max.x + Max.y*Max.y + Max.z*Max.z );	//半径
	Vector3 offset = min + Max;	//中心へのベクトル
	for(int i=0; i<3; i++){
		box_pos[i] -= offset;
	}
	box[3].pos -= offset;
	
	//位置セット
	pos = 0.0; pos += offset;
	pos.z += 65.0;
	pos.y -= 2.0;
	
	//rot初期化
	rot = 0.0;
}

void OBJgroup2::Disp_Colli(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z }, move;	
	
	//box移動
	float rcos = cos(rot);
	float rsin = sin(rot);
	//box1
	move.x = rcos * 6.0 + box_pos[0].x - box[0].pos.x;
	move.y = 0.0;
	move.z = rsin * 6.0 + box_pos[0].z - box[0].pos.z;
	box[0].addpos(move);
	//box2
	move.x = rcos * 6.0 + box_pos[1].x - box[1].pos.x;
	move.y = 0.0;
	move.z = -rsin * 6.0 + box_pos[1].z - box[1].pos.z;
	box[1].addpos(move);
	//box3
	move.x = 0.0; move.y=0.0;
	move.z = rsin * 7.0 + box_pos[2].z - box[2].pos.z;
	box[2].addpos(move);
	//回転角を増やす
	rot += 0.02;
	if(rot > 2*M_PI) rot -= 2*M_PI;
	
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	
	for(int i=0; i<4; i++){
		box[i].Render();
	}
	
	glPopMatrix();
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > ( radius + sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ) ){
		return;
	}

	//当たり判定
	for(int i=0; i<4; i++){
		box[i].NormalForce(v, _speed);
	}
}



/*-----------------------------------------------------------------------------------*
	オブジェクトグループ3
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
OBJgroup3::OBJgroup3()
{
	//初期位置セット
	Vector3 v = { 3.0, 0.1, 3.0 };
	
	//セット
	for(int i=0; i<8; i++)
		box[i].Set(&v, "box3.png");
	
	for(int i=8; i<15; i++)
		box[i].Set(&v, "box4.png");
	
	v.x = 9.0; v.y=6.0; v.z = 6.0;
	box[15].Set(&v, "box.png");
	
	//位置
	for(int i=0; i<15; i++) box_pos[i] = 0.0;
	//0
	box[0].pos.x = 15.0;
	box[0].pos.z = 3.0;
	box_pos[0].x = 15.0;
	box_pos[0].z = 9.0;
	//1
	box[1].pos.x = -9.0;
	box[1].pos.z = 3.0;
	box_pos[1].x = -9.0;
	box_pos[1].z = 9.0;
	//2
	box_pos[2].x = 3.0;
	box_pos[2].z = 27.0;
	//3
	box_pos[3].x = -3.0;
	box_pos[3].z = 33.0;
	//4
	box_pos[4].x = 9.0;
	box_pos[4].z = 51.0;
	//5
	box_pos[5].x = 15.0;
	box_pos[5].z = 63.0;
	//6
	box_pos[6].x = 3.0;
	box_pos[6].z = 63.0;
	//7
	box_pos[7].x = 3.0;
	box_pos[7].z = 81.0;
	//
	//8
	box_pos[8].x = 3.0;
	box_pos[8].z = 9.0;
	//9
	box_pos[9].x = 15.0;
	box_pos[9].z = 21.0;
	//10
	box_pos[10].x = -9.0;
	box_pos[10].z = 27.0;
	//11
	box_pos[11].x = 9.0;
	box_pos[11].z = 33.0;
	//12
	box_pos[12].x = -3.0;
	box_pos[12].z = 51.0;
	//13
	box_pos[13].x = -9.0;
	box_pos[13].z = 57.0;
	//14
	box_pos[14].x = 3.0;
	box_pos[14].z = 69.0;
	//
	//15
	box[15].pos.x = 3.0;
	box[15].pos.y = -6.0;
	box[15].pos.z = 96.0;
	
	
	//バウンディングスフィア計算
	Vector3 Max=box[0].pos, min=box[0].pos;
	for(int i=0; i<16; i++){
		float r = box[i].radius();
		//最大値比較
		if( Max.x < box[i].pos.x + r ) Max.x = box[i].pos.x + r;
		if( Max.y < box[i].pos.y + r ) Max.y = box[i].pos.y + r;
		if( Max.z < box[i].pos.z + r ) Max.z = box[i].pos.z + r;
		//最小値比較
		if( min.x > box[i].pos.x - r ) min.x = box[i].pos.x - r;
		if( min.y > box[i].pos.y - r ) min.y = box[i].pos.y - r;
		if( min.z > box[i].pos.z - r ) min.z = box[i].pos.z - r;
	}
	
	//中心と半径を求める
	Max -= min; Max = Max * 0.5;	//Maxとminの中間点が中央
	radius = sqrt( Max.x*Max.x + Max.x*Max.x + Max.y*Max.y + Max.z*Max.z );	//半径
	Vector3 offset = min + Max;	//中心へのベクトル
	//オフセット分移動
	for(int i=0; i<15; i++){
		box_pos[i] -= offset;
	}
	box[15].pos -= offset;
	
	//全体の位置セット
	pos = 0.0; pos += offset;
	pos.x -= 3.0;
	pos.y += 1.0;
	pos.z += 113.0;
	
	//rot初期化
	rot = 0.0;
	box_action = 0;
}

void OBJgroup3::Disp_Colli(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z }, move;	
	
	//box移動
	float rcos = cos(rot);
	float rsin = sin(rot);
	//box1
	for(int i=0; i<8; i++){
		if(i==6){
			move.x = rsin * 6.0 + box_pos[i].x - box[i].pos.x;
			move.y = box_pos[i].y - box[i].pos.y;
			move.z = box_pos[i].z - box[i].pos.z;
		}else{
			move.x = box_pos[i].x - box[i].pos.x;
			move.y = box_pos[i].y - box[i].pos.y;
			move.z = rsin * 6.0 + box_pos[i].z - box[i].pos.z;
		}
		box[i].addpos(move);
	}
	//box2
	for(int i=8; i<15; i++){
		if(i==9 || i==14){
			move.x = -rsin * 6.0 + box_pos[i].x - box[i].pos.x;
			move.y = box_pos[i].y - box[i].pos.y;
			move.z = box_pos[i].z - box[i].pos.z;
		}else{
			move.x = box_pos[i].x - box[i].pos.x;
			move.y = box_pos[i].y - box[i].pos.y;
			move.z = -rsin * 6.0 + box_pos[i].z - box[i].pos.z;
		}
		box[i].addpos(move);
	}
	//box3
	//move.x = 0.0; move.y=0.0;
	//move.z = rsin * 7.0 + box_pos[2].z - box[2].pos.z;
	//box[2].addpos(move);
	//回転角を増やす
	rot += 0.02;
	if(rot > 2*M_PI) rot -= 2*M_PI;
	
	
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	
	for(int i=0; i<16; i++){
		box[i].Render();
	}
	
	glPopMatrix();
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > radius+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		return;
	}

	//当たり判定
	for(int i=0; i<16; i++){
		box[i].NormalForce(v, _speed);
	}
}



/*-----------------------------------------------------------------------------------*
	オブジェクトグループ4
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
OBJgroup4::OBJgroup4()
{
	//初期位置セット
	Vector3 v = { 3.0, 3.0, 3.0 };
	
	for(int i=0; i<12; i++){
		for(int j=0; j<6; j++){
			box[i*6+j].Set(&v, "box5.png");
			box[i*6+j].pos.x = j*6.0;
			box[i*6+j].pos.y = 0.0;
			box[i*6+j].pos.z = i*6.0;
		}
	}
	
	
	//バウンディングスフィア計算
	Vector3 Max=box[0].pos, min=box[0].pos;
	for(int i=0; i<12*6; i++){
		float r = box[i].radius();
		//最大値比較
		if( Max.x < box[i].pos.x + r ) Max.x = box[i].pos.x + r;
		if( Max.y < box[i].pos.y + r ) Max.y = box[i].pos.y + r;
		if( Max.z < box[i].pos.z + r ) Max.z = box[i].pos.z + r;
		//最小値比較
		if( min.x > box[i].pos.x - r ) min.x = box[i].pos.x - r;
		if( min.y > box[i].pos.y - r ) min.y = box[i].pos.y - r;
		if( min.z > box[i].pos.z - r ) min.z = box[i].pos.z - r;
	}
	
	//中心と半径を求める
	Max -= min; Max = Max * 0.5;	//Maxとminの中間点が中央
	radius = sqrt( Max.x*Max.x + Max.x*Max.x + Max.y*Max.y + Max.z*Max.z );	//半径
	Vector3 offset = min + Max;	//中心へのベクトル
	//オフセット
	for(int i=0; i<12*6; i++){
		box[i].pos -= offset;
	}
	
	//位置セット
	pos = 0.0; pos += offset;
	pos.x -= 15.0;
	pos.y -= 2.0;
	pos.z += 220.0;
	
	//各変数初期化
	box_action = 0;
	rot = 0.0;
	//行列
	float rot = M_PI_4 / 4;	//45/2度(16回で一周)
	float rcos=cos(rot), rsin=sin(rot);	//sin,cos計算
	//x軸回転
	MatrixIdentity(matx.m);
	matx.m[1][1] = rcos; matx.m[2][1] = -rsin;
	matx.m[1][2] = rsin; matx.m[2][2] = rcos;
	//z軸回転
	MatrixIdentity(matz.m);
	matz.m[0][0] = rcos; matz.m[1][0] = -rsin;
	matz.m[0][1] = rsin; matz.m[1][1] = rcos;
	//z軸回転(逆回転)
	MatrixIdentity(mat_z.m);
	mat_z.m[0][0] = rcos; mat_z.m[1][0] = rsin;
	mat_z.m[0][1] = -rsin; mat_z.m[1][1] = rcos;
}

void OBJgroup4::Disp_Colli(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z };	
	bool on=true;
	int a=0;
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > radius+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		on = false;
	}
	
	switch(box_action){
	case 0:
		if(on) box_action++;
		break;
	case 1:		//手前から奥へ回転 行ごと
		for(int i=0; i<12; i++){
			if(i==2 || i==5 || i==8 || i==11 ) continue;
			if(rot > i*2){
				if(rot <= i*2+32){
					for(int j=0;j<6;j++) box[i*6+j].addrot(matx);
				}else if(rot <= i*2+33){
					for(int j=0;j<6;j++) box[i*6+j].rotOriginal();
				}
			}
		}
		if(rot<70.0){
			rot++;
		}else{
			if(on){ rot=0; box_action = 2; }
		}
		break;
	case 2:		//手前から奥へ回転　1マスおき
		a=0;
		for(int i=0; i<12; i++){
			a ^= 1;
			if(rot > i*2){
				if(rot <= i*2+32){
					for(int j=0;j<6;j+=2) box[i*6+j+a].addrot(matx);
				}else if(rot <= i*2+33){
					for(int j=0;j<6;j+=2) box[i*6+j+a].rotOriginal();
				}
			}
		}
		if(rot<70.0){
			rot++;
		}else{
			if(on){ rot=0; box_action = 3; }
		}
		break;
	case 3:		//奥から手前へ回転 行ごと
		a=-1;
		for(int i=11; i>=0; i--){
			a++;
			if(a==2 || a==5 || a==8) continue;
			if(rot > a*2){
				if(rot <= a*2+32){
					for(int j=0;j<6;j++) box[i*6+j].addrot(matx);
				}else if(rot <= a*2+33){
					for(int j=0;j<6;j++) box[i*6+j].rotOriginal();
				}
			}
		}
		if(rot<70.0){
			rot++;
		}else{
			if(on){ rot=0; box_action = 0; }
		}
		break;
	}
	
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	
	for(int i=0; i<12*6; i++){
		box[i].Render();
	}
	
	glPopMatrix();
	
	//判定球の外の場合
	if( !on ) return;

	//当たり判定
	for(int i=0; i<12*6; i++){
		box[i].NormalForceIce(v, _speed);
	}
}



/*-----------------------------------------------------------------------------------*
	オブジェクトグループ5
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
OBJgroup5::OBJgroup5()
{
	//初期位置セット
	Vector3 v = { 3.0, 3.0, 3.0 };
	
	box[0].Set(&v,"box6.png");
	box[0].pos.y = 0.2;
	
	v.x = 15.0; v.z = 15.0;
	box[1].Set(&v,"box.png");
	
	//バウンディングスフィア計算
	Vector3 Max=box[0].pos, min=box[0].pos;
	for(int i=0; i<2; i++){
		float r = box[i].radius();
		//最大値比較
		if( Max.x < box[i].pos.x + r ) Max.x = box[i].pos.x + r;
		if( Max.y < box[i].pos.y + r ) Max.y = box[i].pos.y + r;
		if( Max.z < box[i].pos.z + r ) Max.z = box[i].pos.z + r;
		//最小値比較
		if( min.x > box[i].pos.x - r ) min.x = box[i].pos.x - r;
		if( min.y > box[i].pos.y - r ) min.y = box[i].pos.y - r;
		if( min.z > box[i].pos.z - r ) min.z = box[i].pos.z - r;
	}
	
	//中心と半径を求める
	Max -= min; Max = Max * 0.5;	//Maxとminの中間点が中央
	radius = sqrt( Max.x*Max.x + Max.x*Max.x + Max.y*Max.y + Max.z*Max.z );	//半径
	Vector3 offset = min + Max;	//中心へのベクトル
	//オフセット
	for(int i=0; i<2; i++){
		box[i].pos -= offset;
	}
	
	//位置セット
	pos = 0.0; pos += offset;
	pos.z += 305.0;
	pos.y -= 3.0;
	
	//box[0]の上に乗っているか
	onCLEAR = false;
}

void OBJgroup5::Disp_Colli(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z };	
	
	glPushMatrix();
	
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	
	for(int i=0; i<2; i++){
		box[i].Render();
	}
	
	glPopMatrix();
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > radius+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		return;
	}

	//当たり判定
	for(int i=0; i<2; i++){
		box[i].NormalForce(v, _speed);
	}
	
	//box[0]の上に乗ったかどうか
	if( box[0].laston & 1 ){
		onCLEAR = true;
	}
}





////////////////////////////////////

/*-----------------------------------------------------------------------------------*
	三角形オブジェクト
	当たり判定を含む
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
sankakOBJ::sankakOBJ(){
	for(int i=0; i<3; i++){
		oVertex[i].x=0.0; oVertex[i].y=0.0; oVertex[i].z=0.0;
		vertex[i].x=0.0; vertex[i].y=0.0; vertex[i].z=0.0;
	}
	pos = 0.0;
	DisplayList = 0;
	max_y = 0.0;
	laston = 0;
}
//三角形登録
void sankakOBJ::Set(Vector3 *vec)
{
	//コピー
	for(int i=0; i<3; i++){
		oVertex[i] = vec[i];
		vertex[i] = vec[i];
	}
	//当たり判定登録
	colliOBJ.Set_3(vertex);
	
	//テクスチャ読み込み
	pngInfo info;
	texturePNG = pngBind("test.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	//UV
	float uv[3][2] = { {0.0,0.0}, {1.0,0.0}, {0.0,1.0} };
	
	setDefaultMaterial();
	//ディスプレイリストを作成
	DisplayList = glGenLists(1);
	glNewList(DisplayList,GL_COMPILE); //コンパイルのみ
	
	glBindTexture(GL_TEXTURE_2D, texturePNG);
	glNormal3fv( (GLfloat*)&(colliOBJ.GetNormal().x) );
	glBegin(GL_TRIANGLES);
	for(int i=0; i<3; i++){
		glTexCoord2fv( uv[i] );
		glVertex3fv( vertex[i] );
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	
	glEndList();	//ディスプレイリストおわり
}

//回転させる
void sankakOBJ::rot(MATRIX4x4 &m)
{
	
}

//追加回転させる
void sankakOBJ::addrot(MATRIX4x4 &m)
{
	float v[4];
	for(int i=0; i<3; i++){
		v[0]=vertex[i].x; v[1]=vertex[i].y; v[2]=vertex[i].z; v[3]=1.0;
		Matrixkakeru(v, v, m.m);
		vertex[i].x=v[0]; vertex[i].y=v[1]; vertex[i].z=v[2];
	}
	//当たり判定登録
	colliOBJ.Set_3(vertex);
	//回転した
	laston |= 2;
	//行列保存
	mat = m;
}

//垂直抗力の取得と適用
void sankakOBJ::NormalForce(Vector3 &_pos, Vector3 &_speed)
{
	//float y = 0.0;
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z }, nvec;	
	
	if( laston==3 ){	//前回乗ってて回転したとき
		float vm[4];
		vm[0]=v.x; vm[1]=v.y; vm[2]=v.z; vm[3]=1.0;
		Matrixkakeru(vm, vm, mat.m);
		_pos.x += vm[0]-v.x; _pos.y += vm[1]-v.y; _pos.z += vm[2]-v.z;
		v.x=vm[0]; v.y=vm[1]; v.z=vm[2];
		laston=0;
	}
	
	/*if( laston && v.y<max_y ){
		Vector3 v2 = v, speed2 = _speed;
		//上へ上げる
		y = max_y-v.y +0.001;
		v2.y +=  y;
		speed2.y -= y;
		nvec = colliOBJ.GetNormalForce( v2, speed2);	//垂直抗力計算
		if( nvec.x==0.0 && nvec.y==0.0 && nvec.z==0.0 ){
			laston = false;
		}else{
			Vector3 n = colliOBJ.GetNormal();
			v2 += speed2 + nvec + n*0.01;
			_speed = v2-v;
			//laston = true;
		}
	}else{*/
		nvec = colliOBJ.GetNormalForce( v, _speed);	//垂直抗力計算
		if( nvec.x==0.0 && nvec.y==0.0 && nvec.z==0.0 ){
			laston = 0;
		}else{
			_speed += nvec;
			laston |= 1;
		}
	//}
}

//描画
void sankakOBJ::Render()
{
	//UV
	float uv[3][2] = { {0.0,0.0}, {1.0,0.0}, {0.0,1.0} };
	
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	setDefaultMaterial();
	
	glBindTexture(GL_TEXTURE_2D, texturePNG);
	glNormal3fv( (GLfloat*)&(colliOBJ.GetNormal().x) );
	glBegin(GL_TRIANGLES);
	for(int i=0; i<3; i++){
		glTexCoord2fv( uv[i] );
		glVertex3fv( vertex[i] );
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//glCallList(DisplayList);	//描画
	glPopMatrix();
}

//デストラクタ
sankakOBJ::~sankakOBJ(){
	glDeleteTextures(1, &texturePNG);
}




/*-----------------------------------------------------------------------------------*
	直方体オブジェクト
	当たり判定を含む
 *-----------------------------------------------------------------------------------*/
const int boxOBJ::index[6][4] = { {0, 1, 2, 3}, 
						{4, 5, 1, 0},
						{5, 6, 2, 1},
						{6, 7, 3, 2},
						{7, 4, 0, 3},
						{7, 6, 5, 4} };
//コンストラクタ
boxOBJ::boxOBJ() {
	for(int i=0; i<8; i++){
		oVertex[i].x=0.0; oVertex[i].y=0.0; oVertex[i].z=0.0;
		vertex[i].x=0.0; vertex[i].y=0.0; vertex[i].z=0.0;
	}
	DisplayList = 0;
	texturePNG = 0;
	laston = 0;
	laston_i = 0;
	r = 0.0;
	MatrixIdentity(mat.m);
	//行列初期化
	for(int i=0; i<16; i++){
		if(i%5 == 0){
			Matrix[i] = 1.0;
		}else{
			Matrix[i] = 0.0;
		}
	}
	move = 0.0;
	isSet = false;
	pos = 0.0;
}
//登録
void boxOBJ::Set(Vector3 *vec, const char *texName)
{
	//すでに呼ばれたかどうか
	if(isSet) return;
	isSet = true;
	
	//コピー&頂点位置決定
	oVertex[0].x = fabs(vec->x);
	oVertex[0].y = fabs(vec->y);
	oVertex[0].z = fabs(vec->z);
	oVertex[1].x = -oVertex[0].x; oVertex[1].y =  oVertex[0].y; oVertex[1].z =  oVertex[0].z; 
	oVertex[2].x = -oVertex[0].x; oVertex[2].y = -oVertex[0].y; oVertex[2].z =  oVertex[0].z; 
	oVertex[3].x =  oVertex[0].x; oVertex[3].y = -oVertex[0].y; oVertex[3].z =  oVertex[0].z; 
	oVertex[4].x =  oVertex[0].x; oVertex[4].y =  oVertex[0].y; oVertex[4].z = -oVertex[0].z; 
	oVertex[5].x = -oVertex[0].x; oVertex[5].y =  oVertex[0].y; oVertex[5].z = -oVertex[0].z; 
	oVertex[6].x = -oVertex[0].x; oVertex[6].y = -oVertex[0].y; oVertex[6].z = -oVertex[0].z; 
	oVertex[7].x =  oVertex[0].x; oVertex[7].y = -oVertex[0].y; oVertex[7].z = -oVertex[0].z; 
	for(int i=0; i<8; i++){
		vertex[i] = oVertex[i];
	}
	
	//当たり判定登録
	Vector3 v[4];
	for(int i=0; i<6; i++){
		v[0]=vertex[ index[i][0] ];
		v[1]=vertex[ index[i][1] ];
		v[2]=vertex[ index[i][2] ];
		v[3]=vertex[ index[i][3] ];
		colli[i].Set_4(v);
	}
	
	//判定球の半径測定
	r = sqrt( vec->x*vec->x + vec->y*vec->y + vec->z*vec->z );
	
	//テクスチャ読み込み
	if( texName != NULL ){
		pngInfo info;
		texturePNG = pngBind(texName, PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}
	
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	
	//ディスプレイリスト
	DisplayList = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList,GL_COMPILE); //コンパイルのみ
	
	if(texturePNG) glBindTexture(GL_TEXTURE_2D, texturePNG);
	
	setDefaultMaterial();
	
	glBegin(GL_QUADS);
	for(int f=0; f<6; f++){
		glNormal3fv( (GLfloat*)&(colli[f].GetNormal()) );
		for(int i=0; i<4; i++){
			if(texturePNG) glTexCoord2fv( uv[i] );
			glVertex3fv( oVertex[ index[f][i] ] );
		}
	}
	glEnd();
	
	if(texturePNG) glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	
	glEndList();	//ディスプレイリストおわり
}

//回転させる
void boxOBJ::rotOriginal()
{
	float v[4];
	for(int i=0; i<8; i++){
		vertex[i]=oVertex[i];
	}
	//当たり判定登録
	Vector3 vec[4];
	for(int i=0; i<6; i++){
		vec[0]=vertex[ index[i][0] ];
		vec[1]=vertex[ index[i][1] ];
		vec[2]=vertex[ index[i][2] ];
		vec[3]=vertex[ index[i][3] ];
		colli[i].Set_4(vec);
	}
}

//追加回転させる
void boxOBJ::addrot(MATRIX4x4 &m)
{
	float v[4];
	for(int i=0; i<8; i++){
		v[0]=vertex[i].x; v[1]=vertex[i].y; v[2]=vertex[i].z; v[3]=1.0;
		Matrixkakeru(v, v, m.m);
		vertex[i].x=v[0]; vertex[i].y=v[1]; vertex[i].z=v[2];
	}
	//当たり判定登録
	Vector3 vec[4];
	for(int i=0; i<6; i++){
		vec[0]=vertex[ index[i][0] ];
		vec[1]=vertex[ index[i][1] ];
		vec[2]=vertex[ index[i][2] ];
		vec[3]=vertex[ index[i][3] ];
		colli[i].Set_4(vec);
	}
	//回転した
	laston |= 2;
	//行列保存
	mat = m;
	Matrixkakeru(Matrix, Matrix, m.m);
}

//追加移動させる
void boxOBJ::addpos(Vector3 &vec){
	move = vec;
	pos += vec;
}

//垂直抗力の取得と適用
void boxOBJ::NormalForce(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z }, nvec;	
	
	//移動したとき
	if( move.x!=0.0 || move.y!=0.0 || move.z!=0.0 ){
		//if(laston & 1){
			//_pos += move;
			v += move;
			//_speed += move;
		//}
	}
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > r+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		laston =0;
		move = 0.0;
		return;
	}
	
	
	
	if( laston&2 ){	//前回乗ってて回転したとき
		float vm[4];
		vm[0]=v.x; vm[1]=v.y; vm[2]=v.z; vm[3]=1.0;
		Matrixkakeru(vm, vm, mat.m);
		_pos.x += vm[0]-v.x; _pos.y += vm[1]-v.y; _pos.z += vm[2]-v.z;	//回転に合わせて移動
		//_speed.x -= vm[0]-v.x; _speed.y -= vm[1]-v.y; _speed.z -= vm[2]-v.z;	//回転に合わせて移動
		v.x=vm[0]; v.y=vm[1]; v.z=vm[2];
	}
	
	laston_i--; laston=0;
	for(int i=0; i<6; i++){
		nvec = colli[i].GetNormalForce( v, _speed);	//垂直抗力計算
		if( nvec.x==0.0 && nvec.y==0.0 && nvec.z==0.0 ){
		}else{
			laston_i=2;
			_speed += nvec;
		}
	}
	if(laston_i>0) laston |= 1;
	
	//移動したとき
	if( move.x!=0.0 || move.y!=0.0 || move.z!=0.0 ){
		if(laston & 1){
			//_pos += move;
			//v += move;
			_speed += move;
		}
	}
	move = 0.0;
}

//描画
void boxOBJ::Render()
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	glMultMatrixd(Matrix);	//回転行列乗算
	
	glCallList(DisplayList);
	
	glPopMatrix();
}

//デストラクタ
boxOBJ::~boxOBJ(){
	glDeleteTextures(1, &texturePNG);
}


//氷のとこ用
//垂直抗力の取得と適用
void boxOBJ::NormalForceIce(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z }, nvec;	
	
	//移動したとき
	if( move.x!=0.0 || move.y!=0.0 || move.z!=0.0 ){
		//if(laston & 1){
			//_pos += move;
			v += move;
			//_speed += move;
		//}
	}
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > r+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		laston =0;
		move = 0.0;
		return;
	}
	
	
	//ここ変更 fabs():絶対値を返す
	if( laston&2 &&
		fabs(v.x) < oVertex[0].x && fabs(v.z) < oVertex[0].z ){	//前回乗ってて回転したとき
		float vm[4];
		vm[0]=v.x; vm[1]=v.y; vm[2]=v.z; vm[3]=1.0;
		Matrixkakeru(vm, vm, mat.m);
		_pos.x += vm[0]-v.x; _pos.y += vm[1]-v.y; _pos.z += vm[2]-v.z;	//回転に合わせて移動
		//_speed.x -= vm[0]-v.x; _speed.y -= vm[1]-v.y; _speed.z -= vm[2]-v.z;	//回転に合わせて移動
		v.x=vm[0]; v.y=vm[1]; v.z=vm[2];
	}
	
	laston_i--; laston=0;
	for(int i=0; i<6; i++){
		nvec = colli[i].GetNormalForce( v, _speed);	//垂直抗力計算
		if( nvec.x==0.0 && nvec.y==0.0 && nvec.z==0.0 ){
		}else{
			laston_i=2;
			_speed += nvec;
		}
	}
	if(laston_i>0) laston |= 1;
	
	//移動したとき
	if( move.x!=0.0 || move.y!=0.0 || move.z!=0.0 ){
		if(laston & 1){
			//_pos += move;
			//v += move;
			_speed += move;
		}
	}
	move = 0.0;
}





