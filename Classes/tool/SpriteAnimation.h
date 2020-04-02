#pragma once
#include "cocos2d.h"
USING_NS_CC;


class SpriteAnimation : public Node
{
public:
	static SpriteAnimation* createAni(const std::string &picName, int frames, int rows, int cols, float interval = 0.2f, const std::string &aniCacheName = "");
	
	void play(bool bRepeat, int nDelay = 0, float* arrDelay = nullptr);
	void stopAt(int index = -1); //-1为最后一帧
	bool isDone();

	Sprite* getAni();

protected:
	bool init(const std::string &picName, int frames, int rows, int cols, float interval, const std::string &aniCacheName);

	Sprite*		m_ani;
// 	Vector<SpriteFrame*>	m_frames;
// 	float		m_interval;	
	std::string	m_strAniCacheName;
};


struct tagAnimationInfo
{
	tagAnimationInfo(const char* _file, int _frames, int _rows, int _cols)
	{
		file = _file;
		frames = _frames;
		rows = _rows;
		cols = _cols;
	}
	const char* file;
	int frames;
	int rows;
	int cols;
};