//XLoader_sub.cpp
/*-----------------------------------------------------------------------------------*
 * XLoader.cppが大きくなりすぎるため、自作部だけファイルを分けた。
 * ヘッダに記されてない関数の定義もここに移した。(GetToken()とか...)
 * このファイルはXLoader.cppの中でincludeされる。
 *-----------------------------------------------------------------------------------*/

//こっから#endifまでを省略可能（開発エディタによるが）
#if 1	//囲むため

//
// global Variables
// 静的グローバル関数(このソース内でしか使用できない)
//
static char* Pointer;
static char Token[1024];
static int Node;	//ノード (ルートが１で、深いほど値が大きい)


//-------------------------------------------------------------------------------------------------
// Name : CheckToken()
// Desc : 指定された文字列とトークンをチェックする
//-------------------------------------------------------------------------------------------------
static bool CheckToken(const char* str)
{
	if ( strcmp(Token, str) == 0 )
		return true;
	else
		return false;
}

//-------------------------------------------------------------------------------------------------
// Name : GetToken()
// Desc : トークンを取得する
//-------------------------------------------------------------------------------------------------
static void GetToken()
{
	char *p = Pointer;
	char *q = Token;
	bool slash=false;

	//" ¥t¥r¥n,;¥"" じゃないとこまで移動 "//"がでてくると次の行まで飛ばす
	while ( *p != '\0' && strchr(" \t\r\n,;\"/", *p) ){
		p++;
		if( *p == '/' ){
			if( slash ){
				while( !strchr("\r\n", *p) )
					p++;
			}
			slash = !slash;
		}else{
			slash = false;
		}
	}
	
	//slashはんていがあったときは戻す
	if( slash ){ p--; slash=false; }
	
	//"{}"じゃないときは、" ¥t¥r¥n,;¥"{}"が出てくるまでqに記録
	if (( *p == '{' && Node++ )||( *p == '}' && Node-- ))
		(*q++) = (*p++);
	else
		while ( *p != '\0' && !strchr(" \t\r\n,;\"{}", *p ) )
			(*q++) = (*p++);

	Pointer = p;
	*q = '\0';
}

//--------------------------------------------------------------------------------------------------
// Name : GetToken()
// Desc : トークンを取得し指定された文字列を比較を行う
//--------------------------------------------------------------------------------------------------
static bool GetToken(const char* token)
{
	GetToken();
	if ( strcmp(Token, token) != 0 )
	{
		cout << "Error : 想定トークンと読込トークンが一致しません\n";
		cout << "想定トークン：" << token << endl;
		cout << "読込トークン：" << Token << endl;
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------
// Name : GetFloatToken()
// Desc : トークンをfloat型に変換し取得する
//--------------------------------------------------------------------------------------------------
static float GetFloatToken()
{
	GetToken();
	return (float)atof(Token);
}

//--------------------------------------------------------------------------------------------------
// Name : GetIntToken()
// Desc : トークンをint型に変換し取得する
//--------------------------------------------------------------------------------------------------
static int GetIntToken()
{
	GetToken();
	return (int)atoi(Token);
}

/*-----------------------------------------------------------------------------------*
 * EndNode()
 * 今いるノードが終わるとこまで飛ばす
 *-----------------------------------------------------------------------------------*/
static void EndNode(){
	int count = 1;
	while ( *Pointer != '\0' && count > 0 )
	{
		GetToken();
		if ( strchr(Token, '{' ) ) count++;
		else if ( strchr(Token, '}') ) count--;
	}
	if ( count > 0 )
	{
		cout << "Error : カッコが一致していません\n";
		return;
	}
}

//-------------------------------------------------------------------------------------------------
// Name : SkipNode()
// Desc : ノードを飛ばす
//-------------------------------------------------------------------------------------------------
static void SkipNode()
{
	while ( *Pointer != '\0' )
	{
		GetToken();
		if ( strchr(Token, '{' ) ) break;
	}
	EndNode();
}


#endif	//#if 1  囲むため


///////////////////////////////////////////////////////////////////////////////////////////////////
//こっから自作部

/*-----------------------------------------------------------------------------------*
 * XModelクラス
 *-----------------------------------------------------------------------------------*/
//頂点の変換。フレームから頂点位置を計算
void XModel::AnimateVertex()
{
	float m[4];
	ORIGINAL_VERTEX *ov = oVertex;
	Vector3 pos,nom;
	
	for (int i=0; i<numVertices; i++) {
		// 頂点、法線を初期化。
		pos.x=0.0; pos.y=0.0; pos.z=0.0;
		nom.x=0.0; nom.y=0.0; nom.z=0.0;
		// ウェイト分繰り返し
		for (int j=0; j<4; j++) {
			//頂点
			m[0]=ov[i].pos.x; m[1]=ov[i].pos.y; m[2]=ov[i].pos.z; m[3]=1.0;
			Matrixkakeru(m, m, Frame[ov[i].index[j]]->SkinningMatrix);
			pos.x += m[0] * ov[i].weight[j];
			pos.y += m[1] * ov[i].weight[j];
			pos.z += m[2] * ov[i].weight[j];
			//法線
			m[0]=ov[i].normal.x; m[1]=ov[i].normal.y; m[2]=ov[i].normal.z; m[3]=1.0;
			Matrixkakeru(m, m, Frame[ov[i].index[j]]->SkinningMatrix);
			nom.x += m[0] * ov[i].weight[j];
			nom.y += m[1] * ov[i].weight[j];
			nom.z += m[2] * ov[i].weight[j];
		}
		//正規化
		NormalizeVec(&nom);
		
		//書き込み
		aVertex[i].pos.x = pos.x;
		aVertex[i].pos.y = pos.y;
		aVertex[i].pos.z = pos.z;
		aVertex[i].normal.x = nom.x;
		aVertex[i].normal.y = nom.y;
		aVertex[i].normal.z = nom.z;
	}
}


//現在のアニメーションの時間に合わせてフレームを変化させる
void XModel::AnimateFrame()
{
	//アニメーションが読み込まれていなかったらreturn
	if(Animation.empty())	return;
	
	for (int i=0; i<Animation.size(); i++) {
	// フレームの変換行列を初期化する
		//animにチェックするアニメーションをセット
		XAnimation* anim=Animation[i];
		
		//weightが0の場合はなにもせずにcontinue
		if (anim->weight==0) continue;

		// キーに含まれるフレームの変換行列を0で初期化する
		for (int j=0; j<anim->key.size(); j++) {
			XAnimationKey* key=anim->key[j];
			//TransformMatrixを0で初期化する
			memset(Frame[key->frameIndex]->TransformMatrix, 0, sizeof(float)*16);
		//}

	// フレームの変換行列を計算する
		
		//すべてのキーについて処理
		//for (int j=0; j < anim->key.size(); j++) {
			// key と frame にセット
			//XAnimationKey* key=anim->key[j];
			XFrame* frame=Frame[key->frameIndex];
			
			//キーが空の場合はcontinue
			if (key->time.empty()) continue;
			
			
			// 現在の時間によってどのキーを使用するか決定する
			float time = anim->time;
			
			//有効時間よりも前の場合、最初のキーを使用
			if ( time <= key->time.front() ) {
				MatrixTasuW(frame->TransformMatrix, key->matrix.front()->m, anim->weight);
			}
			
			//有効時間よりも後の場合、最後のキーを使用
			else if (time >= key->time.back() ) {
				MatrixTasuW(frame->TransformMatrix, key->matrix.back()->m, anim->weight);
			}
			
			//有効時間内の場合
			else {
				for (int k=1; k < key->time.size(); k++) {
					//現在の時間の前後にあるキーをさがす
					if ( time < key->time[k]  &&  key->time[k-1] != key->time[k] ) {
						//キーの比率をもとめる。
						float r = ( key->time[k] - time ) / ( key->time[k] - key->time[k-1] );
						
						//行列を加算
						MatrixKeyTasuRW(frame->TransformMatrix, key->matrix[k-1]->m, key->matrix[k]->m, r, anim->weight);
						
						break;
						
					}
				}
			}
		}
	}
	
	// フレームの合成行列とスキニング行列を計算する
	float world[4][4];
	MatrixIdentity(world);
	Frame[0]->Animate(world);

/*	// Aj[VØèÖ¦âÔ
	if (SmootherWeight>0) {
		for (size_t i=0; i<Frame.size(); i++) {
			D3DXMATRIXA16& a=Frame[i]->CombinedMatrix;
			D3DXMATRIXA16& b=Frame[i]->SmootherMatrix;
			a=a*(1-SmootherWeight)+b*SmootherWeight;
			Frame[i]->SkinningMatrix=Frame[i]->OffsetMatrix*a;
		}
	}
*/
}

//アニメーションさせる
bool XModel::Animate(int anim, float speed)
{
	Animation[anim]->time += speed;
	AnimateFrame();
	AnimateVertex();
	//アニメーション時間が次で終わりまでいきそうかどうか
	if(Animation[anim]->GetMaxTime() < Animation[anim]->time + speed)	return true;
	return false;
}

//アニメーション時間を0に戻す
void XModel::ResetAnimate(int anim)
{
	Animation[anim]->time = 0.0;
}

/////////////////////////////////////////////////////////////////
// VBOをつかったレンダリング
//

//VBOが使えるように準備する。
void XModel::SetVBORender()
{
	if( loaded & 1 ){
		cout << "読み込みエラーが発生しているため、VBOの作成を中止します。\n" << endl;
		return;	//エラーがあった場合
	}
	
	// バッファー作成。頂点座標、法線、UV座標のバッファー
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	printf("check1\n");
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(ANIMATED_VERTEX), aVertex, GL_STREAM_DRAW);
	
	printf("check2\n");
	
	// インデックスバッファの作成
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * sizeof(XFace), face, GL_STATIC_DRAW);
	
	// バインドしたのをもとに戻す
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	//不要になったので削除
	delete [] aVertex; aVertex = NULL;
	delete [] face; face = NULL;
}

//VBOでレンダリング
void XModel::VBORender()
{	
	if( loaded & 1 ){
		cout << "読み込みエラーが発生しているため、描画を中止します。\n" << endl;
		return;	//エラーがあった場合
	}
	
	//マテリアルを読み込んでいない場合
	if ( numMaterials <= 0 )
	{
		glInterleavedArrays(GL_N3F_V3F, sizeof(ANIMATED_VERTEX), (GLubyte *)NULL + sizeof(Vector2));	//集合的配列のある要素の先頭と、 集合的配列の次の要素の先頭の間には stride バイトある
		glDrawElements(GL_TRIANGLES, numFaces * sizeof(XFace)/sizeof(float), GL_UNSIGNED_INT, NULL);
	}else{
	//マテリアルが存在する場合
	
		if(bTexture)
		{
			//UV座標・法線・頂点の指定
			glInterleavedArrays(GL_T2F_N3F_V3F, 0, NULL);
		}else{
			//法線・頂点の指定
			glInterleavedArrays(GL_N3F_V3F, sizeof(ANIMATED_VERTEX), (GLubyte*)NULL + sizeof(Vector2));
		}
		//サブセット分繰り返し
		for ( int i=0; i<subset.size(); i++ )
		{		
			//　マテリアル更新
				
			//　基本色
			glColor4fv(material[subset[i].material].diffuse);
			
			//　Ambient Color
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material[subset[i].material].ambient);
			//　Diffuse Color
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material[subset[i].material].diffuse);
			//　Specular Color
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material[subset[i].material].specular);
			//　Emissive Color
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material[subset[i].material].emissive);
			//　Shininess
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material[subset[i].material].power);	
			
			//テクスチャのバインド
			if( bTexture )
				glBindTexture(GL_TEXTURE_2D, material[subset[i].material].texture);
			
			//　描画
			glDrawElements(GL_TRIANGLES,
				subset[i].count * sizeof(XFace)/sizeof(float),
				GL_UNSIGNED_INT,
				(GLubyte *)NULL + sizeof(XFace)*subset[i].index);
		}
		//もとに戻す
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

//VBOをバインドしてあげる
void XModel::BindVertexBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}


/*-----------------------------------------------------------------------------------*
 * XFrameクラス
 *-----------------------------------------------------------------------------------*/
//コンストラクタ(読み込み)
XFrame::XFrame(XModel *model)
{
	index = model->Frame.size();	//フレーム番号を格納
	model->Frame.push_back(this);	//フレームをベクターに追加
	
	//行列の初期化
	MatrixIdentity(TransformMatrix);
	MatrixIdentity(OffsetMatrix);
	
	//フレーム名の取得
	GetToken();
	name = new char[strlen(Token)+1];
	strcpy(name, Token);
	
	//フレームからうまく抜けるため、現在のノード記憶
	int node = Node;
	
	//次のトークンが"{"であると確認する
	GetToken("{");
	
	//変換行列と子フレーム読み込み
	while (*Pointer!='\0') {
		GetToken();
		
		//ノードが終わったらループを抜ける
		if ( node == Node ) break;
		
		//フレームの場合、子フレームを作成 
		if ( CheckToken("Frame") ) {
			child.push_back(new XFrame(model));
		}
		
		// 変換行列を読み込む
		else if ( CheckToken("FrameTransformMatrix") ) {
			GetToken("{");
			for(int g=0; g<4; g++)
				for(int r=0; r<4; r++)
					TransformMatrix[g][r]=GetFloatToken();
			GetToken("}");
		}
		
		// 子フレームと変換行列以外の時
		else {
			//要素を識別して読み込み
			if( !model->Load_element() )
				model->loaded |= 1;	//エラー検出
		}
		
	}
}

//行列の合成。TransformMatrixが定まった後、Frame[0]から呼んでやると、全フレームのスキニング行列を再帰的に計算してくれる。
void XFrame::Animate(float parent[4][4])
{
	Matrixkakeru(CombinedMatrix, parent, TransformMatrix);	//合成行列 = 親の合成行列 x 逆BOf行列
	for (size_t i=0; i<child.size(); i++) {
		child[i]->Animate(CombinedMatrix);		//すべての子に合成行列をわたし、行列計算。
	}
	Matrixkakeru(SkinningMatrix, OffsetMatrix, CombinedMatrix);	//スキニング行列 = BOf行列 x 合成行列
}

//デストラクタ
XFrame::~XFrame(){
	delete [] name;
}


/*-----------------------------------------------------------------------------------*
 * XAnimationクラス
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
XAnimation::XAnimation(){
	name = NULL;
	time = 0.0;
	weight = 0.0;
}

//デストラクタ
XAnimation::~XAnimation(){
	printf("~XAnimation\n");
	delete [] name;
	for(int i=0;i<key.size();i++)
		delete key[i];
}

 /*-----------------------------------------------------------------------------------*
 * XAnimationKeyクラス
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
XAnimationKey::XAnimationKey(){
	frameName = NULL;
	frameIndex = 0;
}

//デストラクタ
XAnimationKey::~XAnimationKey(){
	printf("~XAnimationKey\n");
	delete [] frameName;
	for(int i=0;i<matrix.size();i++)
		delete matrix[i];
}




