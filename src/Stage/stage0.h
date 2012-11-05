// stage0.h

#include "../assist.h"

class Stage0 : public StageClass{
private:
	//背景
	GLuint textureBACK;	//背景のテクスチャ
	GLuint DisplayList_BACK;	//背景のディスプレイリスト
	// タイトル画像
	GLuint textureTITLE;	//テクスチャ
	GLuint DisplayList_TITLE;	//ディスプレイリスト
	//「スペースキーでスタート」
	GLuint textureSpace;	//テクスチャ
	GLuint DisplayList_Space;	//ディスプレイリスト
	//説明画像
	GLuint textureSetumei;	//テクスチャ
	GLuint DisplayList_Setumei;	//ディスプレイリスト
	//点滅用タイマー
	int timer;
	//説明を表示するかのカウント（10以上で説明表示）
	int setumei;
	//spaceが押されているかどうか
	bool space, setu_mes;
	
public:
	//コンストラクタ
	Stage0();
	
	//画面再描画時によばれる(1フレーム毎に呼ばれる)
	void Disp();
	
	//2Dの描画
	void Disp2D(int Width, int Height);
	
	//入力処理
	void Input(char event, int key, int x, int y);
	
	//デストラクタ
	~Stage0();
};
