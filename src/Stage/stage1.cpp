//stage1.cpp


#include "../assist/object.h"	//基本オブジェクト
#include "stage1.h"
#include "stage0.h"	//次のステージ
#include "stage2.h"	//次のステージ


///////////////////////
// 設定
#define DOSEI_CAMRA 0.05	//カメラ操作時の移動量

#define GAME_ZIKI 5		//初期自機数
#define GAME_GRAVITY 0.02	//重力

//////////////////////
// 定数定義
#define GAME_CLEAR 100	//クリア

//////////////////////
// 共有クラスへのショートカット
#define S_keystate PublicData->Key.state
#define S_dosei PublicData->Player.dosei
#define S_shadow PublicData->Player.shadow


/*-----------------------------------------------------------------------------------*
	ステージ1 クラス
 *-----------------------------------------------------------------------------------*/

//コンストラクタ（初期化）
Stage1::Stage1( PublicClass *pd ) : StageClass(pd), sound("whistle.wav")
	//板のコンストラクタの引数を渡してやる。メンバーイニシャライザ
	//(このコンストラクタが実行される前に、板のコンストラクタは実行される)
{
	//ライトの位置セット
	light0pos[0] = 1.0;
	light0pos[1] = 10.0;
	light0pos[2] = 5.0;
	light0pos[3] = 1.0;
	//カメラ角度
	cam_z=20.0; cam_r=M_PI; cam_rx=M_PI_4/2;

	//プレイヤーの初期設定
	PublicData->Player.ResetPos();
	PublicData->Player.ResetAng();
		
	//スコア初期化
	//total=0;
	//スコア用文字列
//	str[0]='\0'; str_t[0]='\0'; 
	str_a[0] = '\0';
	//情報表示
	std::cout << "Vendor :"<<glGetString(GL_VENDOR)<<'\n';
	std::cout << "GPU : "<<glGetString(GL_RENDERER) <<'\n';
	std::cout << "OpenGL ver. " <<glGetString(GL_VERSION)<<'\n';


	//背景画像作成
	float pos[4][3] = { {2.3,1.3,0.0}, {2.3,-1.3,0.0}, {-2.3,-1.3,0.0}, {-2.3,1.3,0.0} };
	textureBACK = png_back.load("yama.png");
	DisplayList_BACK = png_back.CreateDisplayList(pos, false);
	
	// MISS画像作成
	textureMISS = png_miss.load("miss.png");
	DisplayList_MISS = png_miss.CreateDisplayList(NULL, true);
	
	// CLEAR画像作成
	textureCLEAR = png_clear.load("clear.png");
	DisplayList_CLEAR = png_clear.CreateDisplayList(NULL, true);
	
	//ゲーム関連
	game = GAME_ZIKI;
	game_timer = 0;
	sprintf(str_a,"x%d",game);

	printf("stage1 初期化完了。\n");	//デバッグ用

}


//画面再描画時によばれる(1フレーム毎に呼ばれる)
void Stage1::Disp(){
	keystate = &PublicData->Key.state[0];
	
	//背景描画
	glCallList(DisplayList_BACK);
	
	glLoadIdentity();
	
	//プレイヤーの現在位置
	const Vector3 PlayerPos = PublicData->Player.dosei.pos;
	
	//ライトの位置セット
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	
	//カメラの位置計算
	float _x,_y,_z;
	_x = sin(cam_r) * cos(cam_rx) * cam_z;
	_y = sin(cam_rx) * cam_z;
	_z = cos(cam_r) * cos(cam_rx) * cam_z;
	//カメラの位置セット
	gluLookAt(PlayerPos.x+_x, PlayerPos.y+_y, PlayerPos.z+_z, 
		PlayerPos.x,PlayerPos.y,PlayerPos.z, 0.0,1.0,0.0);

		
	//カメラ移動
	if( keystate['a'] ){
		cam_r += DOSEI_CAMRA;
		if(cam_r>2*M_PI) cam_r -= 2*M_PI;
		//どせいも回転
		PublicData->Player.addAngle( DOSEI_CAMRA*180/M_PI );
	}
	if( keystate['s'] ){
		cam_r -= DOSEI_CAMRA;
		if(cam_r<-2*M_PI) cam_r += 2*M_PI;
		//どせいも回転
		PublicData->Player.addAngle( -DOSEI_CAMRA*180/M_PI );
	}
	if( keystate['q'] && cam_z>5.0){
		cam_z -= DOSEI_CAMRA;
	}
	if( keystate['e'] && cam_z<30.0){
		cam_z += DOSEI_CAMRA;
	}
	if( keystate['w'] && cam_rx > -(M_PI_2-DOSEI_CAMRA-0.01)){
		cam_rx -= DOSEI_CAMRA;
	}
	if( keystate['z'] && cam_rx < M_PI_2-DOSEI_CAMRA-0.01){
		cam_rx += DOSEI_CAMRA;
	}

	//どせい移動
	PublicData->Player.Move(keystate);
	

	//救出(どせいが下に落ちた場合の処理)
	if(PlayerPos.y<-50.0){
		if(game > 0 && game < GAME_CLEAR){	//残機あり、クリアじゃない時
			game--;
			sprintf(str_a,"x%d",game);
		}
		if(game>0){
			//初期位置へ戻す
			PublicData->Player.ResetPos(0.0, 10.0, (PlayerPos.z>210.0 ? 210.0 : 0.0) );
		}
	}
		
		//当たり判定 描画・物理処理等を各オブジェクトごとに行う
	{		
		Vector3 speed, bspeed;
		bspeed = speed = PublicData->Player.UpdateSpeed( GAME_GRAVITY );	//speedの更新と当たり判定前のspeedを保存
		
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
		
		//オブジェクトグループ１
		objg1.Disp_Colli( S_dosei.pos, speed, S_shadow );
		
		//オブジェクトグループ2
		objg2.Disp_Colli( S_dosei.pos, speed, S_shadow );
		
		//オブジェクトグループ3
		objg3.Disp_Colli( S_dosei.pos, speed, S_shadow );
		
		//オブジェクトグループ4
		objg4.Disp_Colli( S_dosei.pos, speed, S_shadow );
		
		//オブジェクトグループ5
		objg5.Disp_Colli( S_dosei.pos, speed, S_shadow );
		
		//摩擦 接地していたら止まる
		PublicData->Player.OnFace( speed.y-bspeed.y != 0.0);

		//適用＆描画
		PublicData->Player.Render(speed);

	
	
	//テクスチャマッピング無効
	//glDisable(GL_BLEND);  //ブレンド
	glDisable(GL_TEXTURE_2D);
	}
	
	
	//FPS出力
	fps(true);

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
			ChangeStage( new Stage2(PublicData) );	//Stage2へ進む
		}
	}else if( game <= 0 ){	//MISS
		glCallList(DisplayList_MISS);	//ミス表示
		if(game_timer++ > 60){
			ChangeStage( new Stage0(PublicData) );	//Stage0に戻る
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

}

//デストラクタ（終了処理）
Stage1::~Stage1(){
	printf("~Stage1\n");
}


/*-----------------------------------------------------------------------------------*
	オブジェクトグループ1
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
OBJgroup1::OBJgroup1() : ita(24.0, 24.0)
{
	Vector3 v( 3.0, 3.0, 3.0 );
	
	box[0].Set(&v,"box1.png");
	box[0].pos.z = 15.0;//-6.0;
//	box[0].pos.y = -0.002; //colli.cppを修正したため、ずらす必要がなくなった。
	
	box[1].Set(&v,"box1.png");
	box[1].pos.z = 25.0;//5.0;
	
	v.z = 4.0;
	box[2].Set(&v,"box1.png");
	box[2].pos.Set( 0.0, 3.0, 31.9);
	
	//Ita初期化
	ita.pos.y = 3.0;
	//当たり判定用、初期化
	colliIta.Set_xz4(ita.vertex);
	//直方体OBJ登録
	Vector3 v1[3];
	v1[0].Set( 4.8, 2.2,  2.2);
	v1[1].Set(-0.2, 7.0, -6.0);
	v1[2].Set(-1.6, 1.8,  2.0);
	box1.Set(v1, "box2.png");	//直方体
	box1.pos.Set( 0.0, 3.0, 42.0);
		
	//バウンディングスフィア計算
	Vector3 Max, min;
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
	Vector3 Cnt = (Max - min) * 0.5;	//Maxとminの中間点が中央
	radius = sqrt( Cnt.x*Cnt.x + Cnt.x*Cnt.x + Cnt.y*Cnt.y + Cnt.z*Cnt.z );	//半径
	Vector3 offset = min + Cnt;	//中心へのベクトル
	for(int i=0; i<3; i++){
		box[i].pos -= offset;
	}
	ita.pos -= offset;
	box1.pos -= offset;
	
	//位置セット
	pos += offset;
	//pos.z += 20.99;
	pos.y -= 3.0;
}

void OBJgroup1::Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow)
{
	Vector3 v(_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z );	
	
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
		Vector3 _v = v;	//box1.NormalForce()でvが破壊されるため、一時的な変数を用意
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

	//影の更新
	//判定柱(判定球をy軸方向へ延長したもの)の中の時に処理する
	if( sqrt( v.x*v.x + v.z*v.z ) <= radius ){
		box1.NormalForceShadow(v, shadow);
		for(int i=0; i<3; i++){
			box[i].NormalForceShadow(v, shadow);
		}
		if( colliIta.ColliLine(shadow.pos, shadow.speed) ){
			shadow.pos.y -= colliIta.GetHigh(shadow.pos) - 0.0001;
			shadow.speed = 0.0;
		}
	}

	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > radius+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		return;
	}

	//当たり判定
	for(int i=0; i<3; i++){
		box[i].NormalForce(v, _speed);
	}
	box[1].NormalForce(v, _speed);	//2度目の当たり判定(すり抜け防止)
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
	Vector3 v( 3.0, 3.0, 3.0 );
	
	box[0].Set(&v, "box2.png");
	box[0].pos.z = -15.0;
	box_pos[0].z = -9.0;
	
	box[1].Set(&v, "box2.png");
	box_pos[1].z = 9.0;
	
	box[2].Set(&v, "box2.png");
	box_pos[2].z = 28.0;
	
	v.Set( 5.0, 3.0, 5.0);
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
	pos += offset;
	pos.z += 65.0;
	pos.y -= 2.0;
	
	//rot初期化
	rot = 0.0;
}

void OBJgroup2::Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow)
{
	Vector3 v( _pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z ), move;	
	
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
	move.x = 0.0;
	move.y = 0.0;
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
	
	//影の更新
	//判定柱(判定球をy軸方向へ延長したもの)の中の時に処理する
	if( sqrt( v.x*v.x + v.z*v.z ) <= radius ){
		for(int i=0; i<4; i++){
			box[i].NormalForceShadow(v, shadow);
		}
	}

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
	Vector3 v( 3.0, 0.1, 3.0 );
	
	//セット
	for(int i=0; i<8; i++)
		box[i].Set(&v, "box3.png");
	
	for(int i=8; i<15; i++)
		box[i].Set(&v, "box4.png");
	
	v.Set( 9.0, 6.0, 6.0 );
	box[15].Set(&v, "box.png");
	
	//位置
	box_pos[0].Set( 15.0, 0.0,  9.0 );
	box_pos[1].Set( -9.0, 0.0,  9.0 );
	box_pos[2].Set(  3.0, 0.0, 27.0 );
	box_pos[3].Set( -3.0, 0.0, 33.0 );
	box_pos[4].Set(  9.0, 0.0, 51.0 );
	box_pos[5].Set( 15.0, 0.0, 63.0 );
	box_pos[6].Set(  3.0, 0.0, 63.0 );
	box_pos[7].Set(  3.0, 0.0, 81.0 );
	//
	box_pos[8].Set(  3.0, 0.0,  9.0 );
	box_pos[9].Set( 15.0, 0.0, 21.0 );
	box_pos[10].Set( -9.0, 0.0, 27.0 );
	box_pos[11].Set(  9.0, 0.0, 33.0 );
	box_pos[12].Set( -3.0, 0.0, 51.0 );
	box_pos[13].Set( -9.0, 0.0, 57.0 );
	box_pos[14].Set(  3.0, 0.0, 69.0 );
	//
	box[15].pos.Set( 3.0, -6.0, 96.0 );
	
	
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
	pos += offset;
	pos.x -= 3.0;
	pos.y += 1.0;
	pos.z += 113.0;
	
	//rot初期化
	rot = 0.0;
	box_action = 0;
}

void OBJgroup3::Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow)
{
	Vector3 v( _pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z ), move;	
	
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
	
	//影の更新
	//判定柱(判定球をy軸方向へ延長したもの)の中の時に処理する
	if( sqrt( v.x*v.x + v.z*v.z ) <= radius ){
		for(int i=0; i<16; i++){
			box[i].NormalForceShadow(v, shadow);
		}
	}

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
	Vector3 v( 3.0, 3.0, 3.0 );
	
	for(int i=0; i<12; i++){
		for(int j=0; j<6; j++){
			box[i*6+j].Set(&v, "box5.png");
			box[i*6+j].pos.Set( j*6.0, 0.0, i*6.0 );
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
	pos += offset;
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

void OBJgroup4::Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow)
{
	Vector3 v( _pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z );	
	bool on=true;
	int a=0;
	
	//影の更新
	//判定柱(判定球をy軸方向へ延長したもの)の中の時に処理する
	if( sqrt( v.x*v.x + v.z*v.z ) <= radius ){
		for(int i=0; i<12*6; i++){
			box[i].NormalForceShadow(v, shadow);
		}
	}

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
	Vector3 v( 3.0, 3.0, 3.0 );
	
	box[0].Set(&v,"box6.png");
	box[0].pos.y = 0.2;
	
	v.Set( 15.0, 3.0, 15.0 );
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
	pos += offset;
	pos.z += 305.0;
	pos.y -= 3.0;
	
	//box[0]の上に乗っているか
	onCLEAR = false;
}

void OBJgroup5::Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow)
{
	Vector3 v( _pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z );	
	
	glPushMatrix();
	
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	
	for(int i=0; i<2; i++){
		box[i].Render();
	}
	
	glPopMatrix();
	
	//影の更新
	//判定柱(判定球をy軸方向へ延長したもの)の中の時に処理する
	if( sqrt( v.x*v.x + v.z*v.z ) <= radius ){
		for(int i=0; i<2; i++){
			box[i].NormalForceShadow(v, shadow);
		}
	}

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






