//colli.cpp
/*-----------------------------------------------------------------------------------*
 * 当たり判定系
 * Set〜()で判定する面を登録。Get〜(),Colli〜()で判定結果を受け取る。
 *-----------------------------------------------------------------------------------*/

#include "colli.h"

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
		//v1特定(i番目の頂点とその次の頂点から)
		v1 = vertex[ (i+1>=numVertices)?0:(i+1) ] - vertex[i];
		//外積
		sideNormal[i].x = v1.y*normal.z - v1.z*normal.y;
		sideNormal[i].y = v1.z*normal.x - v1.x*normal.z;
		sideNormal[i].z = v1.x*normal.y - v1.y*normal.x;
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
	
	//法線計算(外積で)
//	Vector3 v1 = vertex[1] - vertex[0],
//			v2 = vertex[2] - vertex[0];
//	normal.Set( v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x );	//外積
//	NormalizeVec(&normal);	//XLorder_subより。正規化
//		
//	//各辺の法線計算。面の中央から外を向くように
//	for(int i=0; i<4; i++){
//		//v1特定(i番目の頂点とその次の頂点から)
//		v1 = vertex[ (i+1>=4)?0:(i+1) ] - vertex[i];
//		//外積
//		sideNormal[i].x = v1.y*normal.z - v1.z*normal.y;
//		sideNormal[i].y = v1.z*normal.x - v1.x*normal.z;
//		sideNormal[i].z = v1.x*normal.y - v1.y*normal.x;
//		//正規化
//		NormalizeVec(&sideNormal[i]);
//	}
	
	//type登録
	type = COLLI_TYPE_4;	//四角形
	//法線計算等
	CalcNormal( 4 );
}


//XZ平面と平行な四角形の登録
void Colli::Set_xz4(const Vector3 *vec)
{
	//４つの頂点読み込み
	for(int i=0; i<4; i++){
		vertex[i] = vec[i];
	}
	
	//法線
	normal.Set( 0.0, 1.0, 0.0 );
	
	//各辺の法線計算。面の中央から外を向くように
	Vector3 v1;
	for(int i=0; i<4; i++){
		//v1特定(i番目の頂点とその次の頂点から)
		v1 = vertex[ (i+1>=4)?0:(i+1) ] - vertex[i];
		//外積
		sideNormal[i].x = v1.z*normal.y - v1.y*normal.z;
		sideNormal[i].y = v1.x*normal.z - v1.z*normal.x;
		sideNormal[i].z = v1.y*normal.x - v1.x*normal.y;
		//正規化
		NormalizeVec(&sideNormal[i]);
	}
	
	//type登録
	type = COLLI_TYPE_XZ4;	//XZに並行な四角形
	numVertices = 4;
}


//高さ特定
float Colli::GetHigh(Vector3 vec)
{
	switch(type){
	case COLLI_TYPE_3:
	case COLLI_TYPE_4:
		{
			Vector3 v = vec - vertex[0];
			return v.x*normal.x + v.y*normal.y + v.z*normal.z;
		}
	case COLLI_TYPE_XZ4:
		return vec.y - vertex[0].y;
	}
	return 0.0;
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
	Vector3 v;
	//高さ取得
	float high = GetHigh(vec);
	//登録面上に点を合わせる
	v = vec - normal * high;
	//点が登録面内か判定
	return ColliPoint(v);
}


//直線と登録面が交わるかどうか(始点vecのfvecを直線とする)裏から表へは判定なし
bool Colli::ColliLine(Vector3 vec, Vector3 fvec)
{
	Vector3 v;
	float high, fhigh;
	
	switch(type){
	case COLLI_TYPE_3:
	case COLLI_TYPE_4:
		//表から裏　へ突き抜けているかどうか
		high = GetHigh(vec); fhigh = GetHigh(vec+fvec);
		if( (high>=0 && fhigh<=0) ){
			//面と交わる点までの比率を求める
			float r = (high-fhigh)?(high/(high-fhigh)):1.0;
			//面と交わる点を求める
			v = vec + fvec * r;
			//その点が登録面上かどうか
			return ColliPoint(v);
		}
		break;
	case COLLI_TYPE_XZ4:
		//表から裏　へ突き抜けているかどうか
		high = GetHigh(vec);
		if( (high>=0 && high+fvec.y<=0) ){
			//面と交わる点までの比率を求める
			float r = - (fvec.y)?(high / fvec.y):1.0;
			//面と交わる点を求める
			v = vec + fvec * r;
			//その点が登録面上かどうか
			return ColliPoint(v);
		}else{
			return false;	//突き抜けていない
		}
		break;
	}
	return false;
}


//衝突点を求める(GetLineの途中までやるだけ)
Vector3 Colli::GetColliPoint(Vector3 vec, Vector3 fvec)
{
	Vector3 v;
	float high, fhigh;

	switch(type){
	case COLLI_TYPE_3:
	case COLLI_TYPE_4:
		//表から裏　へ突き抜けているかどうか
		high = GetHigh(vec); fhigh = GetHigh(vec+fvec);
		if( (high>=0 && fhigh<=0) ){
			//面と交わる点までの比率を求める
			float r = (high-fhigh)?(high/(high-fhigh)):1.0;
			//面と交わる点を求める
			v = vec + fvec * r;
		}
		break;
	case COLLI_TYPE_XZ4:
		//表から裏　へ突き抜けているかどうか
		high = GetHigh(vec);
		if( (high>=0 && high+fvec.y<=0) ){
			//面と交わる点までの比率を求める
			float r = - (fvec.y)?(high / fvec.y):1.0;
			//面と交わる点を求める
			v = vec + fvec * r;
		}
		break;
	}
	return v;	//衝突してなかったらv=0.0(原点)を返す。
}


//垂直抗力を返す。fvecは点にかかってる力
Vector3 Colli::GetNormalForce(Vector3 vec, Vector3 fvec)
{
	Vector3 v, f(0.0, 0.0, 0.0);
	float high=0.0, fhigh=0.0;
	
	switch(type){
	case COLLI_TYPE_3:
	case COLLI_TYPE_4:
		//表から裏　へ突き抜けているかどうか
		high = GetHigh(vec); fhigh = GetHigh(vec+fvec);
		if( (high>=0 && fhigh<=0) ){
			//面と交わる点までの比率を求める
			float r = (high-fhigh)?(high/(high-fhigh)):1.0;
			//面と交わる点を求める
			v = vec + fvec * r;
			//その点が登録面上かどうか
			if( ColliPoint(v) ){
				//垂直抗力
				f = normal * ( -fhigh + 0.01 );
			}
		}
		break;
	case COLLI_TYPE_XZ4:
		//表から裏　へ突き抜けているかどうか
		high = GetHigh(vec);
		if( (high>=0 && high+fvec.y<=0) ){
			//面と交わる点までの比率を求める
			float r = - (fvec.y)?(high / fvec.y):1.0;
			//面と交わる点を求める
			v = vec + fvec * r;
			//その点が登録面上かどうか
			if( ColliPoint(v) ){
				//面を突き抜けた先の高さ(＋)
				float fr = -fvec.y - high;
				//垂直抗力計算
				f = normal * ( fr + 0.01 );
			}
		}
		break;
	}
	return f;
}


//デストラクタ
Colli::~Colli(){
	
}

