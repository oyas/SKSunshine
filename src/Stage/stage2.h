// stage2.h

#ifndef _STAGE2_H
#define _STAGE2_H

//インクルード
#include "../assist.h"
#include "object.h"
#include "stage1.h"

#include <stdlib.h>	//乱数を使うので
#include <time.h>	//乱数初期化用


//定数
#define X_RETU 20	//横マス目
#define Y_GYO 20	//縦
#define ST_MAX 3	//１度に伸ばす最大距離

#define BOX_SIZE 8.0 //一マスの大きさ


/*-----------------------------------------------------------------------------------*
		迷路作成
 *-----------------------------------------------------------------------------------*/

class OBJgroup_Meiro{
private:
	boxOBJ box, box_goal;	//ボックス
	Vector3 pos;	//位置

	//迷路用配列
	int miti[X_RETU][Y_GYO], yokoba[X_RETU][Y_GYO+1], tateba[X_RETU+1][Y_GYO];
	
	//ゴール関係
	int miti_max;	//スタートからの最大の距離
	int miti_x, miti_y;	//↑そのポイントの座標(ゴールの座標)
	
	//前回チェックした場所（そこから道をのばした）
	int check_x, check_y;
	
	//現在道をのばすか検討中の場所
	int now_x, now_y;
	
	//迷路が完成したかどうか
	bool end_f;
	
	
	//迷路作成関数
	
	void check();
	
	bool nobasu();
	
	int tuna(int x, int y);

	//当たり判定用
	void box_colli(int x, int y, boxOBJ &box, Vector3 &v, Vector3 &_speed );

public:
	bool onCLEAR;	//ゴール判定
	OBJgroup_Meiro();	
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed);	//描画と当たり判定
};


/*-----------------------------------------------------------------------------------*
	ステージ２　クラス
	stage0.cppでインスタンスが作られ関数が呼ばれる
 *-----------------------------------------------------------------------------------*/
class Stage2 : public StageClass{
private:
	//どせい関連	
	XModel model;		//Xモデルクラス
	XObj dosei;	//Xオブジェクト
	Vector3 dash_vec;	//どせいさんの向き-前方向(移動時に使用)
	Vector3 dash_svec;	//どせいさんの向き-横方向(移動時に使用)
	//ライト
	GLfloat light0pos[4];	//ライトの位置
	char *fpstxt, fpstxtb[20];	//fps測定用
	float cam_z, cam_r, cam_rx;	//カメラの向き z:距離 r:横周り rx:縦周り
	int key_on;	//キーの状態
	int yaziru;	//特殊キーの状態 矢印キー等
	int total;	//トータルスコア
	char str[64], str_t[64], str_a[64];	//スコア文字列
	//sankakOBJ sanka;	//三角形クラス
	//char c[512];
	Sound sound;	//サウンドクラス
	int onface;	//当たり判定があったかどうか(床の上に何秒いたか)
	int spacekey;	//スペースキーが押されてから何秒か
	int backjamp;	//バックジャンプ用 特殊ジャンプ用
	//オブジェクトグループ
	OBJgroup1 objg1;	//オブジェクトグループ１
	OBJgroup_Meiro objg_meiro; //迷路
	//背景
	GLuint textureBACK;	//背景のテクスチャ
	GLuint DisplayList_BACK;	//背景のディスプレイリスト
	//MISS
	GLuint textureMISS;	//テクスチャ
	GLuint DisplayList_MISS;	//ディスプレイリスト
	//CLEAR
	GLuint textureCLEAR;	//テクスチャ
	GLuint DisplayList_CLEAR;	//ディスプレイリスト
	//ゲームクリアーorミス
	char game;	//1〜5:残機 6:CLEAR 0:MISS
	int game_timer;	//ゲーム終了に使うtimer 
	//どせい高速化用
	bool turbo;
	
public:
	//コンストラクタ
	Stage2();
	
	//画面再描画時によばれる(1フレーム毎に呼ばれる)
	virtual void Disp();
	
	//2D
	virtual void Disp2D(int Width, int Height);
	
	//入力処理
	virtual void Input(char event, int key, int x, int y);
	
	//デストラクタ
	virtual ~Stage2();
};



#endif	//_STAGE2_H
