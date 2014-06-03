//assist.cpp
/*-----------------------------------------------------------------------------------*
 *	ゲームで役立つ補助機能たち。
 *-----------------------------------------------------------------------------------*/

#include "assist.h"




//fps測定
float fps(bool print)
{
	static int frames=0, t1, t2=0;
	static float fps=0.0;
	
	t1 = glutGet(GLUT_ELAPSED_TIME);
	if(t1 - t2 > 1000) {
		fps = (1000.0 * frames)/(t1-t2);
		t2 = t1;
		frames = 0;
		if(print)
			printf("FPS: %g\n", fps);
	}
	frames++; 
	
	return fps;
}


/*-----------------------------------------------------------------------------------*
	デフォルトマテリアルセット
	マテリアルをデフォルト値に戻す。カラーは白がセットされる
 *-----------------------------------------------------------------------------------*/
void setDefaultMaterial(){
	//デフォルトマテリアル
	GLfloat defoma_a[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat defoma_d[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat defoma_i[4] = { 0.0, 0.0, 0.0, 1.0 };
	
	//　基本色
	glColor4f(1.0, 1.0, 1.0, 1.0);

	//　Ambient Color
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, defoma_a);
	
	//　Diffuse Color
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defoma_d);

	//　Specular Color
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defoma_i);

	//　Emissive Color
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, defoma_i);

	//　Shininess
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 0);
}



/*-----------------------------------------------------------------------------------*
	文字描画
	2D専用。描画位置と描画文字列、フォントを指定すると、描いてくれる。
 *-----------------------------------------------------------------------------------*/
void DRAW_STRING(int x, int y, char *string, void *font)
{
	int len, i;
	
	glRasterPos2f(x, y);	//Pos
	len = (int)strlen(string);
	for (i = 0; i < len; i++){
		glutBitmapCharacter(font, string[i]);	//ビットマップフォントを使って文字描画（こいつが文字描画本体）
	}
}


/*-----------------------------------------------------------------------------------*
	キー入力管理クラス
	キーの入力状態をstate[key]に配列として持っておく。
 *-----------------------------------------------------------------------------------*/
//キー状態更新
bool KeyInput::SetKeyState(unsigned char key, bool onoff){
	if( key > 127 )
		return false;

	state[key] = onoff;

	return true;
}

//特殊キー状態更新
bool KeyInput::SetSpecialKeyState(int key, bool onoff){
	for( int i=1; i<sizeof(Keys_SK)/sizeof(Keys_SK[0]); i++){
		if( key == Keys_SK[i] ){
			state[ i + KEY_SK_STARTNUMBER ] = onoff;
			return true;
		}
	}

	return false;
}



