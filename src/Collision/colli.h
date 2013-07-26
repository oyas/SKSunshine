//colli.h
/*-----------------------------------------------------------------------------------*
 * 当たり判定関係
 * 2011/6
 *-----------------------------------------------------------------------------------*/
#ifndef COLLI_H
#define COLLI_H

#include "../XMesh/XLoader.h"	//Vector3とかを使うため


//type の定義
#define COLLI_TYPE_NONE 0	//未登録
#define COLLI_TYPE_3 1		//三角形
#define COLLI_TYPE_4 2		//四角形
#define COLLI_TYPE_XZ3 3	//XZ平面に並行な三角形
#define COLLI_TYPE_XZ4 4	//XZ平面に並行な四角形


/*-----------------------------------------------------------------------------------*
	当たり判定クラス
	いろいろな図形の当たり判定のため、set系で登録。
 *-----------------------------------------------------------------------------------*/
class Colli{
private:
	unsigned char type;	//登録面の種類 上のdefine参照
	unsigned char numVertices;	//頂点数。3か4
	Vector3 vertex[4];	//登録面。標準で四角形対応
	Vector3 sideNormal[4];	//辺の法線。(01,12,23,30の順)
	Vector3 normal;	//面の法線(正規化済み)
public:
	//当たり判定を行う図形をセット
	void Set_3(const Vector3 *vec);		//当たり判定を行う三角形をセット(vecから3つ分使用)
	void Set_4(const Vector3 *vec);		//四角形（vecから4つ分使用）
	void Set_xz4(const Vector3 *vec);		//XZ平面と並行な四角形（vecから4つ分使用）
	//当たり判定
	float GetHigh(Vector3 vec);	//点までの距離測定
	bool ColliPoint(Vector3 vec);	//点が登録面内かどうか	
	bool ColliNormal(Vector3 vec);	//点が領域内(登録面の垂直方向に伸ばした範囲中にある)かどうか
	bool ColliLine(Vector3 vec, Vector3 fvec);			//直線と登録面が交わるかどうか(始点vecのfvecを直線とする)
	Vector3 GetColliPoint(Vector3 vec, Vector3 fvec);	//衝突点を求める
	Vector3 GetNormalForce(Vector3 vec, Vector3 fvec);	//垂直抗力を返す
	const Vector3 &GetNormal(){return normal;}		//垂直ベクトルを返す
	//コンストラクタ・デストラクタ
	Colli();
	~Colli();
};




#endif	//COLLI_H
