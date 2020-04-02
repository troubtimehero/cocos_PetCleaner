#include "MySound.h"
#include <cocos2d.h>

#ifdef USE_NEW_CC_AUDIO

#ifdef _DEBUG
static float m_vol = 0.2f;
#else
static float m_vol = 1.0f;
#endif // _DEBUG

static int gsMusicId = 0;
void MySound::playMusic(const char* filename, bool bLoop)
{
	stopMusic();
	gsMusicId = AudioEngine::play2d(filename, bLoop, m_vol);
}

bool MySound::isMscPlaying()
{
	return AudioEngine::getState(gsMusicId) == AudioEngine::AudioState::PLAYING;
}

void MySound::stopMusic()
{
	AudioEngine::stop(gsMusicId);
}

void MySound::pauseMusic()
{
	AudioEngine::pause(gsMusicId);
}

void MySound::resumeMusic()
{
	AudioEngine::resume(gsMusicId);
}

int MySound::playEffect(const char* pszFilePath, bool bLoop, float, float pan)
{
	return AudioEngine::play2d(pszFilePath, bLoop, m_vol);
}

bool MySound::isEffPlaying(int& id)
{
	return AudioEngine::getState(id) == AudioEngine::AudioState::PLAYING;
}

void MySound::stopEffect(int& id)
{
	AudioEngine::stop(id);
	id = 0;
}

void MySound::setVolume(float vol)
{
	if (vol < 0)
		vol = 0;
	if (vol > 1)
		vol = 1;
	m_vol = vol;
}

void MySound::callbackPlayEffect(const char* pszFilePath)
{
	playEffect(pszFilePath, false, 1, 0);
}

std::list<int> MySound::m_lstEffectId;

void MySound::callbackPlayEffectCanStop(const char* pszFilePath)
{
	int musicId = playEffect(pszFilePath, false, 1, 0);
	m_lstEffectId.push_back(musicId);
	CCLOG("music id: %u", musicId);
}

void MySound::stopEffectList()
{
	for (auto &id : m_lstEffectId)
	{
		if (isEffPlaying(id))
			stopEffect(id);
	}
	m_lstEffectId.clear();
}

float MySound::getDuration(int id)
{
	return AudioEngine::getDuration(id);
}

#else

#include <audio/include/SimpleAudioEngine.h>
using namespace CocosDenshion;

static float m_vol = 1.0f;

void MySound::playMusic(const char* filename, bool bLoop)
{
	SimpleAudioEngine::getInstance()->playBackgroundMusic(filename, bLoop);
	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(m_vol);
}

void MySound::stopMusic()
{
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void MySound::pauseMusic()
{
	SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void MySound::resumeMusic()
{
	SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

int MySound::playEffect(const char* pszFilePath, bool bLoop,
	float pitch, float pan)
{
	return SimpleAudioEngine::getInstance()->playEffect(pszFilePath, bLoop, pitch, pan, m_vol);
}

void MySound::stopEffect(int& id)
{
	SimpleAudioEngine::getInstance()->stopEffect(id);
}

void MySound::setVolume(float vol)
{
	m_vol = vol;
}

#endif // USE_NEW_CC_AUDIO
