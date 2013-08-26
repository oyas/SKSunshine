// vector_math.cpp

#include "vector_math.h"

#include<cmath>
#include<cstdio>


//行列の基本計算定義
//なお、各関数に見られるt[]は、たとえばo==aのときに備えて必ず必要。
//行列の初期化。引数fは4x4の行列。ただの配列
void MatrixIdentity(float f[4][4])
{
	for(int g=0; g<4; g++)
		for(int r=0; r<4; r++)
			if(g==r)
				f[g][r] = 1.0;
			else
				f[g][r] = 0.0;
}

//行列の合成。o = a * b
void Matrixkakeru(float o[4][4], float a[4][4], float b[4][4])
{
	float t[4][4];
	for(int g=0; g<4; g++){
		for(int r=0; r<4; r++){
			t[g][r] = a[g][0]*b[0][r] + a[g][1]*b[1][r] +
						a[g][2]*b[2][r] + a[g][3]*b[3][r];
		}
	}
	for(int g=0; g<4; g++)
		for(int r=0; r<4; r++)
			o[g][r] = t[g][r];
}

//行列の合成。o = a * b
void Matrixkakeru16(float o[16], float a[16], float b[4][4])
{
	float t[16];
	for(int g=0; g<4; g++){
		for(int r=0; r<4; r++){
			t[g*4+r] = a[g*4+0]*b[0][r] + a[g*4+1]*b[1][r] +
						a[g*4+2]*b[2][r] + a[g*4+3]*b[3][r];
		}
	}
	for(int i=0; i<16; i++)
		o[i] = t[i];
}

//行列の合成。[4] = [4] * [4x4]
void Matrixkakeru(float o[4], float a[4], float b[4][4])
{
	float t[4];
	for(int r=0; r<4; r++)
		t[r] = a[0]*b[0][r] + a[1]*b[1][r] + a[2]*b[2][r] + a[3]*b[3][r];
	for(int i=0; i<4; i++)
		o[i] = t[i];
}

//ベクトルの正規化
void NormalizeVec(Vector3 *v)
{
	float a = sqrtf( (v->x)*(v->x) + (v->y)*(v->y) + (v->z)*(v->z) );
	if(a==0) return;
	v->x = v->x / a;
	v->y = v->y / a;
	v->z = v->z / a;
}

Vector3 NormalizeVector(Vector3 v)
{
	NormalizeVec(&v);
	return v;
}

//アニメーション用。AnimateFrame()専用。

//行列をweight倍にして加算。 o[4x4] += a[4x4] * weight
void MatrixTasuW(float o[4][4], float a[4][4], float weight)
{
	for(int g=0; g<4; g++)
		for(int r=0; r<4; r++)
			o[g][r] += a[g][r] * weight;
}

//行列をキーの比率に合わせていろいろして加算。AnimateFrame()専用。
// o[4x4] += ( a[4x4]*s + b[4x4]*(1-s) ) * weight
void MatrixKeyTasuRW(float o[4][4], float a[4][4], float b[4][4], float s, float weight)
{
	for(int g=0; g<4; g++)
		for(int r=0; r<4; r++)
			o[g][r] += ( a[g][r]*s + b[g][r]*(1-s) ) * weight;
}


//表示(バグチェックのため)
void MatrixPrintf(float a[4][4])
{
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++)
			printf("%10f ", a[i][j]);
		printf("\n");
	}
}



