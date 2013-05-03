//stage0.cpp

#include "stage0.h"
#include "stage1.h"
#include "stage2.h"

#define NEXT_STAGE Stage1

Stage0::Stage0()
{
	//////////////////////////
	//背景作成
	//背景画像読み込み
	float pos[4][3] = { {2.3,1.3,0.0}, {2.3,-1.3,0.0}, {-2.3,-1.3,0.0}, {-2.3,1.3,0.0} };
//	DisplayList_BACK = CreatePNGDisplayList("yama.png", &textureBACK, pos, false);
	texture_BACK.load("yama.png");
	DisplayList_BACK = texture_BACK.CreateDisplayList(pos, false);
	//////////////////////////////
	
	//////////////////////////
	//「スペースキーでスタート」作成
	//画像読み込み
//	DisplayList_Space = CreatePNGDisplayList("space.png", &textureSpace, NULL, true);
	texture_Space.load("space.png");
	DisplayList_Space = texture_Space.CreateDisplayList(NULL, true);
	//////////////////////////////
	
	//////////////////////////
	//タイトル画像作成
	//画像読み込み
//	DisplayList_TITLE = CreatePNGDisplayList("title.png", &textureTITLE, NULL, true);
	texture_TITLE.load("title.png");
	DisplayList_TITLE = texture_TITLE.CreateDisplayList(NULL, true);
	//////////////////////////////
	
	//////////////////////////
	//説明画像作成
	//画像読み込み
//	DisplayList_Setumei = CreatePNGDisplayList("setumei.png", &textureSetumei, NULL, true);
	texture_Setumei.load("setumei.png");
	DisplayList_Setumei = texture_Setumei.CreateDisplayList(NULL, true);
	//////////////////////////////
	
	//点滅用タイマー
	timer = 0;
	//説明用カウント
	setumei = 0;
	space = false;
	setu_mes = false;
	
}

void Stage0::Disp()
{
	glCallList(DisplayList_BACK);	//背景描画
	
	//モデル行列初期化	
	glLoadIdentity();
	
	//カウントが５以上で、スペースが押されたら、説明画面へ移行
	if( setumei >= 5 && space){
		setu_mes = true;
	}
	//説明表示
	if( setu_mes ){
		glCallList(DisplayList_Setumei);	//説明
		if(space){
			if(setumei>5) ChangeStage(new NEXT_STAGE);
		}else{
			setumei++;
		}
		return;
	}
	
	glCallList(DisplayList_TITLE);	//タイトル描画
	
	//「スペースキーでスタート」の点滅表示
	if( timer++ < 30 ){
		glCallList(DisplayList_Space);	//「スペースキーでスタート」
	}else if(timer>=60){
		timer=0;
		if(setumei<5) setumei++;	//説明カウントを増加
	}
}

void Stage0::Disp2D(int Width, int Height)
{
	
}

void Stage0::Input(char event, int key, int x, int y)
{
	switch(event){
		case SC_INPUT_KEY_DOWN:	//キーが押されたとき
			if( key == ' ' ){	//スペースキーが押されたとき
				if(setumei < 5){
					ChangeStage(new NEXT_STAGE);
				}else{
					space = true;
				}
			}
			if( key == '2' ){	//2が押されたときStage2へ
				ChangeStage(new Stage2);
			}
			break;
		case SC_INPUT_KEY_UP:	//キーが押されたとき
			if( key == ' ' ){	//スペースキーが押されたとき
				space = false;
			}
			break;
	}
	
}

Stage0::~Stage0()
{
	glDeleteTextures(1, &textureBACK);
	glDeleteLists(DisplayList_BACK, 1);
	glDeleteTextures(1, &textureSpace);
	glDeleteLists(DisplayList_Space, 1);
	glDeleteTextures(1, &textureTITLE);
	glDeleteLists(DisplayList_TITLE, 1);
	glDeleteTextures(1, &textureSetumei);
	glDeleteLists(DisplayList_Setumei, 1);
}
