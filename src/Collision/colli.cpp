//colli.cpp
/*-----------------------------------------------------------------------------------*
 * 当たり判定系
 * Set〜()で判定する面を登録。Get〜(),Colli〜()で判定結果を受け取る。
 *-----------------------------------------------------------------------------------*/

#include "colli.h"

#define _GETNORMALFORCE 1	//新しい_GetNormalForce()を使用するかどうか

//コンストラクタ
Colli::Colli(){
	type = COLLI_TYPE_NONE;
	numVertices = 0;
}

//法線計算等
void Colli::CalcNormal(int _numVertices){
	//頂点の数登録
	numVertices = _numVertices;

	if( numVertices < 3 )
		return;
	
	//法線計算(外積で)
	Vector3 v1 = vertex[1] - vertex[0],
			v2 = vertex[2] - vertex[0];
	normal.Set( v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x );	//外積
	NormalizeVec(&normal);	//正規化
	
	
	//各辺の法線計算。面の中央から外を向くように
	for(int i=0; i<numVertices; i++){
		//辺を表すベクトル特定(i番目の頂点とその次の頂点から)
		vector[i] = vertex[ (i+1>=numVertices)?0:(i+1) ] - vertex[i];
		//外積
		sideNormal[i].x = vector[i].y*normal.z - vector[i].z*normal.y;
		sideNormal[i].y = vector[i].z*normal.x - vector[i].x*normal.z;
		sideNormal[i].z = vector[i].x*normal.y - vector[i].y*normal.x;
		//正規化
		NormalizeVec(&sideNormal[i]);
	}
}
	


//三角形の登録
void Colli::Set_3(const Vector3 *vec)
{
	//3つの頂点読み込み
	for(int i=0; i<3; i++){
		vertex[i] = vec[i];
	}
	//type登録
	type = COLLI_TYPE_3;	//三角形
	//法線計算等
	CalcNormal( 3 );
}

//四角形の登録
void Colli::Set_4(const Vector3 *vec)
{
	//４つの頂点読み込み
	for(int i=0; i<4; i++){
		vertex[i] = vec[i];
	}
	
	//type登録
	type = COLLI_TYPE_4;	//四角形
	//法線計算等
	CalcNormal( 4 );
}


//高さ特定
float Colli::GetHigh(Vector3 vec)
{
	Vector3 v = vec - vertex[0];
	return v.x*normal.x + v.y*normal.y + v.z*normal.z;
}



//点が登録面内かどうか	
bool Colli::ColliPoint(Vector3 vec)
{
	Vector3 v;
	for(int i=0; i<numVertices; i++){
		//辺の端から点までのベクトルを求める
		v = vec - vertex[i];
		//内積より、辺の外側か内側か求める
		if( v.x*sideNormal[i].x + v.y*sideNormal[i].y + v.z*sideNormal[i].z > 0.0 ){
			return false;	//外側
		}
	}
	return true;
}


//点が領域内(登録面の垂直方向に伸ばした範囲中にある)かどうか
bool Colli::ColliNormal(Vector3 vec)
{
//	Vector3 v;
	//高さ取得
//	float high = GetHigh(vec);
	//登録面上に点を合わせる
//	v = vec - normal * high;
	//点が登録面内か判定
//	return ColliPoint(v);
	return ColliPoint( vec );
}


//直線と登録面が交わるかどうか(始点vecのfvecを直線とする)裏から表へは判定なし
bool Colli::ColliLine(Vector3 vec, Vector3 fvec)
{
	Vector3 v;
	float high, fhigh;
	
	//表から裏　へ突き抜けているかどうか
	high = GetHigh(vec);
	fhigh = GetHigh(vec+fvec);
	if( (high>=0.0 && fhigh<=0.0) ){
		//面と交わる点までの比率を求める
		float r = (high-fhigh)?(high/(high-fhigh)):0.0;
		//面と交わる点を求める
		v = vec + fvec * r;
		//その点が登録面上かどうか
		return ColliPoint(v);
	}
	return false;
}


//衝突点を求める(GetLineの途中までやるだけ)
Vector3 Colli::GetColliPoint(Vector3 vec, Vector3 fvec)
{
	Vector3 v;
	float high, fhigh;

		//表から裏　へ突き抜けているかどうか
		high = GetHigh(vec);
		fhigh = GetHigh(vec+fvec);
		if( (high>=0.0 && fhigh<=0.0) ){
			//面と交わる点までの比率を求める
			float r = (high-fhigh)?(high/(high-fhigh)):0.0;
			//面と交わる点を求める
			v = vec + fvec * r;
		}
	return v;	//衝突してなかったらv=0.0(原点)を返す。
}


//垂直抗力を返す。fvecは点にかかってる力
Vector3 Colli::GetNormalForce(Vector3 vec, Vector3 fvec)
{
#if _GETNORMALFORCE
	return _GetNormalForce(vec, fvec);
#endif

	Vector3 v, f;
	float high, fhigh;
	
	//表から裏　へ突き抜けているかどうか
	high = GetHigh(vec);
	fhigh = GetHigh(vec+fvec);
	if( (high>=0.0 && fhigh<=0.0) ){
		//面と交わる点までの比率を求める
		float r = (high-fhigh)?(high/(high-fhigh)):0.0;
		//面と交わる点を求める
		v = vec + fvec * r;
		//その点が登録面上かどうか
		if( ColliPoint(v) ){
			//垂直抗力
			f = normal * ( -fhigh + 0.01 );
		}
	}
	return f;
}

// 垂直抗力を返す。(新しい実装。外積を用いる)
Vector3 Colli::_GetNormalForce(Vector3 vec, Vector3 fvec){
	Vector3 v, v2, f;
	float high, fhigh;
	
	//表から裏　へ突き抜けているかどうか
	high = GetHigh(vec);
	fhigh = GetHigh(vec+fvec);
	if( (high>=0.0 && fhigh<=0.0) ){
		//面と線分の当たり判定
		for(int i=0; i<numVertices; i++){
			//辺の端から点までのベクトルを求める
			v = vec - vertex[i];
			//外積
			v2.x = vector[i].y*v.z - vector[i].z*v.y;
			v2.y = vector[i].z*v.x - vector[i].x*v.z;
			v2.z = vector[i].x*v.y - vector[i].y*v.x;
			//内積より、辺の外側か内側か求める
			if( fvec.x*v2.x + fvec.y*v2.y + fvec.z*v2.z > 0.0 ){
				return f;	//外側。(0,0,0)を返す
			}
		}
		//垂直抗力計算
		f = normal * ( -fhigh + 0.01 );
	}
	return f;
}



//デストラクタ
Colli::~Colli(){
	
}

