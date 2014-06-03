// main.cpp
/*-----------------------------------------------------------------------------------*
 *	OpenGLテンプレート（ゲームプログラミング用）
 *-----------------------------------------------------------------------------------*/

#include "assist/assist.h"
#include "Stage/stage0.h"	//最初のステージクラス
//定数
#define GAMEMODE_FULLSCREEN 0	//ゲームモードを有効にする
#define TIMER_WAIT 33	//タイマーの待ち時間（0だとタイマー無効）
#define FIRST_STAGECLASS Stage0	//最初に呼ばれるステージクラス


//グローバル変数の宣言
int WindowWidth = 400;	//ウィンドウ幅
int WindowHeight = 400;	//ウィンドウ高さ
const char WindowTitle[] = "kunekune";	//ウィンドウタイトル

StageClass *Stage = NULL, *NextStage = NULL;	//ステージクラスのポインタ
bool FullScreenMode = false;	//フルスクリーンモードかどうか
PublicClass *PublicData = NULL;	//共用クラス

//関数のプロトタイプ宣言
void Init();
void Disp();	//Display(), Timer() から呼ばれる。描画処理本体
void Display();
void Idle();
void Reshape(int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void PassiveMotion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void Special(int key, int x, int y);
void SpecialUp(int key, int x, int y);
void FullScreen();
void Close();
void Timer(int);
//void ChangeStage(StageClass* s);	//外部から呼ぶことがあるため宣言はassistの中
void toNextStage();



/*-----------------------------------------------------------------------------------*
    エントリーポイント、初期化
 *-----------------------------------------------------------------------------------*/

// アプリケーションのエントリーポイント
int main(int argc, char *argv[])
{
	//GLUT初期化
	glutInit(&argc, argv);
	//ウィンドウ作成
	glutInitWindowSize(WindowWidth, WindowHeight);	//ウィンドウサイズ
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);	//RGBAカラー、zバッファ、ダブルバッファリング
#if GAMEMODE_FULLSCREEN
	glutGameModeString("800x600:24@50");
	//glutGameModeString("1024x600:24@60");
	//チェック
	if(!glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)){
		printf("無理ぽい\n");
		//終了処理
		Close();
		exit(0);
	}
	glutEnterGameMode();
	glutSetCursor(GLUT_CURSOR_NONE);	//マウスカーソルを消す
	FullScreenMode = true;
#else
	glutCreateWindow(WindowTitle);
#endif
	
	//　GLEWの初期化
#ifdef WIN32
	if ( glewInit() != GLEW_OK )
	{
		printf("GLEWの初期化に失敗しました。\n");
		Close();
		exit(0);
	}
#endif
	
	//　ALUT初期化
	alutInit(&argc,argv);
	
	//コールバック
	glutDisplayFunc(Display);	//再描画時
	glutCloseFunc(Close);	//ウィンドウが閉じられたとき
	glutReshapeFunc(Reshape);	//ウィンドウサイズ変更時
	glutMouseFunc(Mouse);	//マウスのボタン
	glutMotionFunc(Motion);	//ドラッグ時
	glutPassiveMotionFunc(PassiveMotion);	//ドラッグしてないときのマウス移動
	glutKeyboardFunc(Keyboard);	//キー入力
	glutKeyboardUpFunc(KeyboardUp);	//キーが離された
	glutSpecialFunc(Special);	//特別なキーの入力
	glutSpecialUpFunc(SpecialUp);	//特別なキーが離された
	
	//初期化
	Init();
	
#if TIMER_WAIT
	glutTimerFunc( TIMER_WAIT, Timer, 0);	//一定時間ごとに呼ばれる
#else
	glutIdleFunc(Idle);		//アイドル状態になったとき
#endif
	
	//メインループ
	glutMainLoop(); //※決して返ってこない
printf("おわりだよう！！\n");	
	return 0;
}

//初期化
void Init(void)
{
	//　バックバッファをクリアする色の指定
	glClearColor(0.5, 0.5, 1.0, 1.0);
	
	//共用クラスをnewする
	PublicData = new PublicClass();
	
#define FIRST_STAGECLASS_FULLPRM FIRST_STAGECLASS(PublicData)
	//最初のステージクラスをnewする
	Stage = new FIRST_STAGECLASS_FULLPRM;
	//次のステージクラスはNULLにしとく
	NextStage = NULL;

	//　深度テストON
	glEnable(GL_DEPTH_TEST);
	
	//ライト
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	//片面表示
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	//ブレンドの設定
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	//　ウィンドウを表示
	glutShowWindow();
}



/*-----------------------------------------------------------------------------------*
    画面描き換え
 *-----------------------------------------------------------------------------------*/

// 描画
void Disp(void)
{
	//　バックバッファをクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//　モデルビュー行列の設定
	glMatrixMode(GL_MODELVIEW);
	Stage->Disp();	//ステージDisp()
	
	//2Dへ
	glMatrixMode(GL_PROJECTION); //PROJECTION行列を操作に変更
	glPushMatrix(); //Push
	glLoadIdentity();
	gluOrtho2D(0, WindowWidth, 0, WindowHeight); //ビューイング領域を設定(正射影で設定)
	glMatrixMode(GL_MODELVIEW); //MODELVIEW行列を操作
	glPushMatrix(); //Push
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);//いろいろ無効にする
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	//描画
	Stage->Disp2D(WindowWidth, WindowHeight);	//ステージDisp2D()
	//元に戻す
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glPopMatrix(); //Pop
	glMatrixMode(GL_PROJECTION); 
	glPopMatrix(); //Pop

	//ステージ変更処理
	if( NextStage ) toNextStage();
}

// 画面描き換え
void Display(){
	//描画
	Disp();
	//ダブルバッファ入れ替え
	glutSwapBuffers();
}



/*-----------------------------------------------------------------------------------*
    タイマー、ウィンドウサイズ変更時
 *-----------------------------------------------------------------------------------*/

//タイマー
void Timer(int value){
	glutTimerFunc( TIMER_WAIT, Timer, 0);
	glutSwapBuffers();	//ダブルバッファ入れ替え
	Disp();	//次フレームの描画
}

// アイドリング時の処理
void Idle(void)
{
	glutPostRedisplay();	// 暇な時に再描画
}

// ウィンドウサイズ変更
void Reshape(int x, int y)
{
	//　ウィンドウサイズを保存
	WindowWidth = x;
	WindowHeight = y;
	
	//　ウィンドウサイズを16:9に制限
	if( !FullScreenMode && (float)WindowWidth / (float)WindowHeight > 16.0 / 9.0 ){
		glutReshapeWindow( WindowHeight * 16 / 9 , WindowHeight );
	}
	
	//　ビューポートの設定
	glViewport(0, 0, WindowWidth, WindowHeight);
	
	//　射影行列の設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0);	//設定
}



/*-----------------------------------------------------------------------------------*
    入力関係
 *-----------------------------------------------------------------------------------*/

// マウス入力処理
void Mouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN){
		Stage->Input(SC_INPUT_MOUSE_DOWN, button, x, y);
	}else if(state == GLUT_UP){
		Stage->Input(SC_INPUT_MOUSE_UP, button, x, y);
	}
}

// マウスドラッグ時
void Motion(int x, int y)
{
	Stage->Input(SC_INPUT_MOTION, 0, x, y);
	
}

// マウス移動時(ドラッグしてないとき)
void PassiveMotion(int x, int y)
{
	Stage->Input(SC_INPUT_P_MOTION, 0, x, y);
}

// キーボード処理
void Keyboard(unsigned char key, int x, int y)
{
	PublicData->Key.SetKeyState(key, true);
	Stage->Input(SC_INPUT_KEY_DOWN, (int)key, x, y);
	
	switch ( key ){
	case '\033':	//Esc
	//	Close();
		exit(0);
		break;
	}
}

//キーが離された
void KeyboardUp(unsigned char key, int x, int y)
{
	PublicData->Key.SetKeyState(key, false);
	Stage->Input(SC_INPUT_KEY_UP, (int)key, x, y);
}

// 特殊キー処理
void Special(int key, int x, int y)
{
	PublicData->Key.SetSpecialKeyState(key, true);
	Stage->Input(SC_INPUT_SPECIALKEY_DOWN, key, x, y);
	
	switch ( key )
	{
	case GLUT_KEY_F1:
		FullScreen();
		break;
/*
	case GLUT_KEY_F2:
		break;

	case GLUT_KEY_F3:
		break;

	case GLUT_KEY_F4:
		break;

	case GLUT_KEY_F5:
		break;

	case GLUT_KEY_F6:
		break;

	case GLUT_KEY_F7:
		break;

	case GLUT_KEY_F8:
		break;

	case GLUT_KEY_F9:
		break;

	case GLUT_KEY_F10:
		break;

	case GLUT_KEY_F11:
		break;

	case GLUT_KEY_F12:
		break;

	case GLUT_KEY_LEFT:
		break;

	case GLUT_KEY_RIGHT:
		break;

	case GLUT_KEY_UP:
		break;

	case GLUT_KEY_DOWN:
		break;

	case GLUT_KEY_PAGE_UP:
		break;

	case GLUT_KEY_PAGE_DOWN:
		break;

	case GLUT_KEY_HOME:
		break;

	case GLUT_KEY_END:
		break;

	case GLUT_KEY_INSERT:
		break;*/
	}
}

// 特殊キー処理
void SpecialUp(int key, int x, int y)
{
	PublicData->Key.SetSpecialKeyState(key, false);
	Stage->Input(SC_INPUT_SPECIALKEY_UP, key, x, y);
}


/*-----------------------------------------------------------------------------------*
    その他
 *-----------------------------------------------------------------------------------*/

// フルスクリーン(ゲームモードじゃない)
void FullScreen(void)
{
	printf("fullscreen\n");
#if !GAMEMODE_FULLSCREEN
	static int beforeWidth;
	static int beforeHeight;
	FullScreenMode = !FullScreenMode;
	if ( FullScreenMode ){
		//フルスクリーンへ
		beforeWidth = WindowWidth;
		beforeHeight = WindowHeight;
		glutFullScreen();
		glutSetCursor(GLUT_CURSOR_NONE);
	}else{	//復帰
		glutReshapeWindow(beforeWidth, beforeHeight);
		glutShowWindow();
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
#endif
}

//ステージ変更(Stageクラスから呼ばれる)
void ChangeStage(StageClass* s)
{
	NextStage = s;
}
//次のステージへ変更処理(Disp()からのみ呼ばれる)
void toNextStage(){
	delete Stage;
	Stage = NextStage;
	NextStage = NULL;
}

// 終了処理
void Close(void)
{
	printf("finish! - Main\n");
	//ゲームモード終了
	if( glutGameModeGet(GLUT_GAME_MODE_ACTIVE) )
		glutLeaveGameMode();
	
	//ステージクラスを破棄する。デストラクタが呼ばれる。
	delete Stage;
	Stage = NULL;

	//共用クラスを破棄する。
	delete PublicData;
	PublicData = NULL;
	
	// ALUT終了
	alutExit();
}
