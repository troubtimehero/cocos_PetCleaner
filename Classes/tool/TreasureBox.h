#pragma once
#include <cocos2d.h>
USING_NS_CC;
#include "tool/SpriteAnimation.h"

class TreasureBox : public Node
{
public:
	TreasureBox();
	~TreasureBox();
	void init(const std::string &backName, const std::string &openName, int frames, int rows, int cols, float delay, Vec2 openOffset);

	void openBox(bool bWin, float hideDelay);
	void flowCoin(int nCoin);
private:
	Sprite*		m_back;
	SpriteAnimation*		m_open;
	Vector<SpriteFrame*> animFrames;
};

