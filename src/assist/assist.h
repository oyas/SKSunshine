//assist.h
// ステージクラスはこのヘッダをインクロードする

#ifndef _ASSIST_H
#define _ASSIST_H

//インクルード
#include "../XMesh/XLoader.h"		//Xファイルロード系
#include "../Collision/colli.h"	//当たり判定系
#include "../Sound/sound.h"		//サウンド系(OpenAL)
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



/*-----------------------------------------------------------------------------------*
	ステージクラス
	抽象クラス。Main.cppから各イベントが起こったときに呼ばれる。
	各ステージの処理はこのクラスを継承したクラスから行う。
 *-----------------------------------------------------------------------------------*/
class StageClass{
public:
	//コンストラクタを作って初期化をするとよい。
	//StageClass();
	
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
	毎フレームごとに呼ぶことで、"FPS: --.-"という形式の文字列のアドレスを返す。
 *-----------------------------------------------------------------------------------*/
char* fps();


/*-----------------------------------------------------------------------------------*
	床クラス
	床を表示&管理する
 *-----------------------------------------------------------------------------------*/
class Ita{
private:
	GLuint DisplayList;	//ディスプレイリスト
	void SetRender(float u, float v);	//ディスプレイリスト作成
public:
	Vector2 size;	//床のサイズ
	Vector3 pos;		//床の位置
	Vector3 vertex[4];	//当たり判定にでも使え
	
	Ita(float u, float v);		//コンストラクタ
	void Render();	//描画
};


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
	キー入力補助
	KeyBuf: キーの入力状態をもつ変数。Keys配列の番号と対応するビットがそのキーの状態を表す。
	key: 状態を変更したいキー、または調べたいキーを指定する。
	onoff: 状態変更。true=押された false=離された
	Keys: 状態を知りたいキーの配列。
	size: Keysの要素数。
 *-----------------------------------------------------------------------------------*/
//KeyBufのビットを変更する。
bool SetKeyState(int *KeyBuf, int key, bool onoff, int *Keys, int size);

//状態を取得。押されていたらtrue
bool GetKeyState(int *KeyBuf, int key, int *Keys, int size);	//自前でKeyBufのビットを調べることをおすすめする。


/*-----------------------------------------------------------------------------------*
	スプライトクラス
	スプライト(ただの四角形)を表示&管理する
 *-----------------------------------------------------------------------------------*/
/*class Sprite{
private:
	GLuint DisplayList;	//ディスプレイリスト
	void SetRender(float u, float v);	//ディスプレイリスト作成
public:
	Vector2 size;	//床のサイズ
	float x, y, z;		//床の位置
	
	Sprite(float u, float v);		//コンストラクタ
	void Render();	//描画
};*/



/*----------------------------------------------------------------------------------*
	画像表示
	PNG画像を読み込んで、ディスプレイリストを作る
	filename  : 読み込むPNG画像のファイル名
	*texture  : 作成したテクスチャのIDを格納する変数へのポインタ
	pos[4][3] : 表示位置の指定。(左上,左下,右下,右上)z座標は0推奨。NULLを指定するとデフォルトの位置になる。
	BlendON   : アルファブレンド有効
 *----------------------------------------------------------------------------------*/
//GLuin CreatePNGDisplayList(const char *filename, GLuint *texture, float pos[4][3], bool BlendON);


#endif	//_ASSIST_H
