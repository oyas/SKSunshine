//stage1.h
#ifndef _STAGE1_H
#define _STAGE1_H

//インクロード
#include "../assist.h"




/*-----------------------------------------------------------------------------------*
	三角形オブジェクト
	当たり判定を含む
 *-----------------------------------------------------------------------------------*/
class sankakOBJ{
private:
	Vector3 oVertex[3];	//オリジナル頂点
	Vector3 vertex[3];	//頂点
	Colli colliOBJ;	//当たり判定
	GLuint DisplayList;	//ディスプレイリスト
	float max_y;	//一番高いとこ
	int laston;	//前回、面の上に乗っていたかどうか
	GLuint texturePNG;
	MATRIX4x4 mat;	//前回の回転行列
public:
	Vector3 nvec;	//前回の垂直抗力
	Vector3 pos;	//位置
	void Set(Vector3 *vec);	//三角形登録
	void rot(MATRIX4x4 &m);		//回転させる
	void addrot(MATRIX4x4 &m);	//追加回転させる
	void NormalForce(Vector3 &_pos, Vector3 &_speed);		//垂直抗力の取得と適用
	void Render();	//描画
	sankakOBJ();	//コンストラクタ
	~sankakOBJ();
};




/*-----------------------------------------------------------------------------------*
	直方体オブジェクト
	当たり判定を含む
 *-----------------------------------------------------------------------------------*/
class boxOBJ{
private:
	static const int index[6][4];	//インデックス
	Vector3 oVertex[8];	//オリジナル頂点
	Vector3 vertex[8];	//頂点
	Colli colli[6];	//当たり判定
	GLuint DisplayList;	//ディスプレイリスト
	Vector3 nvec;	//前回の垂直抗力
	GLuint texturePNG;
	float r;	//判定球の半径
	MATRIX4x4 mat;	//前回の回転行列
	GLdouble Matrix[16];	//合計の回転行列
	Vector3 move;	//移動量
	bool isSet;	//Set()が呼ばれたかどうか
public:
	int laston, laston_i;	//前回、面の上に乗っていたかどうか
	Vector3 pos;	//位置
	void Set(Vector3 *vec, const char *texName = NULL);	//登録（ベクトル一つから８つの頂点を決める）
	void rotOriginal();		//回転させる
	void addrot(MATRIX4x4 &m);	//追加回転させる
	void addpos(Vector3 &vec);	//移動させる
	void NormalForce(Vector3 &_pos, Vector3 &_speed);		//垂直抗力の取得と適用
	void NormalForceIce(Vector3 &_pos, Vector3 &_speed);		//垂直抗力の取得と適用(氷のとこ用)
	void Render();	//描画
	float radius(){return r;}	//判定球の半径を返す
	boxOBJ();	//コンストラクタ
	~boxOBJ();
};


/*-----------------------------------------------------------------------------------*
	オブジェクトグループ
	オブジェクトをまとまりにして管理
 *-----------------------------------------------------------------------------------*/
//1
class OBJgroup1{
private:
	boxOBJ box[3];	//ボックス３つ
	float radius;	//判定球の半径
	Vector3 pos;	//位置
public:
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed);	//描画と当たり判定
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
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed);	//描画と当たり判定
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
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed);	//描画と当たり判定
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
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed);	//描画と当たり判定
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
	void Disp_Colli(Vector3 &_pos, Vector3 &_speed);	//描画と当たり判定
	OBJgroup5();
};



/*-----------------------------------------------------------------------------------*
	ステージ１　クラス
	Main.cppでインスタンスが作られ関数が呼ばれる
 *-----------------------------------------------------------------------------------*/
class Stage1 : public StageClass{
private:
	GLfloat light0pos[4];	//ライトの位置
	char *fpstxt, fpstxtb[20];	//fps測定用
	XModel model;		//Xモデルクラス
	XObj dosei;	//Xオブジェクト
	Ita ita, ita2;	//床クラス
	float cam_z, cam_r, cam_rx;	//カメラの向き
	int dosei_action;	//アクション
	Vector3 dash_vec, dash_svec;	//どせいさんの向き(移動時に使用)
	int key_on;	//1:a 2:s 4:w 8:z 16:b 32:n 64:space 128:x
	int yaziru;	//矢印キー等　	1:left 2:right 4:up 8:down
	int total;	//トータルスコア
	char str[64], str_t[64], str_a[64];	//スコア文字列
	Colli colliIta;	//板の当たり判定クラス
	//sankakOBJ sanka;	//三角形クラス
	boxOBJ box;		//直方体
	//char c[512];
	Sound sound;	//サウンドクラス
	int onface;	//当たり判定があったかどうか(床の上に何秒いたか)
	int spacekey;	//スペースキーが押されてから何秒か
	int backjamp;	//バックジャンプ用
	//オブジェクトグループ
	OBJgroup1 objg1;	//オブジェクトグループ１
	OBJgroup2 objg2;	//オブジェクトグループ２
	OBJgroup3 objg3;	//3
	OBJgroup4 objg4;	//4
	OBJgroup5 objg5;	//5
	//背景
	GLuint textureBACK;	//背景のテクスチャ
	GLuint DisplayList_BACK;	//背景のディスプレイリスト
	//MISS
	GLuint textureMISS;	//テクスチャ
	GLuint DisplayList_MISS;	//ディスプレイリスト
	//CLEAR
	GLuint textureCLEAR;	//テクスチャ
	GLuint DisplayList_CLEAR;	//ディスプレイリスト
	//影用
	//GLuint textureShadow;	//テクスチャ
	//GLuint DisplayList_Shadow;	//ディスプレイリスト
	//ゲームクリアーorミス
	char game;	//1〜5:残機 6:CLEAR 0:MISS
	int game_timer;	//ゲーム終了に使う 
	
public:
	//コンストラクタ
	Stage1();
	
	//画面再描画時によばれる(1フレーム毎に呼ばれる)
	virtual void Disp();
	
	//2D
	virtual void Disp2D();
	
	//入力処理
	virtual void Input(char event, int key, int x, int y);
	
	//デストラクタ
	virtual ~Stage1();
};



#endif	//_STAGE1_H
