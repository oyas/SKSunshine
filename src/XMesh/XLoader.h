// XLoader.h
/*-----------------------------------------------------------------------------------*
	* ワンスキンメッシュのXモデルのみ読み込み可能。
	* 7での変更点:
	* VBOに対応。マテリアルによって分けるサブセットの作成。meshをmodelにまとめた。テクスチャ(PNGのみ)対応。
	* Vertex3は名前が変わり、operatorもついた
 *-----------------------------------------------------------------------------------*/

#ifndef _X_LOADER_H_INCLUDED_
#define _X_LOADER_H_INCLUDED_


//
// include
//

//GL
#ifdef WIN32
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#else
#  define GL_GLEXT_PROTOTYPES	//linuxではこれを宣言するとglewが使えるぽい
#  include <GL/freeglut.h>
#endif
#include <GL/glpng.h>	//pngのローダー

#include <iostream>
#include <cmath>
#include <vector>

#include <cstring>
#include <cstdio>
#include <fstream>
//windows or other
#ifdef WIN32
	#include <windows.h>
#else
	#define ZeroMemory(x,y) memset(x,0x00,y)
#endif

using namespace std;

//
// define
//
//#ifndef _SAFE_DELETE
//#define SafeDelete(x) { if (x) { delete x; x = NULL; } }
//#endif
//#ifndef _SAFE_DELETEARRAY
//#define SafeDeleteArray(x) { if (x) { delete [] x; x = NULL; } }
//#endif

//
// forward declarations
//
class XFace;
class XMesh;
class XMaterial;
class XModel;
class XAnimation;
class XAnimationKey;
struct Vector2;
struct Vector3;
struct MATRIX4x4;

//colli.cppでも使うため、プロトタイプ宣言
void NormalizeVec(Vector3 *v);	//ベクトル正規化
Vector3 NormalizeVector(Vector3 v);	//返り値がベクトル
void MatrixIdentity(float f[4][4]);	//行列の初期化
void Matrixkakeru(float o[4][4], float a[4][4], float b[4][4]); //行列の掛け算
void Matrixkakeru(float o[4], float a[4], float b[4][4]);	//ベクトルの掛け算
void Matrixkakeru(GLdouble o[16], GLdouble a[16], float b[4][4]); //行列の掛け算(XLoaderでは使わない)


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
};

//
// XColor struct
//
struct XColor 
{
	float r, g, b, a; 
	operator float*() { return (float*)&r; }
	operator const float*() const { return (const float*)&r; }
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


//
// オリジナル頂点バッファ (一つの頂点に影響を与えるボーンは４つまで)
//
struct ORIGINAL_VERTEX {
public:
	Vector3 pos;	//頂点座標
	//以下２つの配列は頂点を読み込む際にいっしょに初期化される
	float weight[4];	//ボーンウェイト
	unsigned char index[4];	//影響を受けるボーン番号
	Vector3 normal;	//法線
	Vector2 uv;	//UV座標
};

//
//アニメーション頂点バッファ。並びは、T2F_N3F_V3F
//
struct ANIMATED_VERTEX {
	Vector2 uv;		//T uv座標
	Vector3 normal;	//N 法線
	Vector3 pos;		//V 頂点
};

//
// サブセット
// indexの面から、count個の面はずっと同じmaterialである。
// マテリアルが変わるごとにサブセットを一つ用意する。
//
struct SUBSET{
	int material;	//マテリアル番号
	int index;		//開始面番号
	int count;		//面数
};



//
// XFace class
// 面データを保持する
// このクラスは１つの面のデータを管理する
// つまり面ごとにインスタンスを作成する必要がある
//
class XFace
{
public:
	GLuint indexVertices[3];	//頂点のインデックス。ちなみに三角形のみ対応
	XFace();	//コンストラクタ　データの初期化
};

//
// XMaterial class
// マテリアル
//
class XMaterial
{
public :
	char *name;	//マテリアル名
	XColor ambient;		//環境光
	XColor diffuse;		//拡散光
	XColor specular;	//鏡面光
	XColor emissive;	//放射輝度
	float power;		//鏡面光の指数
	char *textureFileName;	//テクスチャがある場合、テクスチャファイル名
	GLuint texture;		//テクスチャ(現在はPNGのみ読み込み可能)
	XMaterial();
	~XMaterial();
};

//
// XMesh class
// メッシュ
// メッシュデータ本体。頂点・法線・UV座標・面　を持つ
//
//class XMesh
//{
//public :
//	char *name;
//	XMesh();
//	~XMesh();
//};

//
// Frameクラス
// フレームごとにインスタンスが必要。
//
class XFrame
{
public:
	float TransformMatrix[4][4];	//変換行列 アニメーションによって変化する。(Frameで読み込み)	逆BOf行列。アニメーション適用後は( 変換行列*逆BOf行列 )
	float OffsetMatrix[4][4];	//オフセット行列 頂点座標をフレームの座標系に変換するための行列(SkinWeightsで読み込み)	BOf行列
	float CombinedMatrix[4][4];	//合成行列 ルートフレームまでの変換行列を合成した行列	変換行列*逆BOf行列 * (親の変換行列*親の逆BOf行列) * (…
	float SkinningMatrix[4][4];	//スキニング行列	これを使ってアニメーション適用後の頂点座標を求める BOf行列*合成行列
	char *name;	//フレーム名
	vector<XFrame*> child;	//子フレームのvector配列
	int index;	//フレーム番号
	XFrame(XModel *model);	//コンストラクタ。フレームの読み込みがされる。引数は、XModelの関数を呼ぶために必要。
	void Animate(float parent[4][4]);	//スキニング行列の計算。XModel::AnimateFrame()から呼ばれる。
	~XFrame();
};

//
// XAnimationKeyクラス
// １つのアニメーションキーの情報をもつ。ロード後で変更されることはない
//
class XAnimationKey{
public:
	vector<float> time;	//時間の配列。キー分だけある。
	vector<MATRIX4x4*> matrix;	//行列の配列。キー分だけある。
	char *frameName;	//フレームの名前
	int frameIndex;		//フレーム番号
	
	XAnimationKey();
	~XAnimationKey();
};

//
// XAnimaton class
// １つのアニメーションを管理する。アニメーションキーへの配列を持つ。
//
class XAnimation{
public:
	char *name;		//アニメーション名
	vector<XAnimationKey*> key;	//キークラスへの配列
	float time;		//現在の時間。時間をすすめてやることでアニメーションする。XModel::Animate()とかで変更される
	float weight;		//ウェイト。アニメーション同士を合成するとき以外は1.0。
	
	float GetMaxTime(){ return key.front()->time.back(); }		//アニメーションの長さを返す
	XAnimation();
	~XAnimation();
};

//
// XModel class
// Xモデル
// このクラスのインスタンス１つで１つのXモデルを管理する
// メッシュ・マテリアル　を持つ。スキンメッシュのみ対応。
// Load()で読み込み、Render()で3D空間に実体を作る(配置する、レンダリングする)
//
class XModel
{
private:
	//GLuint DisplayList;	//ディスプレイリスト
	GLuint vertexBuffer, indexBuffer;			//バッファオブジェクトの名前。VBO
	bool Load_AnimationSet();	//アニメーションセットの読み込み（Load_AnimOnly()で使うため、Load_element()から独立）
	bool vbo;	//VBOをしようするかどうか
	int numOFaces;	//Xファイルに記されていた面数
	bool *OFaceFour;	//その番号の面が四角形だったかどうか(分割したはじめの方のみtrue)
	vector<char*> SkinWeightsPointer;	//SkinWeights用vector。スキンウェイトがあったら、記憶
	char *filedir;	//Xファイルのあるディレクトリ
	bool Load_SkinWeights();	//SkinWeightsの読み込み。読み込み失敗防止のため独立。AnimationSetの手前で呼ばれる。

public:
	int numMaterials;	//マテリアル数
	int numVertices;	//頂点数
	int numFaces;	//面数(４角形面を分割した分も含む)
	bool bTexture;	//テクスチャを読み込んだかどうか
	ORIGINAL_VERTEX *oVertex;	//オリジナルの頂点　配列
	ANIMATED_VERTEX *aVertex;	//アニメーション適用後の頂点　配列
	XFace *face;		//面(頂点,マテリアルのインデックスの指定で表現。）　配列
	int *indexMaterials;	//マテリアル番号(面に対応するマテリアルの番号。面数分確保される)
	XMaterial *material;		//マテリアル（反射率・テクスチャ等）
	vector<XFrame*> Frame;	//フレームの配列(モデルの全フレームへのポインタを格納)
	vector<XAnimation*> Animation;	//全アニメーションへのポインタの配列
	vector<SUBSET> subset;	//サブセット。
	int loaded;		//読み込んだ要素 1のビットが立っているときはエラー。2以降は、load_element()に書いてある
	
	//ロード関係
	XFrame* FindFrame(const char* name);	//指定した名前のフレームを探す
	bool Load(char *filename, bool AnimOnly);	//Xファイルをロードする。内部で↓Load_element()を呼び出している。
	bool Load_element();		//要素を識別して、変数へ格納する。 ←こいつが一番がんばってる
	
	//描画関係
	void Render(float scale=1.0f);	//メッシュをレンダリングする
	//ディスプレイリストを使用した描画
	//void SetDListRender(float scale=1.0f);	//ディスプレイリストを作る。Load()内で呼ばれる
	//void DListRender();	//ディスプレイリストですべてのMeshをレンダリングする。
	//VBOを使用した描画
	void SetVBORender();	//VBOが使えるように用意する。
	void VBORender();		//VBOレンダリング
	void BindVertexBuffer();	//vertexBufferをバインドしてあげるだけ。
	bool VBO(){return vbo;}	//vbo　(VBOを使用してるかどうか)を返す。
	
	//コンストラクタとデストラクタ
	XModel();
	~XModel();
	
	//アニメーション関係
	void AnimateVertex();	//フレーム(ボーン)から頂点位置計算
	void AnimateFrame();	//現在のアニメーションの時間に合わせてフレームを変化させる
	bool Animate(int anim, float speed);	//アニメーションさせる。アニメーションが終わってたら、trueが返る。
	void ResetAnimate(int anim);		//アニメーション時間を0に戻す
};


/*-----------------------------------------------------------------------------------*
	Xオブジェクト
	Xファイルから読み込み、管理
 *-----------------------------------------------------------------------------------*/
class XObj{
public:
	XModel *model;
	Vector3 pos;	//位置
	float angle;	//回転角度
	Vector3 ang;	//回転
	Vector3 speed;	//速度(当たり判定で使用)
	Vector3 force;	//力、摩擦の時に使う
	Vector3 offset;	//オフセット、原点をきめるのに使う
	
	XObj();
	~XObj();
	void setXModel(XModel *m);	//Xモデルの登録
	void Render();	//描画
};



#endif	//　_X_LOADER_H_INCLUDED_
