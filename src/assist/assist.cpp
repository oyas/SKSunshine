//assist.cpp
/*-----------------------------------------------------------------------------------*
 *	ゲームで役立つ補助機能たち。
 *-----------------------------------------------------------------------------------*/

#include "assist.h"




//fps測定
const char* fps(bool print)
{
	static int fps=0, t1, t2=0;
	static char txt_fps[20]="";
	
	t1 = glutGet(GLUT_ELAPSED_TIME);
	if(t1 - t2 > 1000) {
		sprintf(txt_fps, "FPS: %g", (1000.0 * fps)/(t1-t2));
		t2 = t1;
		fps = 0;
		if(print)
			printf("%s\n",txt_fps);
	}
	fps++; 
	
	return txt_fps;
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
bool SetKeyState(int *KeyBuf, int key, bool onoff, int *Keys, int size)
{
	//サイズを要素数に変換	
	size /= sizeof(int);

	//サイズが大き過ぎないかチェック
	if( size >= sizeof(int)*8 )
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
bool GetKeyState(int *KeyBuf, int key, int *Keys, int size)
{
	//サイズが大き過ぎないかチェック
	if( size >= sizeof(int)*8 )
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

//キー入力管理クラス
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


/*-----------------------------------------------------------------------------------*
	画像表示
	PNG画像を読み込んで、ディスプレイリストを作る
 *-----------------------------------------------------------------------------------
//filename  : 読み込むPNG画像のファイル名
//texture   : 作成したテクスチャのIDを格納する変数
//pos[4][3] : 表示位置の指定。(頂点４つ)z座標は0推奨。NULLを指定するとデフォルトの位置になる。
//BlendON   : アルファブレンド有効
GLuint CreatePNGDisplayList(const char *filename, GLuint *texture, float pos[4][3], bool BlendON)//, std::vector<unsigned char> &image)
{
	GLuint DisplayList;
	
	pngInfo info;
	//*texture = pngBind(filename, PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	*texture = pngBind(filename, PNG_BUILDMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);


//PicoPNGによるPNG読み込み
	*texture = loadPNGbind(filename);//, image);

//画像表示ポリゴン作成
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
		float pos_default[4][3] = { {1.0,1.0,0.0}, {1.0,-1.0,0.0}, {-1.0,-1.0,0.0}, {-1.0,1.0,0.0} };
		pos = pos_default;	//ちょっと強引だがデフォルトのposを設定
	}
	//テクスチャマッピング有効
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, *texture);//バインド
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
	
	return DisplayList;	//実態はただの整数なのでそのまま渡す。
}
*/



