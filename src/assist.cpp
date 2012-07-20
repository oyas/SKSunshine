//assist.cpp
/*-----------------------------------------------------------------------------------*
 *	OpenGLテンプレート（ゲームプログラミング用）
 *	ゲームで役立つ補助機能たち。
 *	ファイル名を「subMain.cpp」→「assist.cpp」と変更した。
 *	2011/6
 *-----------------------------------------------------------------------------------*/

#include "assist.h"


//fps測定
char* fps(void)
{
	static int fps=0, t1, t2=0;
	static char txt_fps[20]="";
	
	t1 = glutGet(GLUT_ELAPSED_TIME);
	if(t1 - t2 > 1000) {
		sprintf(txt_fps, "FPS: %g", (1000.0 * fps)/(t1-t2));
		t2 = t1;
		fps = 0;
	}
	fps++; 
	
	return txt_fps;
}


/*-----------------------------------------------------------------------------------*
	床クラス
	床を表示&管理する
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
Ita::Ita(float u, float v){
	pos.x=0.0; pos.y=0.0; pos.z=0.0;
	size.x = 0.0;
	size.y = 0.0;
	SetRender(u, v);
}

//セット。ディスプレイリストを作成する
//１２分割の白黒の板をつくる
void Ita::SetRender(float u, float v){
	const static GLfloat ground[][4] = {   /* 台の色　　　 */
		{ 0.6, 0.6, 0.6, 1.0 },
		{ 1.0, 1.0, 1.0, 1.0 }
	};
	
	DisplayList = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList,GL_COMPILE); //コンパイルのみ
	
	//マテリアルをデフォルトにもどす。
	setDefaultMaterial();
	
	glBegin(GL_QUADS);
	glNormal3d(0.0, 1.0, 0.0);
	for (int j = -6; j < 6; ++j) {
		for (int i = -6; i < 6; ++i) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
			glVertex3f((GLfloat)i*(u/12.0), 0.0, (GLfloat)j*(v/12.0));
			glVertex3f((GLfloat)i*(u/12.0), 0.0, (GLfloat)(j + 1)*(v/12.0));
			glVertex3f((GLfloat)(i + 1)*(u/12.0), 0.0, (GLfloat)(j + 1)*(v/12.0));
			glVertex3f((GLfloat)(i + 1)*(u/12.0), 0.0, (GLfloat)j*(v/12.0));
		}
	}
	glEnd();
	glEndList();	//ディスプレイリストおわり
	
	size.x = u; size.y = v;
	//頂点位置保存
	vertex[0].x = -u/2; vertex[0].y = 0.0; vertex[0].z = -v/2;
	vertex[1].x =  u/2; vertex[1].y = 0.0; vertex[1].z = -v/2;
	vertex[2].x =  u/2; vertex[2].y = 0.0; vertex[2].z =  v/2;
	vertex[3].x = -u/2; vertex[3].y = 0.0; vertex[3].z =  v/2;
}

//描画
void Ita::Render(){
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	glCallList(DisplayList);	//描画
	glPopMatrix();
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
	キー入力補助
	KeyBufのKeysと対応するビットを変更する。
 *-----------------------------------------------------------------------------------*/
//KeyBufの状態を変更する。
bool SetKeyState(int *KeyBuf, int key, bool onoff, int *Keys, char size)
{
	//サイズが大き過ぎないかチェック
	if( size >= (char)sizeof(*KeyBuf)*8 )
		return false;
	
	//サイズ分処理
	for(int i=0; i<size; i++){
		if( key == Keys[i] ){
			int k=1;
			if( onoff )
				*KeyBuf |= k << i;	//ビットON
			else
				*KeyBuf &= ~( k << i );	//ビットOFF
		}
	}
	
	return true;
}

//状態を取得。押されていたらtrue
bool GetKeyState(int *KeyBuf, int key, int *Keys, char size)
{
	//サイズが大き過ぎないかチェック
	if( size >= (char)sizeof(*KeyBuf)*8 )
		return false;
	
	//サイズ分処理
	for(int i=0; i<size; i++){
		if( key == Keys[i] ){
			int k=1;
			return *KeyBuf & (k<<i);
		}
	}
	
	return true;
}




