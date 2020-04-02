#pragma once
// #include "Singleton.h"
#include <list>
#define USE_NEW_CC_AUDIO

#ifdef USE_NEW_CC_AUDIO
#include <audio/include/AudioEngine.h>
using namespace cocos2d::experimental;
#endif

class MySound
{
public:
	MySound(){}
	~MySound(){}

	static float getDuration(int id);

	static void playMusic(const char* filename, bool bLoop = false);
	static bool isMscPlaying();
	static void stopMusic();
	static void pauseMusic();
	static void resumeMusic();

	static int playEffect(const char* pszFilePath, bool bLoop = false, float pitch = 1.0f, float pan = 0.0f);
	static bool isEffPlaying(int& id);
	static void stopEffect(int& id);
	static void setVolume(float vol);	//0.0f~1.0f

	void callbackPlayEffect(const char* pszFilePath);
	void callbackPlayEffectCanStop(const char* pszFilePath);
	static void stopEffectList();

private:
	static std::list<int>		m_lstEffectId;
};

extern MySound		g_sound;

// 回调函数播放声音 [4/27/2018 pc-ah]
/*
CallFunc::create(CC_CALLBACK_0(MySound::playEffect2, &g_sound, "sound/card_send.mp3")),
*/