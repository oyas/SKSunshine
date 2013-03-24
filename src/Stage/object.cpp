// object.cpp

#include "object.h"


////////////////////////////////////

/*-----------------------------------------------------------------------------------*
	三角形オブジェクト
	当たり判定を含む
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
sankakOBJ::sankakOBJ(){
	for(int i=0; i<3; i++){
		oVertex[i].x=0.0; oVertex[i].y=0.0; oVertex[i].z=0.0;
		vertex[i].x=0.0; vertex[i].y=0.0; vertex[i].z=0.0;
	}
	pos = 0.0;
	DisplayList = 0;
	max_y = 0.0;
	laston = 0;
}
//三角形登録
void sankakOBJ::Set(Vector3 *vec)
{
	//コピー
	for(int i=0; i<3; i++){
		oVertex[i] = vec[i];
		vertex[i] = vec[i];
	}
	//当たり判定登録
	colliOBJ.Set_3(vertex);
	
	//テクスチャ読み込み
	pngInfo info;
	texturePNG = pngBind("test.png", PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	//UV
	float uv[3][2] = { {0.0,0.0}, {1.0,0.0}, {0.0,1.0} };
	
	setDefaultMaterial();
	//ディスプレイリストを作成
	DisplayList = glGenLists(1);
	glNewList(DisplayList,GL_COMPILE); //コンパイルのみ
	
	glBindTexture(GL_TEXTURE_2D, texturePNG);
	glNormal3fv( (GLfloat*)&(colliOBJ.GetNormal().x) );
	glBegin(GL_TRIANGLES);
	for(int i=0; i<3; i++){
		glTexCoord2fv( uv[i] );
		glVertex3fv( vertex[i] );
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	
	glEndList();	//ディスプレイリストおわり
}

//回転させる
void sankakOBJ::rot(MATRIX4x4 &m)
{
	
}

//追加回転させる
void sankakOBJ::addrot(MATRIX4x4 &m)
{
	float v[4];
	for(int i=0; i<3; i++){
		v[0]=vertex[i].x; v[1]=vertex[i].y; v[2]=vertex[i].z; v[3]=1.0;
		Matrixkakeru(v, v, m.m);
		vertex[i].x=v[0]; vertex[i].y=v[1]; vertex[i].z=v[2];
	}
	//当たり判定登録
	colliOBJ.Set_3(vertex);
	//回転した
	laston |= 2;
	//行列保存
	mat = m;
}

//垂直抗力の取得と適用
void sankakOBJ::NormalForce(Vector3 &_pos, Vector3 &_speed)
{
	//float y = 0.0;
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z }, nvec;	
	
	if( laston==3 ){	//前回乗ってて回転したとき
		float vm[4];
		vm[0]=v.x; vm[1]=v.y; vm[2]=v.z; vm[3]=1.0;
		Matrixkakeru(vm, vm, mat.m);
		_pos.x += vm[0]-v.x; _pos.y += vm[1]-v.y; _pos.z += vm[2]-v.z;
		v.x=vm[0]; v.y=vm[1]; v.z=vm[2];
		laston=0;
	}
	
	/*if( laston && v.y<max_y ){
		Vector3 v2 = v, speed2 = _speed;
		//上へ上げる
		y = max_y-v.y +0.001;
		v2.y +=  y;
		speed2.y -= y;
		nvec = colliOBJ.GetNormalForce( v2, speed2);	//垂直抗力計算
		if( nvec.x==0.0 && nvec.y==0.0 && nvec.z==0.0 ){
			laston = false;
		}else{
			Vector3 n = colliOBJ.GetNormal();
			v2 += speed2 + nvec + n*0.01;
			_speed = v2-v;
			//laston = true;
		}
	}else{*/
		nvec = colliOBJ.GetNormalForce( v, _speed);	//垂直抗力計算
		if( nvec.x==0.0 && nvec.y==0.0 && nvec.z==0.0 ){
			laston = 0;
		}else{
			_speed += nvec;
			laston |= 1;
		}
	//}
}

//描画
void sankakOBJ::Render()
{
	//UV
	float uv[3][2] = { {0.0,0.0}, {1.0,0.0}, {0.0,1.0} };
	
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	setDefaultMaterial();
	
	glBindTexture(GL_TEXTURE_2D, texturePNG);
	glNormal3fv( (GLfloat*)&(colliOBJ.GetNormal().x) );
	glBegin(GL_TRIANGLES);
	for(int i=0; i<3; i++){
		glTexCoord2fv( uv[i] );
		glVertex3fv( vertex[i] );
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	//glCallList(DisplayList);	//描画
	glPopMatrix();
}

//デストラクタ
sankakOBJ::~sankakOBJ(){
	glDeleteTextures(1, &texturePNG);
}




/*-----------------------------------------------------------------------------------*
	直方体オブジェクト
	当たり判定を含む
 *-----------------------------------------------------------------------------------*/
const int boxOBJ::index[6][4] = { {0, 1, 2, 3}, 
						{4, 5, 1, 0},
						{5, 6, 2, 1},
						{6, 7, 3, 2},
						{7, 4, 0, 3},
						{7, 6, 5, 4} };
//コンストラクタ
boxOBJ::boxOBJ() {
	for(int i=0; i<8; i++){
		oVertex[i].x=0.0; oVertex[i].y=0.0; oVertex[i].z=0.0;
		vertex[i].x=0.0; vertex[i].y=0.0; vertex[i].z=0.0;
	}
	DisplayList = 0;
	texturePNG = 0;
	laston = 0;
	laston_i = 0;
	r = 0.0;
	MatrixIdentity(mat.m);
	//行列初期化
	for(int i=0; i<16; i++){
		if(i%5 == 0){
			Matrix[i] = 1.0;
		}else{
			Matrix[i] = 0.0;
		}
	}
	move = 0.0;
	isSet = false;
	pos = 0.0;
}
//登録
void boxOBJ::Set(Vector3 *vec, const char *texName)
{
	//すでに呼ばれたかどうか
	if(isSet) return;
	isSet = true;
	
	//コピー&頂点位置決定
	oVertex[0].x = fabs(vec->x);
	oVertex[0].y = fabs(vec->y);
	oVertex[0].z = fabs(vec->z);
	oVertex[1].x = -oVertex[0].x; oVertex[1].y =  oVertex[0].y; oVertex[1].z =  oVertex[0].z; 
	oVertex[2].x = -oVertex[0].x; oVertex[2].y = -oVertex[0].y; oVertex[2].z =  oVertex[0].z; 
	oVertex[3].x =  oVertex[0].x; oVertex[3].y = -oVertex[0].y; oVertex[3].z =  oVertex[0].z; 
	oVertex[4].x =  oVertex[0].x; oVertex[4].y =  oVertex[0].y; oVertex[4].z = -oVertex[0].z; 
	oVertex[5].x = -oVertex[0].x; oVertex[5].y =  oVertex[0].y; oVertex[5].z = -oVertex[0].z; 
	oVertex[6].x = -oVertex[0].x; oVertex[6].y = -oVertex[0].y; oVertex[6].z = -oVertex[0].z; 
	oVertex[7].x =  oVertex[0].x; oVertex[7].y = -oVertex[0].y; oVertex[7].z = -oVertex[0].z; 
	for(int i=0; i<8; i++){
		vertex[i] = oVertex[i];
	}
	
	//当たり判定登録
	Vector3 v[4];
	for(int i=0; i<6; i++){
		v[0]=vertex[ index[i][0] ];
		v[1]=vertex[ index[i][1] ];
		v[2]=vertex[ index[i][2] ];
		v[3]=vertex[ index[i][3] ];
		colli[i].Set_4(v);
	}
	
	//判定球の半径測定
	r = sqrt( vec->x*vec->x + vec->y*vec->y + vec->z*vec->z );
	
	//テクスチャ読み込み
	if( texName != NULL ){
		pngInfo info;
		texturePNG = pngBind(texName, PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}
	
	//UV
	float uv[4][2] = { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} };
	
	//ディスプレイリスト
	DisplayList = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList,GL_COMPILE); //コンパイルのみ
	
	if(texturePNG) glBindTexture(GL_TEXTURE_2D, texturePNG);
	
	setDefaultMaterial();
	
	glBegin(GL_QUADS);
	for(int f=0; f<6; f++){
		glNormal3fv( (GLfloat*)&(colli[f].GetNormal()) );
		for(int i=0; i<4; i++){
			if(texturePNG) glTexCoord2fv( uv[i] );
			glVertex3fv( oVertex[ index[f][i] ] );
		}
	}
	glEnd();
	
	if(texturePNG) glBindTexture(GL_TEXTURE_2D, 0);//バインドしたのをもとに戻す
	
	glEndList();	//ディスプレイリストおわり
}

//回転させる
void boxOBJ::rotOriginal()
{
	float v[4];
	for(int i=0; i<8; i++){
		vertex[i]=oVertex[i];
	}
	//当たり判定登録
	Vector3 vec[4];
	for(int i=0; i<6; i++){
		vec[0]=vertex[ index[i][0] ];
		vec[1]=vertex[ index[i][1] ];
		vec[2]=vertex[ index[i][2] ];
		vec[3]=vertex[ index[i][3] ];
		colli[i].Set_4(vec);
	}
}

//追加回転させる
void boxOBJ::addrot(MATRIX4x4 &m)
{
	float v[4];
	for(int i=0; i<8; i++){
		v[0]=vertex[i].x; v[1]=vertex[i].y; v[2]=vertex[i].z; v[3]=1.0;
		Matrixkakeru(v, v, m.m);
		vertex[i].x=v[0]; vertex[i].y=v[1]; vertex[i].z=v[2];
	}
	//当たり判定登録
	Vector3 vec[4];
	for(int i=0; i<6; i++){
		vec[0]=vertex[ index[i][0] ];
		vec[1]=vertex[ index[i][1] ];
		vec[2]=vertex[ index[i][2] ];
		vec[3]=vertex[ index[i][3] ];
		colli[i].Set_4(vec);
	}
	//回転した
	laston |= 2;
	//行列保存
	mat = m;
	Matrixkakeru(Matrix, Matrix, m.m);
}

//追加移動させる
void boxOBJ::addpos(Vector3 &vec){
	move = vec;
	pos += vec;
}

//垂直抗力の取得と適用
void boxOBJ::NormalForce(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z }, nvec;	
	
	//移動したとき
	if( move.x!=0.0 || move.y!=0.0 || move.z!=0.0 ){
		//if(laston & 1){
			//_pos += move;
			v += move;
			//_speed += move;
		//}
	}
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > r+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		laston =0;
		move = 0.0;
		return;
	}
	
	
	
	if( laston&2 ){	//前回乗ってて回転したとき
		float vm[4];
		vm[0]=v.x; vm[1]=v.y; vm[2]=v.z; vm[3]=1.0;
		Matrixkakeru(vm, vm, mat.m);
		_pos.x += vm[0]-v.x; _pos.y += vm[1]-v.y; _pos.z += vm[2]-v.z;	//回転に合わせて移動
		//_speed.x -= vm[0]-v.x; _speed.y -= vm[1]-v.y; _speed.z -= vm[2]-v.z;	//回転に合わせて移動
		v.x=vm[0]; v.y=vm[1]; v.z=vm[2];
	}
	
	laston_i--; laston=0;
	for(int i=0; i<6; i++){
		nvec = colli[i].GetNormalForce( v, _speed);	//垂直抗力計算
		if( nvec.x==0.0 && nvec.y==0.0 && nvec.z==0.0 ){
		}else{
			laston_i=2;
			_speed += nvec;
		}
	}
	if(laston_i>0){
		laston |= 1;
	}
	
	//移動したとき
	if( move.x!=0.0 || move.y!=0.0 || move.z!=0.0 ){
		if(laston & 1){
			//_pos += move;
			//v += move;
			_speed += move;
		}
	}
	move = 0.0;
}

//描画
void boxOBJ::Render()
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);	//移動
	glMultMatrixd(Matrix);	//回転行列乗算
	
	glCallList(DisplayList);
	
	glPopMatrix();
}

//デストラクタ
boxOBJ::~boxOBJ(){
	glDeleteTextures(1, &texturePNG);
}


//氷のとこ用
//垂直抗力の取得と適用
void boxOBJ::NormalForceIce(Vector3 &_pos, Vector3 &_speed)
{
	Vector3 v = {_pos.x-pos.x, _pos.y-pos.y, _pos.z-pos.z }, nvec;	
	
	//移動したとき
	if( move.x!=0.0 || move.y!=0.0 || move.z!=0.0 ){
		//if(laston & 1){
			//_pos += move;
			v += move;
			//_speed += move;
		//}
	}
	
	//判定球の外の場合
	if( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ) > r+sqrt( _speed.x*_speed.x + _speed.y*_speed.y + _speed.z*_speed.z ) ){
		laston =0;
		move = 0.0;
		return;
	}
	
	
	//ここ変更 fabs():絶対値を返す
	if( laston&2 &&
		fabs(v.x) < oVertex[0].x && fabs(v.z) < oVertex[0].z ){	//前回乗ってて回転したとき
		float vm[4];
		vm[0]=v.x; vm[1]=v.y; vm[2]=v.z; vm[3]=1.0;
		Matrixkakeru(vm, vm, mat.m);
		_pos.x += vm[0]-v.x; _pos.y += vm[1]-v.y; _pos.z += vm[2]-v.z;	//回転に合わせて移動
		//_speed.x -= vm[0]-v.x; _speed.y -= vm[1]-v.y; _speed.z -= vm[2]-v.z;	//回転に合わせて移動
		v.x=vm[0]; v.y=vm[1]; v.z=vm[2];
	}
	
	laston_i--; laston=0;
	for(int i=0; i<6; i++){
		nvec = colli[i].GetNormalForce( v, _speed);	//垂直抗力計算
		if( nvec.x==0.0 && nvec.y==0.0 && nvec.z==0.0 ){
		}else{
			laston_i=2;
			_speed += nvec;
		}
	}
	if(laston_i>0) laston |= 1;
	
	//移動したとき
	if( move.x!=0.0 || move.y!=0.0 || move.z!=0.0 ){
		if(laston & 1){
			//_pos += move;
			//v += move;
			_speed += move;
		}
	}
	move = 0.0;
}



