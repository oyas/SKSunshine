//PNGtexture.h
/*-----------------------------------------------------------------------------------*
 *	PNGの読み込み
 *	picoPNG(LodePNGの軽量版)使用
 *-----------------------------------------------------------------------------------*/

#ifndef _PNGTEXTURE_H
#define _PNGTEXTURE_H


#include <GL/freeglut.h>
#include <vector>
#include <string.h>


/*-----------------------------------------------------------------------------------*
	PNG画像の読み込みクラス
	picoPNG(LodePNGの軽量版)を使用。
	自動でテクスチャまで作ってくれる。
 *-----------------------------------------------------------------------------------*/
class PNGtexture{
private:
	int status;	//0:初期 1:PNG読み込み完了 2:ディスプレイリスト作成完了
	
public:
	std::vector<unsigned char> buffer, image;
	unsigned long w, h;
	GLuint texture;
	GLuint DisplayList;
	
	PNGtexture();
	GLuint load(const char *filename);
	GLuint CreateDisplayList(float pos[4][3], bool BlendON);
	~PNGtexture();
};


#endif	//_PNGTEXTURE_H
