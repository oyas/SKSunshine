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
	Vector3 v = {0.0,0.0,0.0};
	for(int i=0; i<4; i++){
		vertex[i] = v;
		sideNormal[i] = v;
	}
	normal = v;
	MatrixIdentity(mat.m);
}

//三角形の登録
void Colli::Set_3(const Vector3 *vec)
{
	//3つの頂点読み込み
	for(int i=0; i<3; i++){
		vertex[i].x = vec[i].x;
		vertex[i].y = vec[i].y;
		vertex[i].z = vec[i].z;
	}
	
	//法線計算(外積で)
	Vector3 v1 = {vertex[1].x - vertex[0].x,
					vertex[1].y - vertex[0].y,
					vertex[1].z - vertex[0].z },
			v2 = {vertex[2].x - vertex[0].x,
					vertex[2].y - vertex[0].y,
					vertex[2].z - vertex[0].z };
	normal.x = v1.y*v2.z - v1.z*v2.y;	//外積
	normal.y = v1.z*v2.x - v1.x*v2.z;
	normal.z = v1.x*v2.y - v1.y*v2.x;
	NormalizeVec(&normal);	//XLorder_subより。正規化
	
/*	//高さ測定用行列
	MATRIX4x4 m; MatrixIdentity(m.m);
	m.m[3][0]=-vertex[0].x; m.m[3][1]=-vertex[0].y; m.m[3][2]=-vertex[0].z;
	Vector3 VX, VY, VZ;
	//X方向決定
	/*VX.x = normal.y*( 1.0 ) - normal.z*( 0.0 );
	VX.y = normal.z*( 0.0 ) - normal.x*( 1.0 );
	VX.z = normal.x*( 0.0 ) - normal.y*( 0.0 );
	VX.x = normal.y; VX.y = -normal.x; VX.z = 0.0;/
	//Z方向決定
	/*VZ.x = VX.y*normal.z - ( 0.0 )*normal.y;	//外積
	VZ.y = ( 0.0 )*normal.x - VX.x*normal.z;
	VZ.z = VX.x*normal.y - VX.y*normal.x;/
	VZ.x = VX.y*normal.z;
	VZ.y = -VX.x*normal.z;
	VZ.z = VX.x*normal.y - VX.y*normal.x;
	VZ.x=-VZ.x; VZ.z=-VZ.z; 
	//Y方向決定
	VY.x=VZ.x
	//X方向決定
	/*VX.x = normal.y*( 1.0 ) - normal.z*( 0.0 );
	VX.y = normal.z*( 0.0 ) - normal.x*( 1.0 );
	VX.z = normal.x*( 0.0 ) - normal.y*( 0.0 );/
	VX.x = normal.y; VX.y = -normal.x; VX.z = 0.0;
	//行列作成
	mat.m[0][0]=VX.x;      mat.m[0][1]=VX.y;     mat.m[0][2]=VX.z;
	mat.m[1][0]=-normal.x; mat.m[1][1]=normal.y;  mat.m[1][2]=-normal.z; 
	mat.m[2][0]=-VZ.x;     mat.m[2][1]=-VZ.y;     mat.m[2][2]=VZ.z; 
	//mat.m[3][0]=-vertex[0].x; mat.m[3][1]=-vertex[0].y; mat.m[3][2]=-vertex[0].z; 
	Matrixkakeru(mat.m, m.m, mat.m);
*/	
	
	
	
	//各辺の法線計算。面の中央から外を向くように
	for(int i=0; i<3; i++){
		//v1特定(i番目の頂点とその次の頂点から)
		v1.x = vertex[ (i+1>=3)?0:(i+1) ].x - vertex[i].x;
		v1.y = vertex[ (i+1>=3)?0:(i+1) ].y - vertex[i].y;
		v1.z = vertex[ (i+1>=3)?0:(i+1) ].z - vertex[i].z;
		//外積
		sideNormal[i].x = v1.y*normal.z - v1.z*normal.y;
		sideNormal[i].y = v1.z*normal.x - v1.x*normal.z;
		sideNormal[i].z = v1.x*normal.y - v1.y*normal.x;
		//正規化
		NormalizeVec(&sideNormal[i]);
	}
	
	//type登録
	type = COLLI_TYPE_3;	//XZに並行な四角形
	numVertices = 3;
}

//四角形の登録
void Colli::Set_4(const Vector3 *vec)
{
	//４つの頂点読み込み
	for(int i=0; i<4; i++){
		vertex[i].x = vec[i].x;
		vertex[i].y = vec[i].y;
		vertex[i].z = vec[i].z;
	}
	
	//法線計算(外積で)
	Vector3 v1 = {vertex[1].x - vertex[0].x,
					vertex[1].y - vertex[0].y,
					vertex[1].z - vertex[0].z },
			v2 = {vertex[2].x - vertex[0].x,
					vertex[2].y - vertex[0].y,
					vertex[2].z - vertex[0].z };
	normal.x = v1.y*v2.z - v1.z*v2.y;
	normal.y = v1.z*v2.x - v1.x*v2.z;
	normal.z = v1.x*v2.y - v1.y*v2.x;
	NormalizeVec(&normal);	//XLorder_subより。正規化
		
	//各辺の法線計算。面の中央から外を向くように
	for(int i=0; i<4; i++){
		//v1特定(i番目の頂点とその次の頂点から)
		v1.x = vertex[ (i+1>=4)?0:(i+1) ].x - vertex[i].x;
		v1.y = vertex[ (i+1>=4)?0:(i+1) ].y - vertex[i].y;
		v1.z = vertex[ (i+1>=4)?0:(i+1) ].z - vertex[i].z;
		//外積
		sideNormal[i].x = v1.y*normal.z - v1.z*normal.y;
		sideNormal[i].y = v1.z*normal.x - v1.x*normal.z;
		sideNormal[i].z = v1.x*normal.y - v1.y*normal.x;
		//正規化
		NormalizeVec(&sideNormal[i]);
	}
	
	//type登録
	type = COLLI_TYPE_4;	//XZに並行な四角形
	numVertices = 4;
}


//XZ平面と平行な四角形の登録
void Colli::Set_xz4(const Vector3 *vec)
{
	//４つの頂点読み込み
	for(int i=0; i<4; i++){
		vertex[i].x = vec[i].x;
		vertex[i].y = vec[i].y;
		vertex[i].z = vec[i].z;
	}
	
	//法線
	normal.x=0.0; normal.y=1.0; normal.z=0.0;
	
	//各辺の法線計算。面の中央から外を向くように
	Vector3 v1;
	for(int i=0; i<4; i++){
		//v1特定(i番目の頂点とその次の頂点から)
		v1.x = vertex[ (i+1>=4)?0:(i+1) ].x - vertex[i].x;
		v1.y = vertex[ (i+1>=4)?0:(i+1) ].y - vertex[i].y;
		v1.z = vertex[ (i+1>=4)?0:(i+1) ].z - vertex[i].z;
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
		Vector3 v; v = vec-vertex[0];
		return v.x*normal.x + v.y*normal.y + v.z*normal.z;
		break;
	case COLLI_TYPE_XZ4:
		return vec.y - vertex[0].y;
		break;
	}
	return 0.0;
}



//高さ特定(行列使用)
float Colli::GetHighMatrix(Vector3 vec)
{
	switch(type){
	case COLLI_TYPE_3:
		return vec.x*mat.m[0][1] + vec.y*mat.m[1][1] + vec.z*mat.m[2][1] + mat.m[3][1];
		break;
	case COLLI_TYPE_XZ4:
		return vec.y - vertex[0].y;
		break;
	}
	return 0.0;
}


//点が登録面内かどうか	
bool Colli::ColliPoint(Vector3 vec)
{
	Vector3 v;
	for(int i=0; i<numVertices; i++){
		//辺の端から点までのベクトルを求める
		v.x = vec.x - vertex[i].x;
		v.y = vec.y - vertex[i].y;
		v.z = vec.z - vertex[i].z;
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
//	v.y = vec.y - normal.y*high;
//	v.z = vec.z - normal.z*high;
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
			float r = high/(high-fhigh);
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
			float r = - high / fvec.y;
			//面と交わる点を求める
			v = vec + fvec * r;
	//		v.y = vec.y + fvec.y*r;
	//		v.z = vec.z + fvec.z*r;
			//その点が登録面上かどうか
			return ColliPoint(v);
		}else{
			return false;	//突き抜けていない
		}
		break;
	}
	return false;
}


//垂直抗力を返す。fvecは点にかかってる力
Vector3 Colli::GetNormalForce(Vector3 vec, Vector3 fvec)
{
	Vector3 v, f = {0.0, 0.0, 0.0};
	float high=0.0, fhigh=0.0;
	
	switch(type){
	case COLLI_TYPE_3:
	case COLLI_TYPE_4:
		//表から裏　へ突き抜けているかどうか
		high = GetHigh(vec); fhigh = GetHigh(vec+fvec);
		if( (high>=0 && fhigh<=0) ){
			//面と交わる点までの比率を求める
			float r = high/(high-fhigh);
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
			float r = - high / fvec.y;
			//面と交わる点を求める
			v = vec + fvec * r;
		//	v.y = vec.y + fvec.y*r;
		//	v.z = vec.z + fvec.z*r;
			//その点が登録面上かどうか
			if( ColliPoint(v) ){
				//面を突き抜けた先の高さ(＋)
				float fr = -fvec.y - high;
				//垂直抗力計算
				f = normal * ( fr + 0.01 );
		//		f.y += normal.y * fr;
		//		f.z += normal.z * fr;
			}
		}
		break;
	}
	return f;
}


//デストラクタ
Colli::~Colli(){
	
}

