//stage1.h
#ifndef _STAGE1_H
#define _STAGE1_H

//インクルード
#include "../assist/assist.h"
#include "../assist/object.h"



/*-----------------------------------------------------------------------------------*
	オブジェクトグループ
	オブジェクトをまとまりにして管理
 *-----------------------------------------------------------------------------------*/
//1
class OBJgroup1{
private:
	boxOBJ box[3];	//ボックス３つ
	Ita ita;	//床クラス
	Colli colliIta;	//板の当たり判定クラス
	boxOBJ box1;		//直方体
	float radius;	//判定球の半径
	Vector3 pos;	//位置
public:
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow);	//描画と当たり判定
	OBJgroup1();
};

//2
class OBJgroup2{
private:
	boxOBJ box[4];	//ボックス4つ
	Vector3 box_pos[3];	//boxは動くため、初期位置を記憶しておく
	float radius;	//判定球の半径
	Vector3 pos;	//位置
	float rot;	//回転角(各boxの位置)
public:
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow);	//描画と当たり判定
	OBJgroup2();
};

//3
class OBJgroup3{
private:
	boxOBJ box[16];	//ボックス8x2
	Vector3 box_pos[15];	//boxは動くため、初期位置を記憶しておく
	float radius;	//判定球の半径
	Vector3 pos;	//位置
	int box_action;	//boxの動作 1:奥へ移動 2:回転 3:前へ移動 4:回転
	float rot;	//回転角(各boxの位置)
public:
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow);	//描画と当たり判定
	OBJgroup3();
};

//4
class OBJgroup4{
private:
	boxOBJ box[72];	//ボックス12*6つ
	//Vector3 box_pos[3];	//boxは動くため、初期位置を記憶しておく
	float radius;	//判定球の半径
	Vector3 pos;	//位置
	int box_action;	//boxの動作 
	int rot;	//回転角(各boxの位置)
	MATRIX4x4 matx, matz, mat_z;	//回転行列
public:
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow);	//描画と当たり判定
	OBJgroup4();
};

//5
class OBJgroup5{
private:
	boxOBJ box[2];	//ボックス2つ
	float radius;	//判定球の半径
	Vector3 pos;	//位置
public:
	bool onCLEAR;	//box[0]の上に乗っているかどうか
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed, ShadowOBJ &shadow);	//描画と当たり判定
	OBJgroup5();
};



/*-----------------------------------------------------------------------------------*
	ステージ１　クラス
	Main.cppでインスタンスが作られ関数が呼ばれる
 *-----------------------------------------------------------------------------------*/
class Stage1 : public StageClass{
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
	OBJgroup2 objg2;	//オブジェクトグループ２
	OBJgroup3 objg3;	//3
	OBJgroup4 objg4;	//4
	OBJgroup5 objg5;	//5
	//背景
	PNGtexture png_back;
	GLuint textureBACK;	//背景のテクスチャ
	GLuint DisplayList_BACK;	//背景のディスプレイリスト
	//MISS
	PNGtexture png_miss;
	GLuint textureMISS;	//テクスチャ
	GLuint DisplayList_MISS;	//ディスプレイリスト
	//CLEAR
	PNGtexture png_clear;
	GLuint textureCLEAR;	//テクスチャ
	GLuint DisplayList_CLEAR;	//ディスプレイリスト
	//影用
	ShadowOBJ shadow;
	//ゲームクリアーorミス
	char game;	//1〜5:残機 6:CLEAR 0:MISS
	int game_timer;	//ゲーム終了に使うtimer 
	
public:
	//コンストラクタ
	Stage1();
	
	//画面再描画時によばれる(1フレーム毎に呼ばれる)
	virtual void Disp();
	
	//2D
	virtual void Disp2D(int Width, int Height);
	
	//入力処理
	virtual void Input(char event, int key, int x, int y);
	
	//デストラクタ
	virtual ~Stage1();
};



#endif	//_STAGE1_H
