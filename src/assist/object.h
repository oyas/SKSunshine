// object.h
#ifndef _STAGE_OBJECT_H
#define _STAGE_OBJECT_H

//インクルード
#include "assist.h"


//picoPNGへの移行用
#define USE_GLPNG 0



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
	影オブジェクト
	当たり判定なしスプライト
 *-----------------------------------------------------------------------------------*/
class ShadowOBJ{
private:
	GLuint DisplayList;	//ディスプレイリスト
	PNGtexture png;
	void SetRender(float scale);	//ディスプレイリスト作成
	
public:
	Vector3 pos;	//位置
	Vector3 speed;	//速度(当たり判定で使用)
	void Set(const char *texName = NULL, float scale=1.0f);	//登録
	void Render();	//描画
	ShadowOBJ(const char *texName = NULL, float scale=-1.0f);	//コンストラクタ
	~ShadowOBJ();
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
#if !USE_GLPNG
	PNGtexture pngt;	//PNGテクスチャ
#endif
	
public:
	int laston, laston_i;	//前回、面の上に乗っていたかどうか
	Vector3 pos;	//位置
	void Set(Vector3 *vec, const char *texName = NULL);	//登録（ベクトル一つから８つの頂点を決める）
	void rotOriginal();		//回転させる
	void addrot(MATRIX4x4 &m);	//追加回転させる
	void addpos(Vector3 &vec);	//移動させる
	void NormalForce(Vector3 &_pos, Vector3 &_speed);		//垂直抗力の取得と適用
	void NormalForceIce(Vector3 &_pos, Vector3 &_speed);		//垂直抗力の取得と適用(氷のとこ用)
	void NormalForceShadow( Vector3 _pos, ShadowOBJ &shadow );	//影の表示位置の決定と適用
	void Render();	//描画
	float radius(){return r;}	//判定球の半径を返す
	boxOBJ();	//コンストラクタ
	~boxOBJ();
};






#endif	//_STAGE_OBJECT_H
