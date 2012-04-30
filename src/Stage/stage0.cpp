//stage0.cpp

#include "stage0.h"
#include "stage1.h"


Stage0::Stage0()
{
	//////////////////////////
	//背景作成
	//背景画像読み込み
	{
	pngInfo info;
	textureBACK = pngBind("yama.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	DisplayList_BACK = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList_BACK,GL_COMPILE); //コンパイルのみ
	
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);//いろいろ無効にする
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glColor3f(1.0, 1.0, 1.0);
	//カメラ
	gluLookAt(0, 0, -2.4, 0,0,0, 0.0,1.0,0.0);
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	//pos
	float pos[4][3] = { {1.8,1.0,0.0}, {1.8,-1.0,0.0}, {-1.8,-1.0,0.0}, {-1.8,1.0,0.0} };
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureBACK);//バインド
	glBegin(GL_QUADS);
		for(int i=0; i<4; i++){
			glTexCoord2fv( uv[i] );
			glVertex3fv( pos[ i ] );
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEndList();	//ディスプレイリストおわり
	}
	//背景ここまで
	//////////////////////////////
	
	//////////////////////////
	//「スペースキーでスタート」作成
	//画像読み込み
	{
	pngInfo info;
	textureSpace = pngBind("space.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	//アルファテスト用
	glAlphaFunc(GL_GEQUAL, 0.5);
	
	DisplayList_Space = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList_Space,GL_COMPILE); //コンパイルのみ
	
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);//いろいろ無効にする
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_ALPHA_TEST);// アルファテスト有効
	
	glColor3f(1.0, 1.0, 1.0);
	//カメラ
	gluLookAt(0, 0, -3.0, 0,0,0, 0.0,1.0,0.0);
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	//pos
	float pos[4][3] = { {1.0,1.0,0.0}, {1.0,-1.0,0.0}, {-1.0,-1.0,0.0}, {-1.0,1.0,0.0} };
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureSpace);//バインド
	glBegin(GL_QUADS);
		for(int i=0; i<4; i++){
			glTexCoord2fv( uv[i] );
			glVertex3fv( pos[ i ] );
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);// アルファテスト無効
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEndList();	//ディスプレイリストおわり
	}
	//ここまで
	//////////////////////////////
	
	//////////////////////////
	//タイトル画像作成
	//画像読み込み
	{
	pngInfo info;
	textureTITLE = pngBind("title.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	//アルファテスト用
	glAlphaFunc(GL_GEQUAL, 0.5);
	
	DisplayList_TITLE = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList_TITLE,GL_COMPILE); //コンパイルのみ
	
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);//いろいろ無効にする
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_ALPHA_TEST);// アルファテスト有効
	
	glColor3f(1.0, 1.0, 1.0);
	//カメラ
	gluLookAt(0, 0, -3.0, 0,0,0, 0.0,1.0,0.0);
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	//pos
	float pos[4][3] = { {1.0,1.0,0.0}, {1.0,-1.0,0.0}, {-1.0,-1.0,0.0}, {-1.0,1.0,0.0} };
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureTITLE);//バインド
	glBegin(GL_QUADS);
		for(int i=0; i<4; i++){
			glTexCoord2fv( uv[i] );
			glVertex3fv( pos[ i ] );
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);// アルファテスト無効
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEndList();	//ディスプレイリストおわり
	}
	//ここまで
	//////////////////////////////
	
	//////////////////////////
	//説明画像作成
	//画像読み込み
	{
	pngInfo info;
	textureSetumei = pngBind("setumei.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	//アルファテスト用
	glAlphaFunc(GL_GEQUAL, 0.5);
	
	DisplayList_Setumei = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList_Setumei,GL_COMPILE); //コンパイルのみ
	
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);//いろいろ無効にする
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_ALPHA_TEST);// アルファテスト有効
	
	glColor3f(1.0, 1.0, 1.0);
	//カメラ
	gluLookAt(0, 0, -3.0, 0,0,0, 0.0,1.0,0.0);
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	//pos
	float pos[4][3] = { {1.0,1.0,0.0}, {1.0,-1.0,0.0}, {-1.0,-1.0,0.0}, {-1.0,1.0,0.0} };
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureSetumei);//バインド
	glBegin(GL_QUADS);
		for(int i=0; i<4; i++){
			glTexCoord2fv( uv[i] );
			glVertex3fv( pos[ i ] );
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);// アルファテスト無効
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEndList();	//ディスプレイリストおわり
	}
	//ここまで
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
	
	if( setumei>=5 && space){
		setu_mes = true;
	}
	if( setu_mes ){
		glCallList(DisplayList_Setumei);
		if(space){
			if(setumei>5) ChangeStage(new Stage1);
		}else{
			setumei++;
		}
		return;
	}
	
	//glCallList(DisplayList_TITLE);	//タイトル描画
	
	if( timer++ < 30 ){
		glCallList(DisplayList_Space);	//「スペースキーでスタート」
	}else if(timer>=60){
		timer=0;
		//if(setumei<5) setumei++;	//説明
	}
}

void Stage0::Disp2D()
{
}

void Stage0::Input(char event, int key, int x, int y)
{
	switch(event){
		case SC_INPUT_KEY_DOWN:	//キーが押されたとき
			if( key == ' ' ){	//スペースキーが押されたとき
				if(setumei < 5){
					ChangeStage(new Stage1);
				}else{
					space = true;
				}
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
