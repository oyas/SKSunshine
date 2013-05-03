//PNGtexture.cpp
/*-----------------------------------------------------------------------------------*
 *	PNGの読み込み
 *	picoPNG(LodePNGの軽量版)使用
 *-----------------------------------------------------------------------------------*/

#include "PNGtexture.h"


//picoPNG読み込み(LodePNGの軽量版)
#include "picopng.cpp"

/*-----------------------------------------------------------------------------------*
	PNG画像の読み込み
	picoPNG(LodePNGの軽量版)を使用。
	自動でテクスチャまで作ってくれる。
 *-----------------------------------------------------------------------------------*/
PNGtexture::PNGtexture()
{
	status = 0;
	texture = 0;
	DisplayList = 0;
	w = 0;
	h = 0;
}

//読み込む
GLuint PNGtexture::load(const char *filename)//, std::vector<unsigned char> &image)
//filename	: 読み込むPNG画像のファイル名
//image		: 読みだしたイメージ用のvector配列
//戻り値	: テクスチャ名
{
	//すでに読み込んでいる場合は何もしない。
	if( status >= 1 ){
		return texture;
	}

	//以下、picopng.cppのサンプル通り
	//load and decode
	loadFile(buffer, filename);
	int error = decodePNG(image, w, h, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size());
	 
	//if there's an error, display it
	if(error != 0){
		std::cout << "画像の読み込みに失敗しました。 picoPNG error: " << error << std::endl;
		return 0;
	}
	
	buffer.clear();
	image.clear();
	Image = new unsigned char[ w * h * 4 ];
	memcpy(Image, &image[0], w * h * 4);
	
	//テクスチャ作成
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す

	//ステータス更新。テクスチャ作成完了
	status = 1;
	
//printf("テクスチャ作成完了 %d %s\n", texture, filename);
	
	
	return texture;
}

//画像表示用ディスプレイリスト作成
GLuint PNGtexture::CreateDisplayList( float pos[4][3], bool BlendON)
//pos[4][3] : 表示位置の指定。(頂点４つ)z座標は0推奨。NULLを指定するとデフォルトの位置になる。
//BlendON   : アルファブレンド有効
{
	//すでに作成している場合は何もしない。
	if( status != 1 ){
		return DisplayList;
	}


	DisplayList = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList, GL_COMPILE); //コンパイルのみ
	
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);//いろいろ無効にする
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	if(BlendON == true) glEnable(GL_BLEND); //GL_ALPHA_TEST);// アルファテスト有効
	glColor3f(1.0, 1.0, 1.0);
	//カメラ
	gluLookAt(0, 0, -3.0, 0,0,0, 0.0,1.0,0.0);
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	//pos
	if(pos == NULL){
		static float pos_default[4][3] = { {1.0,1.0,0.0}, {1.0,-1.0,0.0}, {-1.0,-1.0,0.0}, {-1.0,1.0,0.0} };
		pos = pos_default;	//ちょっと強引だがデフォルトのposを設定
	}
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);//バインド
	glBegin(GL_QUADS);
		for(int i=0; i<4; i++){
			glTexCoord2fv( uv[i] );
			glVertex3fv( pos[ i ] );
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//元に戻す
	glDisable(GL_TEXTURE_2D);
	if(BlendON == true) glDisable(GL_BLEND); //GL_ALPHA_TEST);// アルファテスト無効
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEndList();	//ディスプレイリストおわり

	//ディスプレイリスト作成完了。
	status = 2;

	return DisplayList;	//実態はただの整数なのでそのまま渡す。
}


PNGtexture::~PNGtexture()
{
//	printf("ですと%d\n", texture);
	glDeleteLists(DisplayList, 1);
	glDeleteTextures(1, &texture);
}

