//----------------------------------------------------
// File : XLoader.cpp
//
// かなり変更を加えた。2011.6
// ワンスキンメッシュのみに対応。三角形ポリゴン対応。四角形にも対応(三角形に分割)。
// アニメーションに対応。UVテクスチャ(PNGのみ)に対応。
// SkinWeightの読み込みを、AnimationSetを読み込む直前にした。
// blenderで出力する際、"flip z"はoff、"anim"をonにする。
//----------------------------------------------------

//
// include
//
#include "XLoader.h"

//ファイルの巨大化防止のため、分けたやつ。GetToken()とかの定義はこの中
#include "XLoader_sub.cpp"

////////////////////////////////////////////////////////////////////////
// XFace class
////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Name : XFace()
// Desc : コンストラクタ
//-------------------------------------------------------------------------------------------------
XFace::XFace()
{
	for(int i=0; i<3; i++ )
		indexVertices[i] = 0;
}

////////////////////////////////////////////////////////////////////////
// XMaterial class
////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------
// Name : XMaterial()
// Desc : コンストラクタ
//--------------------------------------------------------------------------------------------------
XMaterial::XMaterial()
{
	name = NULL;
	ambient.r = 0.0f;	ambient.g = 0.0f;	ambient.b = 0.0f;	ambient.a = 0.0f;
	diffuse.r = 0.0f;	diffuse.g = 0.0f;	diffuse.b = 0.0f;	diffuse.a = 0.0f;
	specular.r = 0.0f;	specular.g = 0.0f;	specular.b = 0.0f;	specular.a = 0.0f;
	emissive.r = 0.0f;	emissive.g = 0.0f;	emissive.g = 0.0f;	emissive.a = 0.0f;
	power = 0.0f;
	textureFileName = NULL;
	texture = 0;
}

////////////////////////////////////////////////////////////////////////
// XMesh class
////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------
// Name : XMesh()
// Desc : コンストラクタ
//---------------------------------------------------------------------------------------------------
/*XMesh::XMesh()
{
	name = NULL;
}*/

///////////////////////////////////////////////////////////////////////
// XModel class
////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------
// Name : XModel()
// Desc : コンストラクタ
//---------------------------------------------------------------------------------------------------
XModel::XModel()
{
	numMaterials = 0;
	numVertices = 0;
	numFaces = 0;
	bTexture = false;
	oVertex = NULL;
	aVertex = NULL;
	face = NULL;
	indexMaterials = NULL;
	material = NULL;
	loaded = 0;
	
	vertexBuffer=0; indexBuffer=0;
	vbo = false;
	numOFaces = 0;
	OFaceFour = NULL;
	filedir = NULL;
}

//--------------------------------------------------------------------------------------------------
// Name : Load()
// Desc : テキストフォーマットのXファイルをロードする
//--------------------------------------------------------------------------------------------------
bool XModel::Load(char *filename, bool AnimOnly)
{	
	printf("Xファイル読み込み開始: %s\n", filename);
	
	//ファイル読み込み
	std::ifstream ifs( filename );
	//char buf[1024];
	
	if(ifs.fail()){
		cout << "Error : ファイルが開けませんでした" << endl;
		cout << "File Name : " << filename << endl;
		return false;
	}
	
	std::fstream::pos_type begp, endp, fsize;
	
	//ファイル終端に移動
	ifs.seekg(0, std::ios_base::end);
	endp = ifs.tellg();
	//ファイル先頭に移動
	ifs.seekg(0, std::ios_base::beg);
	begp = ifs.tellg();
	//ファイルサイズを求める
	fsize = endp - begp;
	//バッファ確保
	char buffer[(int)fsize+1];
	//char* buffer = new char[(int)fsize+1];
	//if (buffer == NULL) return false;
	buffer[fsize] = '\0';
	//読み込み
	ifs.read(buffer, fsize);
//ファイル読み込みここまで

	//　Pointerに読み込んだバッファの先頭アドレスをセット
	Pointer = buffer;
	
	//ノードを1に初期化
	Node = 1;	//(ルート)
	
	//Xファイルがあるディレクトリを求める
	int csize = 0;
	char *c = strrchr(filename, '/');	//文字検索
	if( c != NULL ){
		csize = c - filename + 1;	//ディレクトリの文字数
	}
	filedir = new char[csize+1];	//ディレクトリ名用の文字列確保
	filedir[csize] = '\0';
	for(int i=0; i<csize; i++)
		filedir[i] = filename[i];
	
	
	//　ループ (内容解析)
	while ( *Pointer != '\0' )
	{
		//　トークンを取得
		GetToken();
		
		if(AnimOnly){
			//アニメーションのみ読み込み。CheckToken()が偽の場合、Load_AnimationSet()は実行されない。
			if( CheckToken("AnimationSet") && Load_AnimationSet() )
				loaded |= 1;	//エラー検出
			
		}else{
			//要素を識別して読み込み。
			if( !Load_element() )
				loaded |= 1;	//エラー検出
		}
	}
	
	//最終ノードは1になるはず
	if( Node != 1 )
		printf("最終ノードがおかしいです。ノード：%d", Node );
	
	
	//ディスプレイリストの作成
	//SetDListRender();
	
	//アニメーション関連
	if( !Animation.empty() ){
		//アニメーションを読み込んでる場合は、アニメーションの初期化
		Animation[0]->weight = 1.0;
		Animate(0, 1.0);
	}else{
		//アニメーションを読み込んでいない場合は、アニメーション頂点バッファにオリジナルを代入しておく
		for(int i=0; i<numVertices; i++){
			aVertex[i].uv.x = oVertex[i].uv.x;
			aVertex[i].uv.y = oVertex[i].uv.y;
			aVertex[i].normal.x = oVertex[i].normal.x;
			aVertex[i].normal.y = oVertex[i].normal.y;
			aVertex[i].normal.z = oVertex[i].normal.z;
			aVertex[i].pos.x = oVertex[i].pos.x;
			aVertex[i].pos.y = oVertex[i].pos.y;
			aVertex[i].pos.z = oVertex[i].pos.z;
		}
	}
	
	//デバッグ用
	printf("numOFaces %i\n", numOFaces);
	printf("numFaces %i\n", numFaces);
	printf("bTexture = %s\n", (bTexture?"true":"false") );
	
	
	//VBO作成
	//SetVBORender();
	//vbo = true;
	
	return true;
}

/*-----------------------------------------------------------------------------------*
	要素を識別して読み込み。
	XModel::Load() と XFrame::XFrame() から呼ばれる。
 *-----------------------------------------------------------------------------------*/
bool XModel::Load_element()
{
		//　templateの場合
		if ( CheckToken("template") )
		{
			//　スキップする
			SkipNode();
		}

		//　Meshの場合
		else if ( CheckToken("Mesh") )
		{
			printf("Mesh 読み込み開始\n");
			if(loaded & 2){	//Mesh は 2
				cout << "Error : Mesh はすでに読み込んでいます。\n" << endl;
				return false;
			}else{
				loaded |= 2;
			}
			
			//　トークンを取得
			GetToken();
			
			//　トークンが" { " でない場合
			if ( !CheckToken("{") )
			{
				//名前は無視
			
				//　トークンを取得
				GetToken("{");
			}
			
			//　トークンから頂点数を取得
			numVertices = GetIntToken();
			
			//頂点数分確保
			oVertex = new ORIGINAL_VERTEX[numVertices];
			aVertex = new ANIMATED_VERTEX[numVertices];
			
			//頂点数分処理
			for( int i=0; i<numVertices; i++ )
			{
				//　トークンから頂点データを取得
				oVertex[i].pos.x = GetFloatToken();
				oVertex[i].pos.y = GetFloatToken();
				oVertex[i].pos.z = GetFloatToken();
				
				//ウェイトとインデックス初期化
				for(int j=0;j<4;j++) oVertex[i].weight[j]=0.0;
				oVertex[i].weight[0] = 1.0;
				for(int j=0;j<4;j++) oVertex[i].index[j]=0;
				
				//アニメーション頂点バッファも初期化
				aVertex[i].uv.x = 0.0;
				aVertex[i].uv.y = 0.0;
				aVertex[i].normal.x = 0.0;
				aVertex[i].normal.y = 0.0;
				aVertex[i].normal.z = 0.0;
				aVertex[i].pos.x = oVertex[i].pos.x;
				aVertex[i].pos.y = oVertex[i].pos.y;
				aVertex[i].pos.z = oVertex[i].pos.z;
			}
			
			//　トークンから面数を取得
			numOFaces = GetIntToken();
			
			//四角形面があるかどうか、最終面数決定
			char *StartPoint = Pointer;	//現在位置保持
			numFaces = numOFaces;	//面数
			bool sikakkei = false;	//四角形が見つかったかどうかの変数
			//すべての面を見て、四角形があるか調べる
			for(int i=0; i<numOFaces; i++){
				GetToken();	//トークン取得
				//　三角形の場合　トークン3つ飛ばす
				if ( CheckToken("3") ){
					for( int j=0; j<3; j++ ) GetToken();
				}
				//　四角形の場合
				else if ( CheckToken("4") )
				{
					if( sikakkei == false ){
						cout << "情報 : 四角形の面がありました。三角形に分割して処理します。\n" << endl;
						sikakkei = true;
					}
					numFaces++;	//分割分面数を増やす。
					
					//　トークン4つ飛ばす
					for( int j=0; j<4; j++ ) GetToken();
				}
			}
			Pointer = StartPoint;	//ポインタをもとに戻す
			
			//　面数分確保
			face = new XFace[numFaces];
			indexMaterials = new int[numFaces];
			OFaceFour = new bool[numFaces];
			
			//　トークンから面データを取得
			for( int i=0; i<numFaces; i++ )
			{
				indexMaterials[i] = -1;
				
				//　トークンを取得
				GetToken();
				
				//　三角形の場合
				if ( CheckToken("3") )
				{
					//　トークンから頂点インデックスを取得
					for( int j=0; j<3; j++ )
					{
						face[i].indexVertices[j] = GetIntToken();
					}
					//面数記録
					OFaceFour[i] = false;
				}

				//　四角形の場合
				else if ( CheckToken("4") )
				{
					// トークンから頂点インデックスを取得
					
					//１つ目
					for( int j=0; j<3; j++ )
					{
						face[i].indexVertices[j] = GetIntToken();
					}
					OFaceFour[i] = true;	//四角形かどうか
					
					//２つ目
					i++;
					indexMaterials[i] = -1;
					face[i].indexVertices[0] = face[i-1].indexVertices[0];
					face[i].indexVertices[1] = face[i-1].indexVertices[2];
					face[i].indexVertices[2] = GetIntToken();
					OFaceFour[i] = false;	//分割した２つ目はfalseにしとく
				}
			}
		}

		//　MeshNormalsの場合
		else if ( CheckToken("MeshNormals") )
		{
			printf("MeshNormals 読み込み開始\n");
			if(loaded & 4){	//Mesh は 4
				cout << "Error : MeshNormals はすでに読み込んでいます。\n" << endl;
				return false;
			}else{
				loaded |= 4;
			}
			
			//　トークンを取得
			GetToken("{");

			//　トークンから法線数を取得
			int numNormals = GetIntToken();
			
			//格納
			if(numVertices == numNormals){
				// 頂点数と法線数が等しい時
				
				//　トークンから法線データを取得
				for ( int i=0; i<numVertices; i++ )
				{
					oVertex[i].normal.x = GetFloatToken();
					oVertex[i].normal.y = GetFloatToken();
					oVertex[i].normal.z = GetFloatToken();
				}
				
				//　法線インデックス数をチェック
				if ( GetIntToken() != numOFaces )
				{
					cout << "Error : 面数と法線インデックス数が一致していません\n";
					return false;
				}
			}
			else{
				// 頂点数と法線数が等しくない時
				
				// 法線一時保存用配列
				Vector3 tempNormal[numNormals];
				
				//　トークンから法線データを取得
				for ( int i=0; i<numNormals; i++ )
				{
					tempNormal[i].x = GetFloatToken();
					tempNormal[i].y = GetFloatToken();
					tempNormal[i].z = GetFloatToken();
				}
				
				//　法線インデックス数をチェック
				if ( GetIntToken() != numOFaces )
				{
					cout << "Error : 面数と法線インデックス数が一致していません\n";
					return false;
				}
				
				for ( int i=0; i<numFaces; i++ )
				{	
					//　トークンを取得
					GetToken();
					
					//　三角形の場合
					if ( CheckToken("3") )
					{
						//　トークンから法線インデックスを取得
						for ( int j=0; j<3; j++ )
						{
							int a = GetIntToken();
							oVertex[face[i].indexVertices[j]].normal.x = tempNormal[ a ].x;
							oVertex[face[i].indexVertices[j]].normal.y = tempNormal[ a ].y;
							oVertex[face[i].indexVertices[j]].normal.z = tempNormal[ a ].z;
						}
					}
					
					//　四角形の場合
					else if( CheckToken("4") )
					{
						//　法線インデックスを取得
						for ( int j=0; j<3; j++ )
						{
							int a = GetIntToken();
							oVertex[face[i].indexVertices[j]].normal.x = tempNormal[ a ].x;
							oVertex[face[i].indexVertices[j]].normal.y = tempNormal[ a ].y;
							oVertex[face[i].indexVertices[j]].normal.z = tempNormal[ a ].z;
						}
						//４つ目
						int a = GetIntToken();
						i++;
						oVertex[face[i].indexVertices[2]].normal.x = tempNormal[ a ].x;
						oVertex[face[i].indexVertices[2]].normal.y = tempNormal[ a ].y;
						oVertex[face[i].indexVertices[2]].normal.z = tempNormal[ a ].z;
					}
				}
			}
			//読み飛ばす
			EndNode();	//ノードが終わるとこまで飛ばす
			printf("Check11\n");
		}

		//　MeshTextureCoordsの場合 メッシュテクスチャ座標
		else if ( CheckToken("MeshTextureCoords") )
		{
			printf("MeshTextureCoords 読み込み開始\n");
			if(loaded & 8){	//MeshTextureCoords は 8
				cout << "Error : MeshTextureCoords はすでに読み込んでいます。\n" << endl;
				return false;
			}else{
				loaded |= 8;
			}
			
			//　トークンを取得
			GetToken("{");

			//　トークンからテクスチャ座標数を取得
			if( GetIntToken() != numVertices )
			{
				cout << "Error : 頂点数とテクスチャ座標数が一致していません\n";
				return false;
			}
			
			for ( int i=0; i<numVertices; i++ )
			{
				//　トークンからテクスチャ座標データを取得
				oVertex[i].uv.x = GetFloatToken();
				oVertex[i].uv.y = GetFloatToken();
				//揃える(0.0〜1.0の間になるように調整)
				oVertex[i].uv.x -= int(oVertex[i].uv.x) - (oVertex[i].uv.x<0?1:0) ;
				oVertex[i].uv.y -= int(oVertex[i].uv.y) - (oVertex[i].uv.y<0?1:0) ;
				//アニメーションバッファにもコピーしておく
				aVertex[i].uv.x = oVertex[i].uv.x;
				aVertex[i].uv.y = oVertex[i].uv.y;
			}
			
			bTexture = true;	//テクスチャ座標は読み込んだ
			
		}

		//　MeshMaterialListの場合 メッシュマテリアルリスト & マテリアル
		else if ( CheckToken("MeshMaterialList") )
		{
			printf("MeshMaterialList 読み込み開始\n");
			if(loaded & 16){	//MeshMaterialList は 16
				cout << "Error : MeshMaterialList はすでに読み込んでいます。\n" << endl;
				return false;
			}else{
				loaded |= 16;
			}
			
			//　トークンを取得
			GetToken("{");

			//　トークンからマテリアル数を取得
			numMaterials = GetIntToken();
			
			//マテリアル数分確保
			material = new XMaterial[numMaterials];
			
			//　マテリアル数をチェック
			if ( GetIntToken() != numOFaces )
			{
				cout << "Error : 面数とマテリアルリスト数が一致しません\n";
				return false;
			}
			
			//　トークンからマテリアルインデックスを取得
			SUBSET sub;	sub.material = -1;	//サブセット用
			for ( int i=0; i<numFaces; i++ )
			{
				//マテリアル番号を格納
				indexMaterials[i] = GetIntToken();
				
				//サブセット
				if(sub.material != indexMaterials[i]){
					if(sub.material != -1){	//はじめ以外
						//subsetに追加。
						subset.push_back(sub);
					}
					//次の初期化
					sub.material = indexMaterials[i];
					sub.index = i;
					sub.count = 0;
				}
				sub.count++;
				
				//四角形だったら一個すすめる
				if(OFaceFour[i]){
					i++;
					sub.count++;
					indexMaterials[i] = indexMaterials[i-1];
				}
			}
			subset.push_back(sub);	//subsetに追加。
			
			
			// Material マテリアル読み込み
			for ( int i=0; i<numMaterials; i++ )
			{
				printf("Material %d 読み込み開始\n",i);
				
				//　トークンを取得
				GetToken("Material");
				GetToken();
				
				//名前
				//　トークンが" { "でない場合
				if ( !CheckToken("{") )
				{
					//　名前をセット
					material[i].name = new char[strlen(Token+1)];
					strcpy(material[i].name, Token);
				
					//　トークンを取得
					GetToken("{");
				}
				else	//トークンが" { "の場合
				{
					//　名前は"mat*"とする
					material[i].name = new char[10];
					sprintf(material[i].name, "mat%d", i);
				}

				//　Ambient Color
				material[i].ambient.r = 0.15f;
				material[i].ambient.g = 0.15f;
				material[i].ambient.g = 0.15f;
				material[i].ambient.a = 1.0f;

				//　Diffuse Color
				material[i].diffuse.r = GetFloatToken();
				material[i].diffuse.g = GetFloatToken();
				material[i].diffuse.b = GetFloatToken();
				material[i].diffuse.a = GetFloatToken();

				//　Shiness
				material[i].power = GetFloatToken();

				//　Specular Color
				material[i].specular.r = GetFloatToken();
				material[i].specular.g = GetFloatToken();
				material[i].specular.b = GetFloatToken();
				material[i].specular.a = 1.0f;

				//　Emissive Color
				material[i].emissive.r = GetFloatToken();
				material[i].emissive.g = GetFloatToken();
				material[i].emissive.b = GetFloatToken();
				material[i].emissive.a = 1.0f;

				//　トークンを取得
				GetToken();

				//　TextureFileNameの場合
				if ( CheckToken("TextureFileName") || CheckToken("TextureFilename") )
				{
					//　トークンを取得
					GetToken("{");
					
					//　トークンを取得
					GetToken();
					//　ファイル名をセット
					material[i].textureFileName = new char[strlen(Token+1)];
					strcpy(material[i].textureFileName, Token);
					
					//拡張子が「.png」かどうか
					char *tfname = material[i].textureFileName;
					if( strcmp( tfname + strlen(tfname) - 4 , ".png" ) &&
						strcmp( tfname + strlen(tfname) - 4 , ".PNG" ) ){
						cout << "Error : PNGファイル以外のテクスチャには対応していません。\n" << endl;
						return false;
					}
					//ファイル名を作る
					char texfilename[strlen(filedir)+strlen(tfname)+1];
					texfilename[0]='\0'; strcat(texfilename, filedir); strcat(texfilename, tfname);
				printf("tesfilename: %s\n",texfilename);
					//テクスチャ読み込み
					pngInfo info;
					material[i].texture = pngBind(texfilename, PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
					
					//　トークンを取得
					GetToken("}");
					GetToken("}");
				}//トークンが" } "の場合はなにもしない
			}
			
		}
		
		// Frame　フレームの場合
		else if( CheckToken("Frame") )
		{
			printf("Frame 読み込み開始\n");
			//Xファイル内に複数のルートフレームがある場合、エラーを返す
			if( !Frame.empty() ){
				cout << "Error : 複数のルートフレームがあります\n" << endl;
				return false;
			}
			
			//フレームの階層構造を読み込む　以下コンストラクタで読み込み処理を行う
			new XFrame(this);
		}
		
		// SkinWeights スキンウェイトの場合
		else if( CheckToken("SkinWeights") )
		{
			/*
			printf("Check1\n");
			//SkinWeightsの位置保存(最後、AnimationSetで読み込む)
			SkinWeightsPointer.push_back(Pointer);
			//ノードを終わりまで飛ばす。
			GetToken("{");
			EndNode();
			printf("Check2\n");
			*/
			Load_SkinWeights();
		}
		
		// AnimationSet アニメーションセットの場合
		else if( CheckToken("AnimationSet") ) {
			/*
			//記録
			char *NowPointer = Pointer;
			
			//SkinWeightの読み込みもやっちゃう
			printf("Check3\n");
			for( int i=0; i<SkinWeightsPointer.size(); i++ ){
				//セット
				Pointer = SkinWeightsPointer[i];
				//読み込み
				if( Load_SkinWeights() == false )
					return false;
			}
			printf("Check4\n");
			
			//現在の位置に戻す
			Pointer = NowPointer;
			*/
			//アニメーション読み込み
			return Load_AnimationSet();
		}
	
	return true;
}

/*-----------------------------------------------------------------------------------*
 * Load_AnimationSet()
 * AnimationSetを読み込む。Load_element()とLoadAnimOnly()から呼ばれる。
 *-----------------------------------------------------------------------------------*/
bool XModel::Load_AnimationSet()
{
	printf("AnimationSet 読み込み開始\n");
	
printf("check1\n");
	//XAnimationクラスのインスタンス作成
	XAnimation* anim=new XAnimation();
	anim->time=0;
	anim->weight=0;
printf("check2\n");
	
	//アニメーションセット名取得
	GetToken();
printf("check2 %d\n",strlen(Token));
	//anim->name=new char[strlen(Token)+2];
printf("check2\n");
	//strcpy(anim->name, Token);
printf("check2\n");
	
	GetToken("{");
printf("check3\n");
	
	//アニメーション読み取り
	while (*Pointer!='\0') {
		GetToken();
		
		//'}'だったら、ループを抜けて読み取り終了
		if ( strchr(Token, '}') ) break;
		
		//"Animation”だったら読み取り
		if ( CheckToken("Animation") ) {	
printf("check4\n");
			//アニメーションキークラスのインスタンス作成
			XAnimationKey* key = new XAnimationKey();	
printf("check4_0\n");	
printf("check4_1\n");
			
			GetToken();		//１個読み飛ばし
			if ( strcmp(Token, "{") ) GetToken("{");	//アニメーション名があった場合、もう一個飛ばす
			
			//フレームの名前取得、フレーム番号を検索して格納
			GetToken("{");
			GetToken();	
printf("check4_2\n");
			
			//key->frameName=new char[strlen(Token)+1];
printf("check4_3\n");
			//strcpy(name, Token);		//名前格納
			//string 
			key->frameIndex=FindFrame(key->frameName)->index;	//フレームを検索して格納
			if( !(key->frameIndex) ) return false;	//フレームが見つからなかった場合
			GetToken("}");
			
			GetToken("AnimationKey");	//"AnimationKey"であることを確認
			GetToken("{");
			GetToken("4");	//タイプが4であることを確認
			
printf("check5\n");
			//キー数を取得。キー数分だけ読み取り。
			int key_count=GetIntToken();
			for (int i=0; i<key_count; i++) {
				//時間を取得
				key->time.push_back((float)GetIntToken());	
				
				//"16"であることを確認
				GetToken("16");
				
				//行列を取得
				MATRIX4x4 *m = new MATRIX4x4;
				for(int g=0; g<4; g++)
						for(int r=0; r<4; r++)
							m->m[g][r]=GetFloatToken();
				key->matrix.push_back(m);
			}
			GetToken("}");	
			GetToken("}");
			//キー追加
			anim->key.push_back(key);
		}
	}
	//追加
	Animation.push_back(anim);
	return true;
}

/*-----------------------------------------------------------------------------------*
 * Load_SkinWeights()
 * AnimationSetを読み込む。Load_element()とLoadAnimOnly()から呼ばれる。
 *-----------------------------------------------------------------------------------*/
bool XModel::Load_SkinWeights()
{
	
				printf("SkinWeights 読み込み開始");
				GetToken("{");
				
				GetToken();	//フレーム名取得
				printf(" Frame名：%s\n",Token);
				//フレーム検索
				XFrame* frame=FindFrame(Token);
				//フレームが見つかった場合
				if (frame)
				{
					//ウェイト数の取得
					int count=GetIntToken();
					
					//インデックスの読み込み
					int* vertex=new int[count];
					for (int i=0; i<count; i++) vertex[i]=GetIntToken();
					
					//ウェイトを変数へ格納
					for (int i=0; i<count; i++) {
						int j=0;
						
						//まだウェイトを書き込んでないとこを見つける
						while (j<3 && oVertex[vertex[i]].weight[j]>0.0 && oVertex[vertex[i]].weight[j]<1.0) j++;
						
						//見つかったらそこへ書きこむ
						if (j<3) {
							oVertex[vertex[i]].weight[j]=GetFloatToken();	//ウェイト
							oVertex[vertex[i]].index[j]=(unsigned char)frame->index;	//ボーン番号
						}
					}
					
					//オフセット行列の読み込み
					for(int g=0; g<4; g++)
						for(int r=0; r<4; r++)
							frame->OffsetMatrix[g][r]=GetFloatToken();
					
					GetToken("}");
					
					delete[] vertex;
					
				}
				//フレームが見つからなかったとき
				else{
					cout << "Error : SkinWeights:フレームが見つかりません\n" << endl;
					return false;
				}
	return true;
}

//指定した名前のフレームを検索
XFrame* XModel::FindFrame(const char* name) {
	for (size_t i=0; i<Frame.size(); i++) {
		if (strcmp(Frame[i]->name, name)==0) return Frame[i];
	}
	cout << "指定されたフレームは見つかりませんでした。\n" << endl;
	return NULL;
}

//デストラクタ
XMaterial::~XMaterial(){
	printf("~XMaterial ");
	delete [] name;
	delete [] textureFileName;
	printf("~XMaterial\n");
}

//デストラクタ
/*XMesh::~XMesh(){
	printf("~XMesh\n");
	delete [] name;
}*/

//デストラクタ
XModel::~XModel(){
	printf("~XModel ");
	delete [] oVertex;
	delete [] aVertex;
	delete [] face;
	delete [] material;
	for(int i=0;i<Frame.size();i++)
		delete Frame[i];
	for(int i=0;i<Animation.size();i++)
		delete Animation[i];
	delete [] OFaceFour;
	delete [] filedir;
	printf("~XModel\n");
}


//---------------------------------------------------------------------------------------------------
// Name : Render()
// Desc : メッシュの描画
//---------------------------------------------------------------------------------------------------
void XModel::Render(float scale)
{
	int pre_mat = -1;
	int cur_mat = 0;
	bool exist_material = false;
	
	if( loaded & 1 ){
		cout << "読み込みエラーが発生しているため、描画を中止します。\n" << endl;
		return;	//エラーがあった場合
	}

	if ( numMaterials>0 ){ exist_material = true; }else{ glBegin(GL_TRIANGLES); }

	for ( int i=0; i<numFaces; i++ )
	{
		//　マテリアルが存在する場合
		if ( exist_material )
		{
			//　マテリアルインデックスを取得
			cur_mat = indexMaterials[i];
			
			//　マテリアルインデックスが異なる場合
			if ( cur_mat != pre_mat )
			{
				//描画終わり(最初のループ以外で)
				if(i) glEnd();
				
				//　基本色
				glColor4fv(material[cur_mat].diffuse);
				
				//　Ambient Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material[cur_mat].ambient);
				//　Diffuse Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material[cur_mat].diffuse);
				//　Specular Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material[cur_mat].specular);
				//　Emissive Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material[cur_mat].emissive);
				//　Shininess
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material[cur_mat].power);
				
				//テクスチャのバインド
				if( bTexture )
					glBindTexture(GL_TEXTURE_2D, material[cur_mat].texture);
				
				//　マテリアルインデックスを更新
				pre_mat = cur_mat;
				
				//描画再開
				glBegin(GL_TRIANGLES);
			}
		}
		
		for ( int j=0; j<3; j++ )
		{
			//　テクスチャ座標が存在する場合
			if ( bTexture )
				glTexCoord2fv(aVertex[face[i].indexVertices[j]].uv);
			
			//　法線ベクトル
			glNormal3fv(aVertex[face[i].indexVertices[j]].normal);
			
			//　頂点座標
			glVertex3f(
				aVertex[face[i].indexVertices[j]].pos.x * scale,
				aVertex[face[i].indexVertices[j]].pos.y * scale,
				aVertex[face[i].indexVertices[j]].pos.z * scale );
		}
	}
	//描画終了
	glEnd();
	//バインドしたのをもとに戻す
	if( bTexture ) glBindTexture(GL_TEXTURE_2D, 0);
}


/*-----------------------------------------------------------------------------------*
	ディスプレイリストを作る。Load()内で呼ばれる。
 *-----------------------------------------------------------------------------------*/
/*void XModel::SetDListRender(float scale){
	printf("SetDListRender\n");
	if(!Animation.empty()){
		Animation[0]->time = 10.0;
		Animation[0]->weight = 1.0;
		AnimateFrame(); 
	}
	AnimateVertex();
	
	DisplayList = glGenLists(1);//ディスプレイリストを作成
	glNewList(DisplayList,GL_COMPILE); //コンパイルのみ 	
	Render(scale);	//レンダリング
	glEndList(); //リスト終わり
}

//ディスプレイリストで描画
void XModel::DListRender(){
	glCallList(DisplayList);	//描画
}*/



///////////////////////////////////////////////////////////////////////////////////////
/*-----------------------------------------------------------------------------------*
	XObjクラス
	オブジェクトの管理はこいつをつかうとよい
 *-----------------------------------------------------------------------------------*/
//コンストラクタ
XObj::XObj(){
	model = NULL;
	pos.x=0.0; pos.y=0.0; pos.z=0.0;
	ang.x=0.0; ang.y=0.0; ang.z=0.0;
	angle = 0.0;
	speed.x=0.0; speed.y=0.0; speed.z=0.0;
	offset.x=0.0; offset.y=0.0; offset.z=0.0;
	force = 0.0;
}

//Xモデル登録
void XObj::setXModel(XModel *m){
	model = m;	//Xモデルのポインタ登録
}

//描画
void XObj::Render(){
	//描画位置まで移動
	glPushMatrix();	//プッシュ
	glTranslatef(offset.x+pos.x, offset.y+pos.y, offset.z+pos.z);	//移動
	glRotatef(angle, ang.x, ang.y, ang.z);	//回転
	
	if( model->VBO() ){
		//vboバインド＆編集可能にする
		model->BindVertexBuffer();
		model->aVertex = (ANIMATED_VERTEX*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		
		//アニメーション
		if( !model->Animation.empty() && model->Animate(0, 1.0) )
			model->ResetAnimate(0); 
		
		//アンマップ
		glUnmapBuffer(GL_ARRAY_BUFFER);
		
		//vboで描画
		model->VBORender();
		 
		//バインドしたのをもとに戻す  
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		model->aVertex = NULL;
	}else{
		//アニメーション
		if( !model->Animation.empty() && model->Animate(0, 1.0) )
			model->ResetAnimate(0); 
		
		//model->DListRender();	//描画
		model->Render();
	}
	
	//ポップ
	glPopMatrix();
}

//デストラクタ
XObj::~XObj(){
	
}



