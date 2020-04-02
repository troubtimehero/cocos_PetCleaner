#pragma once
// #include <hge.h>
#include <cocos2d.h>
#include "tool/MyTools.h"

class CGameMusic
{
public:
	CGameMusic();
	~CGameMusic();

	void init();
	void play();	//��������ʱ�䳤��(��)
	bool isPlaying();
	void stop();

private:
	int	m_hChannel;

	std::vector<std::string>	m_lstFile;
	RandomController<int,1>*	m_pRcMusicIndex;
};