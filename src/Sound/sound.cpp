// sound.cpp

#include "sound.h"


Sound::Sound(const char *BGMFileName){
	
	//有効なバッファとソース番号を取得します
	alGenSources( 1, &source );
	
	//wavファイルから読み込み
	buffer = alutCreateBufferFromFile( BGMFileName ); //モノラルじゃないと、3D音響にできない
	if( AL_NONE == buffer ){
		printf("Sound 読み込み失敗\n");
		exit(1);	//読み込みが失敗したら終了
	}
	
	//ソースとバッファを結び付けます
	alSourcei( source, AL_BUFFER, buffer );
	
	
	// リスナーの位置(x, y, z)
	alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
	
	// リスナーの向き(x, y, z)とUPベクトル(x, y, z)
	float vec[6] = {0, 0, 1, 0, 1, 0};
	alListenerfv(AL_ORIENTATION, vec);
	
	//再生
	alSourcePlay( source );
	angle = 0.0;
}

void Sound::stream(){
		// 0.1秒待つ
		//alutSleep (0.1);
		
		// 角度を毎フレーム更新
		angle += 0.005f;
		
		// ソースの位置を設定
		float x = cosf(angle)*2;
		float y = 0;
		float z = sinf(angle)*2;
		alSource3f(source, AL_POSITION, x, y, z);
		
		//停止していたら再生
		ALint state;
		if( alGetSourcei(source, AL_SOURCE_STATE, &state), state != AL_PLAYING ){
			alSourcePlay( source );
		}
}
		
Sound::~Sound(){
	//再生されていたら停止
	ALint state;
	if( alGetSourcei(source, AL_SOURCE_STATE, &state), state == AL_PLAYING )
		alSourceStop( source );
		
	//関連付け解除
	alSourcei( source, AL_BUFFER, 0 );
	
	//終了
	alDeleteBuffers(1, &buffer);
	alDeleteSources(1, &source);
}
