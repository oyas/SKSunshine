//assist.h
// ステージクラスはこのヘッダをインクロードする

#ifndef _ASSIST_H
#define _ASSIST_H

//インクルード
#include "../XMesh/XLoader.h"		//Xファイルロード系
#include "../Collision/colli.h"	//当たり判定系
#include "../Sound/sound.h"		//サウンド系(OpenAL)
#include "../Player/player.h"	//操作キャラクター用クラス
#include <GL/freeglut.h>
#include <cstdio>
#include <cstring>
#include <cmath>



//定数
#define SC_INPUT_MOUSE_DOWN 'a'
#define SC_INPUT_MOUSE_UP 'b'
#define SC_INPUT_MOTION 'c'
#define SC_INPUT_P_MOTION 'd'
#define SC_INPUT_KEY_DOWN 'e'
#define SC_INPUT_KEY_UP 'f'
#define SC_INPUT_SPECIALKEY_DOWN 'g'
#define SC_INPUT_SPECIALKEY_UP 'h'

//宣言
class PublicClass;
class PlayerClass;


//使用する特殊キーの定義
enum ENUM_S_KEY{
	KEY_SK_STARTNUMBER = 128,	//開始番号は128
	KEY_SK_LEFT,
	KEY_SK_RIGHT,
	KEY_SK_UP,
	KEY_SK_DOWN
};
static const int Keys_SK[] = {
	0,	//KEY_SK_STARTNUMBER分のダミー
	GLUT_KEY_LEFT,	//129
	GLUT_KEY_RIGHT,	
	GLUT_KEY_UP,	
	GLUT_KEY_DOWN	
};


/*-----------------------------------------------------------------------------------*
	ステージクラス
	抽象クラス。Main.cppから各イベントが起こったときに呼ばれる。
	各ステージの処理はこのクラスを継承したクラスから行う。
 *-----------------------------------------------------------------------------------*/
class StageClass{
protected:
	//共用クラスへのポインタ
	PublicClass *PublicData;

public:
	//コンストラクタを作って初期化をするとよい。
	StageClass( PublicClass *pd ){
		PublicData = pd;
	}
	
	//画面再描画時によばれる(1フレーム毎に呼ばれる)
	virtual void Disp() = 0;
	
	//2Dの描画
	//Disp()が終わったあと呼ばれる。3D画面の上にステータスを表示するみたいな感じに使う
	//左下原点、最大値(WindowWidth, WindowHeight)の範囲で描画を行う。（座標単位はピクセル単位に相当）
	virtual void Disp2D(int WindowWidth, int WindowHeight){}
	
	//入力があった場合呼ばれる(eventでなんの入力か判断して、必要な引数をとってつかう)
	//入力されている間はdownとupを繰り返すが、upから次のdownの前にdispが呼ばれることは多分ない
	virtual void Input(char event, int key, int x, int y){
		switch(event){
		case SC_INPUT_MOUSE_DOWN:	//マウスのボタンが押されたとき
			break;
		case SC_INPUT_MOUSE_UP:	//マウスのボタンが離されたとき
			break;
		case SC_INPUT_MOTION:	//マウスが動いたとき（ドラッグ中）
			// 常に key=0 である
			break;
		case SC_INPUT_P_MOTION:	//マウスが動いたとき（非ドラッグ中）
			// 常に key=0 である
			break;
		case SC_INPUT_KEY_DOWN:	//キーが押されたとき
			break;
		case SC_INPUT_KEY_UP:	//キーが離されたとき
			break;
		case SC_INPUT_SPECIALKEY_DOWN:	//特殊なキーが押されたとき
			break;
		case SC_INPUT_SPECIALKEY_UP:	//特殊なキーが離されたとき
			break;
		}
	}
	
	//デストラクタ
	virtual ~StageClass(){}
};


/*-----------------------------------------------------------------------------------*
	ステージ変更
	主にステージクラスから呼び出して使う
	これを実行した次のDisp()終了時に、ステージが変更される。
 *-----------------------------------------------------------------------------------*/
void ChangeStage(StageClass* s);	//ステージ変更(Main.cppで定義)sは次のステージのインスタンス


/*-----------------------------------------------------------------------------------*
	FPS測定
	毎フレームごとに呼ぶことで、fpsを計算してくれる。
	print:	trueの時、"FPS: --.-"という形式の文字列を毎秒標準出力に出力する。
 *-----------------------------------------------------------------------------------*/
float fps(bool print);


/*-----------------------------------------------------------------------------------*
	デフォルトマテリアルセット
	マテリアルをデフォルト値に戻す。カラーは白がセットされる
 *-----------------------------------------------------------------------------------*/
void setDefaultMaterial();
 

/*-----------------------------------------------------------------------------------*
	文字描画
	2D専用。描画位置と描画文字列、フォントを指定すると、描いてくれる。
	フォントは、「GLUT_BITMAP_TIMES_ROMAN_24」とかで指定
 *-----------------------------------------------------------------------------------*/
void DRAW_STRING(int x, int y, char *string, void *font);
 


/*-----------------------------------------------------------------------------------*
	キー入力管理クラス
	キーの入力状態をstate[key]に配列として持っておく。
	state[key]は直接参照できるが値の変更はSetKeyState()等を使うべきである。
	key:	状態を変更したいキーを指定する。
	onoff:	状態変更。true=押された false=離された
 *-----------------------------------------------------------------------------------*/
class KeyInput{
public:
	bool state[256];	//キーの入力状態 0:離れている 1:押されている
						//0〜127: ASCII文字コードに対応したキーの状態
						//128〜255: その他の特殊キー(定義は上)
	KeyInput(){
		for(int i=0; i<256; i++)
			state[i] = false;
	}
	bool SetKeyState(unsigned char key, bool onoff);
	bool SetSpecialKeyState(int key, bool onoff);
};



/*-----------------------------------------------------------------------------------*
	PublicClass
	ステージが変わっても保持されるデータやリソースの管理
	PublicData->Key	.state[256]	(bool)キーの入力状態参照
 *-----------------------------------------------------------------------------------*/
class PublicClass{
public:
	KeyInput Key;
	PlayerClass Player;

	PublicClass() : Player(this) {}
};


#endif	//_ASSIST_H
