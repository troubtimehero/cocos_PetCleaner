#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "MachineDefined.h"
#include "tool/SceneMgr.h"

class OpenOneByOne : public SceneMgr
{
public:
	static Scene* createScene();

	CREATE_FUNC(OpenOneByOne);

	bool init();

private:
	void timerOpenNext(float delta);

	Sprite*	m_spr[COUNT_BOX];
	int		m_curIndex;
	int		m_step;
};
