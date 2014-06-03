//stage0.cpp

#include "stage0.h"
#include "stage1.h"
#include "stage2.h"

#define NEXT_STAGE Stage1(PublicData)
#define SETUMEI_COUNT 5		//何回点滅したら説明を表示するか

Stage0::Stage0( PublicClass *pd ) : StageClass(pd)
{
	//背景作成
	texture_BACK.load("yama.png");	//背景画像読み込み
	float pos[4][3] = { {2.3,1.3,0.0}, {2.3,-1.3,0.0}, {-2.3,-1.3,0.0}, {-2.3,1.3,0.0} };
	texture_BACK.CreateDisplayList(pos, false);	//ディスプレイリスト作成
	
	//「スペースキーでスタート」作成
	texture_Space.load("space.png");
	texture_Space.CreateDisplayList(NULL, true);
	
	//タイトル画像作成
	texture_TITLE.load("title.png");
	texture_TITLE.CreateDisplayList(NULL, true);
	
	//説明画像作成
	texture_Setumei.load("setumei.png");
	texture_Setumei.CreateDisplayList(NULL, true);
	
	//点滅用タイマー
	timer = 0;
	//説明用カウント
	setumei = 0;
	setu_mes = false;
	
}

void Stage0::Disp()
{
	glCallList( texture_BACK.DisplayList );	//背景描画
	
	//モデル行列初期化	
	glLoadIdentity();

	//2が押されたときStage2へ
	if( PublicData->Key.state['2'] ){
		ChangeStage( new Stage2(PublicData) );
	}

	//スペースキーが押されたとき
	if( PublicData->Key.state[' '] ){
		if( setumei < SETUMEI_COUNT ){
			ChangeStage(new NEXT_STAGE);
		}else if( setumei == SETUMEI_COUNT ){
			//カウントが５で、スペースが押されたら、説明画面へ移行
			setu_mes = true;
		}else{ 
			//カウントが６以上（説明画面がでてから）なら、次のステージへ
			ChangeStage(new NEXT_STAGE);
		}
	}

	//説明表示
	if( setu_mes ){
		glCallList( texture_Setumei.DisplayList );	//説明
		if( !PublicData->Key.state[' '] ){	//スペースを離すまでsetumeiは加算されない
			setumei = SETUMEI_COUNT + 1;
		}
		return;	//説明以外は表示しない
	}
	
	glCallList( texture_TITLE.DisplayList );	//タイトル描画
	
	//「スペースキーでスタート」の点滅表示
	if( timer++ < 30 ){
		glCallList( texture_Space.DisplayList );	//「スペースキーでスタート」
	}else if( timer >= 60 ){
		timer=0;
		if( setumei < SETUMEI_COUNT )
			setumei++;	//説明カウントを増加
	}
}

void Stage0::Disp2D(int Width, int Height)
{
	
}

void Stage0::Input(char event, int key, int x, int y)
{
	
}

Stage0::~Stage0()
{

}
