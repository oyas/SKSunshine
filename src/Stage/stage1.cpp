//stage1.cpp


#include "stage1.h"
#include "stage0.h"	//次のステージ


#define DOSEI_IDORYOU 0.2
#define DOSEI_CAMRA 0.05

/*-----------------------------------------------------------------------------------*
	ステージ1 クラス
 *-----------------------------------------------------------------------------------*/

//コンストラクタ（初期化）
Stage1::Stage1() :ita(24.0, 24.0), ita2(24.0, 24.0)//, sound("whistle.wav")
	//板のコンストラクタの引数を渡してやる。メンバーイニシャライザ
	//(このコンストラクタが実行される前に、板のコンストラクタは実行される)
{
	//Xファイル読み込み
	char a[]="Models/dosei.x";
	model.Load(a, 0);
printf("Stage1 Check1\n");
	//Xオブジェクト作成
	dosei.setXModel(&model);
	dosei.pos.y = 10.0;
	dosei.pos.z = 0.0;
	dosei.offset.y=1.0;
	//どせい向き
	dosei.angle = 180;
	dosei.ang.y = 1.0;
	//Ita初期化
	ita2.pos.z = -50.0;
	//ライトの位置セット
	light0pos[0] = 1.0;
	light0pos[1] = 10.0;
	light0pos[2] = 5.0;
	light0pos[3] = 1.0;
	//カメラ角度
	cam_z=20.0; cam_r=M_PI; cam_rx=M_PI_4/2;
	//どせいに関するもの初期化
	dosei_action=0;
	dash_vec.x=0.0; dash_vec.y=0.0; dash_vec.z=1.0;
	dash_svec.x=1.0; dash_svec.y=0.0; dash_svec.z=0.0;
	//キー
	key_on=0; yaziru=0;
	//スコア初期化
	total=0;
	//スコア用文字列
	str[0]='\0'; str_t[0]='\0'; str_a[0]='\0';
	std::cout << "Vendor :"<<glGetString(GL_VENDOR)<<'\n';
	std::cout << "GPU : "<<glGetString(GL_RENDERER) <<'\n';
	std::cout << "OpenGL ver. " <<glGetString(GL_VERSION)<<'\n';
	//当たり判定用、初期化
	colliIta.Set_xz4(ita.vertex);
	//三角形OBJ登録
	Vector3 v[3] = { {4.8, 2.2, 2.2},
						{-0.2, 7.0, -6.0},
						{-1.6, 1.8, 2.0}  };
	//sanka.Set(v);
	//sanka.pos.z = -4.0;
	box.Set(v, "box2.png");	//直方体
	box.pos.z = 42.0;
	//当たり判定があったか
	onface = 0;
	spacekey = 0;
	backjamp =0;
	
printf("stage1 Check2\n");
	//////////////////////////
	//背景作成
	//背景画像読み込み
	pngInfo info;
	textureBACK = pngBind("yama.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	DisplayList_BACK = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList_BACK,GL_COMPILE); //コンパイルのみ
	
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);//いろいろ無効にする
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glColor3f(1.0, 1.0, 1.0);
	//カメラ
	gluLookAt(0, 0, -2.4, 0,0,0, 0.0,1.0,0.0);
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	//pos
	float pos[4][3] = { {1.8,1.0,0.0}, {1.8,-1.0,0.0}, {-1.8,-1.0,0.0}, {-1.8,1.0,0.0} };
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureBACK);//バインド
	glBegin(GL_QUADS);
		for(int i=0; i<4; i++){
			glTexCoord2fv( uv[i] );
			glVertex3fv( pos[ i ] );
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEndList();	//ディスプレイリストおわり
	//背景ここまで
	//////////////////////////////
	
	//////////////////////////
	// MISS作成
	//画像読み込み
	//アルファテスト用
	glAlphaFunc(GL_GEQUAL, 0.5);
	{
	pngInfo info;
	textureMISS = pngBind("miss.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	DisplayList_MISS = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList_MISS,GL_COMPILE); //コンパイルのみ
	
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);//いろいろ無効にする
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_ALPHA_TEST);// アルファテスト有効
	
	glColor3f(1.0, 1.0, 1.0);
	//カメラ
	gluLookAt(0, 0, -3.0, 0,0,0, 0.0,1.0,0.0);
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	//pos
	float pos[4][3] = { {1.0,1.0,0.0}, {1.0,-1.0,0.0}, {-1.0,-1.0,0.0}, {-1.0,1.0,0.0} };
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureMISS);//バインド
	glBegin(GL_QUADS);
		for(int i=0; i<4; i++){
			glTexCoord2fv( uv[i] );
			glVertex3fv( pos[ i ] );
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);// アルファテスト無効
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEndList();	//ディスプレイリストおわり
	}
	//ここまで
	//////////////////////////////
	
	//////////////////////////
	// CLEAR作成
	//画像読み込み
	{
	pngInfo info;
	textureCLEAR = pngBind("clear.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	DisplayList_CLEAR = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList_CLEAR,GL_COMPILE); //コンパイルのみ
	
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);//いろいろ無効にする
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_BLEND);// ブレンド有効
	
	glColor3f(1.0, 1.0, 1.0);
	//カメラ
	gluLookAt(0, 0, -3.0, 0,0,0, 0.0,1.0,0.0);
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	//pos
	float pos[4][3] = { {1.0,1.0,0.0}, {1.0,-1.0,0.0}, {-1.0,-1.0,0.0}, {-1.0,1.0,0.0} };
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureCLEAR);//バインド
	glBegin(GL_QUADS);
		for(int i=0; i<4; i++){
			glTexCoord2fv( uv[i] );
			glVertex3fv( pos[ i ] );
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);// ブレンド無効
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEndList();	//ディスプレイリストおわり
	}
	//ここまで
	//////////////////////////////
/*
	//////////////////////////
	// 影作成
	//画像読み込み
	{
	pngInfo info;
	textureShadow = pngBind("clear.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	DisplayList_Shadow = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList_Shadow,GL_COMPILE); //コンパイルのみ
	
	glLoadIdentity();
	glEnable(GL_BLEND);// ブレンド有効
	
	glColor3f(1.0, 1.0, 1.0);
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	//pos
	float pos[4][3] = { {1.0,0.0,1.0}, {1.0,0.0,-1.0}, {-1.0,0.0,-1.0}, {-1.0,0.0,1.0} };
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureShadow);//バインド
	glBegin(GL_QUADS);
		for(int i=0; i<4; i++){
			glTexCoord2fv( uv[i] );
			glVertex3fv( pos[ i ] );
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);// ブレンド無効
	
	glEndList();	//ディスプレイリストおわり
	}
	//ここまで
	//////////////////////////////
*/	
	//ゲーム関連
	game = 5;
	game_timer = 0;
	sprintf(str_a,"x%d",game);
	
	printf("stage1 初期化完了。\n");
}


//画面再描画時によばれる(1フレーム毎に呼ばれる)
void Stage1::Disp(){
	float _x,_y,_z;
	
	glCallList(DisplayList_BACK);	//背景描画
	
	glLoadIdentity();
	//カメラの位置
	_x = sin(cam_r)*cos(cam_rx)*cam_z;
	_y = sin(cam_rx)*cam_z;
	_z = cos(cam_r)*cos(cam_rx)*cam_z;
	//ライトの位置セット
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	
	//カメラ
	gluLookAt(dosei.pos.x+_x, dosei.pos.y+_y, dosei.pos.z+_z, 
		dosei.pos.x,dosei.pos.y,dosei.pos.z, 0.0,1.0,0.0);
	
	//キー処理
	if(key_on & 16){	//bキー
		dosei.angle += 1.0;
		//どせいさんの向いてる方向計算
		dash_vec.x = -sin(dosei.angle/180 * M_PI);
		dash_vec.z = -cos(dosei.angle/180 * M_PI);
		dash_svec.x = dash_vec.z;
		dash_svec.z = -dash_vec.x;
	}else if(key_on & 32){	//nキー
		dosei.angle -= 1.0;
		dash_vec.x = -sin(dosei.angle/180 * M_PI);
		dash_vec.z = -cos(dosei.angle/180 * M_PI);
	}else if(key_on & 64){	//space
		if(onface&&spacekey<1){
			switch(dosei_action){
			case 0:
				if( backjamp&7 ){
					dosei.speed.y+=1.8;
				}else{
					dosei.speed.y+=0.4;
				}
					//dosei.speed += dash_vec * DOSEI_IDORYOU * 0.5;
					dosei_action = 1;
				break;
			case 1:
				if(onface<5){
					dosei.speed.y+=0.5;
					dosei_action = 2;
				}else{
					dosei.speed.y+=0.4;
				}
				//dosei.speed += dash_vec * DOSEI_IDORYOU * 0.5;
				break;
			case 2:
				if(onface<30){
					dosei.speed.y+=2.0;
					dosei.speed += dash_vec * DOSEI_IDORYOU * 50.0;
					dosei_action = 0;
				}else{
					dosei.speed.y+=0.4;
					//dosei.speed += dash_vec * DOSEI_IDORYOU * 0.5;
					dosei_action = 1;
				}
				break;
			default:
				dosei_action = 0;
				break;
			}
		}
		spacekey++;
	}else{
		spacekey=0;
	}
	
	//どせい移動
	dosei.force = 0.0;
	if(yaziru & 1 || key_on&256){	//left
		dosei.force += dash_svec * DOSEI_IDORYOU;
	}
	if(yaziru & 2 || key_on&512){	//right
		dosei.force -= dash_svec * DOSEI_IDORYOU;
	}
	if(yaziru & 4 || key_on&1024){	//up
		dosei.force += dash_vec * DOSEI_IDORYOU;
		backjamp |= 16;
		if(backjamp&32){
			backjamp += 5;
		}
	}
	if(yaziru & 8 || key_on&2048){	//down
		dosei.force -= dash_vec * DOSEI_IDORYOU;
		backjamp |= 32;
		if(backjamp&16){
			backjamp += 5;
		}
	}else{
		backjamp &= ~32;
	}
	if(!(yaziru & 4) ){
		backjamp &= ~16;
	}
	if( backjamp&7 ) backjamp--;	//1〜3bitのどれかが立っていたらマイナス
	
	//救出
	if(dosei.pos.y<-50){
		if(game>0 && game<6){
			game--;
			sprintf(str_a,"x%d",game);
		}
		if(game>0){
			if(dosei.pos.z>210.0){
				dosei.pos.x = 0.0;
				dosei.pos.y = 10.0;
				dosei.pos.z = 210.0;
				dosei.speed = 0.0;
			}else{
				dosei.pos.x = 0.0;
				dosei.pos.y = 10.0;
				dosei.pos.z = 0.0;
				dosei.speed = 0.0;
			}
		}
	}
		
	//三角の回転
	MATRIX4x4 m;
	float rot=0.01;
	MatrixIdentity(m.m);
	m.m[0][0] = cos(rot);
	m.m[1][0] = sin(rot);
	m.m[0][1] = -m.m[1][0];
	m.m[1][1] = m.m[0][0];
	//sanka.addrot(m);
	box.addrot(m);
	
		//当たり判定・影判定
		/*if(dosei.speed.y>-0.3)*/ dosei.speed.y -= 0.02;	//重力
		Vector3 speed = dosei.speed;
		if( (dosei.force.x<0 && speed.x>dosei.force.x)||(dosei.force.x>0 && speed.x<dosei.force.x) ) speed.x = dosei.force.x;
		if( (dosei.force.y<0 && speed.y>dosei.force.y)||(dosei.force.y>0 && speed.y<dosei.force.y) ) speed.y = dosei.force.y;
		if( (dosei.force.z<0 && speed.z>dosei.force.z)||(dosei.force.z>0 && speed.z<dosei.force.z) ) speed.z = dosei.force.z;
		Vector3 bspeed = speed;	//出力用
		//影用
		float shadow_y = dosei.pos.y+0.01;
		//床１
		Vector3 nvec;
		nvec = colliIta.GetNormalForce(dosei.pos, speed);	//垂直抗力計算
		speed.y += nvec.y;
		//床2の計算
		nvec = colliIta.GetNormalForce(dosei.pos-ita2.pos, speed);
		speed.y += nvec.y;
		//三角形OBJ
		//sanka.NormalForce(dosei.pos, speed);
		box.NormalForce(dosei.pos, speed);
	
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
		
		//摩擦
		//dosei.speed.x -= dosei.speed.x;
		//dosei.speed.z -= dosei.speed.z;
		if( speed.y-bspeed.y != 0.0){
			/*float a = sqrtf( speed.x*speed.x + speed.y*speed.y + speed.z*speed.z );
			if(a>0.02){
				speed -= speed * (1/a) * 0.02;
				//a = sqrtf( dosei.speed.x*dosei.speed.x + dosei.speed.y*dosei.speed.y + dosei.speed.z*dosei.speed.z );
				//dosei.speed -= dosei.speed * (1/a) * 0.1;
			}else if(a<=0.02){
				speed = 0.0;
				//dosei.speed = 0.0;
			}*/
			dosei.speed = 0.0;
			if( onface++ > 3) dosei_action=0;	//3段ジャンプ用
		}else onface=0;
		dosei.pos += speed;	//適用
		//dosei.speedにも適用
		/*Vector3 s = speed - bspeed;
		if( (dosei.speed.x<0 && (dosei.speed.x+=s.x)>0)||(dosei.speed.x>0 && (dosei.speed.x+=s.x)<0) ) dosei.speed.x = 0.0;
		if( (dosei.speed.y<0 && (dosei.speed.y+=s.y)>0)||(dosei.speed.y>0 && (dosei.speed.y+=s.y)<0) ) dosei.speed.y = 0.0;
		if( (dosei.speed.z<0 && (dosei.speed.z+=s.z)>0)||(dosei.speed.z>0 && (dosei.speed.z+=s.z)<0) ) dosei.speed.z = 0.0;*/
	//出力用文字列
	sprintf(str_t, "speed.y: %f %f", dosei.speed.y, dosei.speed.y-bspeed.y);
	
	
	//床描画
	ita.Render();
	ita2.Render();
	//テクスチャマッピング有効
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	//どせいさん描画
	if( !(key_on & 128) ) dosei.Render();
	//三角形描画
	//sanka.Render();
	//直方体描画
	box.Render();
	//テクスチャマッピング無効
	//glDisable(GL_BLEND);  //ブレンド
	glDisable(GL_TEXTURE_2D);
	
	//FPS出力
	fpstxt = fps();
	if(strcmp(fpstxt, fpstxtb)){
		strcpy(fpstxtb, fpstxt);
		printf("%s\n",fpstxt);
	}
	
	//カメラ移動
	if(key_on & 1){
		cam_r += DOSEI_CAMRA;
		if(cam_r>2*M_PI) cam_r -= 2*M_PI;
		//どせいも
		dosei.angle += DOSEI_CAMRA*180/M_PI;
		dash_vec.x = -sin(dosei.angle/180 * M_PI);
		dash_vec.z = -cos(dosei.angle/180 * M_PI);
		dash_svec.x = dash_vec.z;
		dash_svec.z = -dash_vec.x;
	}
	if(key_on & 2){
		cam_r -= DOSEI_CAMRA;
		if(cam_r<-2*M_PI) cam_r += 2*M_PI;
		//どせいも
		dosei.angle -= DOSEI_CAMRA*180/M_PI;
		dash_vec.x = -sin(dosei.angle/180 * M_PI);
		dash_vec.z = -cos(dosei.angle/180 * M_PI);
		dash_svec.x = dash_vec.z;
		dash_svec.z = -dash_vec.x;
	}
	if(key_on & 4096 && cam_z>5.0){
		cam_z -= DOSEI_CAMRA;
	}
	if(key_on & 8192 && cam_z<30.0){
		cam_z += DOSEI_CAMRA;
	}
	if(key_on & 4 && cam_rx<M_PI_2-DOSEI_CAMRA-0.01){
		cam_rx -= DOSEI_CAMRA;
	}
	if(key_on & 8 && cam_rx>-(M_PI_2-DOSEI_CAMRA-0.01)){
		cam_rx += DOSEI_CAMRA;
	}
	
	//サウンド
	//sound.stream();
	
	
	//クリア判定
	if( objg5.onCLEAR ){
		game = 6;
	}
	//MISS or CLEAR
	if(game==6){	//CLEAR
		glCallList(DisplayList_CLEAR);	//描画
		if(game_timer++ > 120){
			ChangeStage(new Stage0);
		}
	}else if(game<=0){	//MISS
		glCallList(DisplayList_MISS);	//描画
		if(game_timer++ > 60){
			ChangeStage(new Stage0);
		}
	}
}

//2Dの描画
void Stage1::Disp2D(){
	glColor4f(0.0,0.0,0.0, 1.0);	//カラー
	DRAW_STRING(10, 10, str_a, GLUT_BITMAP_TIMES_ROMAN_24);
	/*DRAW_STRING(10, 75, str_t, GLUT_BITMAP_TIMES_ROMAN_24);
	DRAW_STRING(10, 50, str, GLUT_BITMAP_TIMES_ROMAN_24);*/
}

//入力処理
void Stage1::Input(char event, int key, int x, int y){
	//チェックするボタンの登録
	static char size = 14;
	static int Keys[] = {
		'a',	//1
		's',	//2
		'w',	//4
		'z',	//8
		'b',	//16
		'n',	//32
		' ',	//64
		'x',	//128
		'4',	//256 left
		'6',	//512 right
		'8',	//1024 up
		'5',	//2048 down
		'q',	//4096
		'e'		//8192
		};
	static char size_sk = 4;
	static int Keys_sk[] = {
		GLUT_KEY_LEFT,	//1
		GLUT_KEY_RIGHT,	//2
		GLUT_KEY_UP,	//4
		GLUT_KEY_DOWN	//8
		};
	
	//ボタン状態チェック
	if( event==SC_INPUT_KEY_DOWN ){
		SetKeyState(&key_on, key, true, Keys, size);
	}else if( event==SC_INPUT_KEY_UP ){
		SetKeyState(&key_on, key, false, Keys, size);
	}else if( event==SC_INPUT_SPECIALKEY_DOWN ){
		SetKeyState(&yaziru, key, true, Keys_sk, size_sk);
	}else if( event==SC_INPUT_SPECIALKEY_UP ){
		SetKeyState(&yaziru, key, false, Keys_sk, size_sk);
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
OBJgroup1::OBJgroup1()
{
	printf("!!\n");
	//初期位置セット
	Vector3 v = { 3.0, 3.0, 3.0 };
	
	box[0].Set(&v,"box1.png");
	box[0].pos.z = -6.0;
	
	box[1].Set(&v,"box1.png");
	box[1].pos.z = 5.0;
	
	v.z = 4.0;
	box[2].Set(&v,"box1.png");
	box[2].pos.z = 11.9;
	box[2].pos.y = 3.0;
	
	//バウンディングスフィア計算
	Vector3 Max=box[0].pos, min=box[0].pos;
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
	
	//中心と半径を求める
	Max -= min; Max = Max * 0.5;	//Maxとminの中間点が中央
	radius = sqrt( Max.x*Max.x + Max.x*Max.x + Max.y*Max.y + Max.z*Max.z );	//半径
	Vector3 offset = min + Max;	//中心へのベクトル
	for(int i=0; i<3; i++){
		box[i].pos -= offset;
	}
	
	//位置セット
	pos = 0.0; pos += offset;
	pos.z += 20.99;
	pos.y -= 3.0001;
}

void OBJgroup1::Disp_Colli(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z };	
	
	glPushMatrix();
	
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	
	for(int i=0; i<3; i++){
		box[i].Render();
	}
	
	glPopMatrix();
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > radius+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		return;
	}

	//当たり判定
	for(int i=0; i<3; i++){
		box[i].NormalForce(v, _speed);
	}
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
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > radius+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
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
	
	
	//ここ変更
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





