//player.h
/*-----------------------------------------------------------------------------------*
	プレイヤー操作キャラクターに関するクラス
 *-----------------------------------------------------------------------------------*/
#ifndef _PLAYER_H
#define _PLAYER_H


#include "../XMesh/XLoader.h"
#include "../image/PNGtexture.h"

//宣言（定義はassist.hの中。変更予定）
class PublicClass;



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
	操作キャラクター用のクラス
 *-----------------------------------------------------------------------------------*/
class PlayerClass{
	//どせい関連	
	XModel model;		//Xモデルクラス
	Vector3 dash_vec;	//どせいさんの向き-前方向(移動時に使用)
	Vector3 dash_svec;	//どせいさんの向き-横方向(移動時に使用)
	int onface;	//当たり判定があったかどうか(床の上に何秒いたか)
	int spacekey;	//スペースキーが押されてから何秒か
	int backjamp;	//バックジャンプ用 特殊ジャンプ用
	
	//共用クラスへのポインタ
	PublicClass *PublicData;

public:
	XObj dosei;	//Xオブジェクト
	//影用
	ShadowOBJ shadow;

	PlayerClass();
	void SetPublicClass(PublicClass *pd){PublicData = pd;}	//親(PublicClass)から一回だけ呼ばれる

	void ResetPos(float x = 0.0, float y = 10.0, float z = 0.0);	//初期位置へ戻す
	void Move(bool keystate[256]);	//現在のキー状態にしたがって動く
	void addAngle(float rot);	//どせいの向きを変更
	Vector3 UpdateSpeed(float gravity);	//スピードを更新
	void OnFace(bool on);	//着地時の処理
	void Render(Vector3 speed);
};




#endif	// _PLAYER_H
