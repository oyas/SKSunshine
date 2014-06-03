// stage0.h

#include "../assist/assist.h"

class Stage0 : public StageClass{
private:
	//背景
	PNGtexture texture_BACK;	//テクスチャ・ディスプレイリスト
	// タイトル画像
	PNGtexture texture_TITLE;	//テクスチャ・ディスプレイリスト
	//「スペースキーでスタート」
	PNGtexture texture_Space;	//テクスチャ・ディスプレイリスト
	//説明画像
	PNGtexture texture_Setumei;	//テクスチャ・ディスプレイリスト
	//点滅用タイマー
	int timer;
	//説明を表示するかのカウント（5までたまると説明表示）
	int setumei;
	//説明を表示しているかどうか
	bool setu_mes;
	
public:
	//コンストラクタ
	Stage0( PublicClass *pd );
	
	//画面再描画時によばれる(1フレーム毎に呼ばれる)
	void Disp();
	
	//2Dの描画
	void Disp2D(int Width, int Height);
	
	//入力処理
	void Input(char event, int key, int x, int y);
	
	//デストラクタ
	~Stage0();
};
