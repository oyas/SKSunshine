// vector_math.h
/***********************************************************************************
 * vector3(３次元ベクトル)や、行列計算系関数など
 ***********************************************************************************/
#ifndef _VECTOR_MATH_H
#define _VECTOR_MATH_H


struct Vector2;
struct Vector3;
struct MATRIX4x4;


//ベクトル正規化
void NormalizeVec(Vector3 *v);	//ベクトル正規化
Vector3 NormalizeVector(Vector3 v);	//返り値がベクトル


//行列の操作

//行列の初期化。引数fは4x4の行列。ただの配列
void MatrixIdentity(float f[4][4]);

//行列の掛け算。o = a * b
void Matrixkakeru(float o[4][4], float a[4][4], float b[4][4]);

//行列の掛け算。o = a * b
void Matrixkakeru16(float o[16], float a[16], float b[4][4]);

//ベクトルの掛け算。[4] = [4] * [4x4]
void Matrixkakeru(float o[4], float a[4], float b[4][4]);

//ベクトルの正規化
void NormalizeVec(Vector3 *v);

Vector3 NormalizeVector(Vector3 v);

//アニメーション用。AnimateFrame()専用。

//行列をweight倍にして加算。 o[4x4] += a[4x4] * weight
void MatrixTasuW(float o[4][4], float a[4][4], float weight);

//行列をキーの比率に合わせていろいろして加算。AnimateFrame()専用。
// o[4x4] += ( a[4x4]*s + b[4x4]*(1-s) ) * weight
void MatrixKeyTasuRW(float o[4][4], float a[4][4], float b[4][4], float s, float weight);


//行列表示(バグチェックのため)
void MatrixPrintf(float a[4][4]);



//
// Vector2 struct
//
struct Vector2
{
	float x, y; 
	operator float* () { return (float*)&x; }
	operator const float*() const { return (const float*)&x; }
};

//
// Vector3 struct
//
struct Vector3
{ 
	float x, y, z; 
	operator float*() { return (float*)&x; }
	operator const float*() const { return (const float*)&x; }
	Vector3 &operator +=(const Vector3 &in){ this->x += in.x; this->y += in.y; this->z += in.z; return *this; }
	Vector3 &operator -=(const Vector3 &in){ this->x -= in.x; this->y -= in.y; this->z -= in.z; return *this; }
	Vector3 operator + (const Vector3 &in){ Vector3 v = *this; v += in; return v; }
	Vector3 operator - (const Vector3 &in){ Vector3 v = *this; v -= in; return v; }
	Vector3 &operator = (const float &in){ x=in; y=in; z=in; return *this; }
	Vector3 operator * (const float &in){
		Vector3 out;
		out.x = this->x*in; out.y = this->y*in; out.z = this->z*in;
		return out;
	}
	Vector3(float _x=0.0, float _y=0.0, float _z=0.0){ x=_x; y=_y; z=_z; }
	void Set(float _x, float _y, float _z){ x=_x; y=_y; z=_z; }
};

//
// MATRIX4x4 struct
//
struct MATRIX4x4{
	float m[4][4];
	MATRIX4x4 &operator = (const MATRIX4x4 &in){
		for(int g=0; g<4; g++)
			for(int r=0; r<4; r++)
				this->m[g][r] = in.m[g][r];
		return *this;
	}
};


#endif	//　_X_LOADER_H_INCLUDED_
