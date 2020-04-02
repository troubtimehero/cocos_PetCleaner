#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "tool/SceneMgr.h"

class GoodsGettingTypeScene : public SceneMgr
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(GoodsGettingTypeScene);

	virtual bool init();
	virtual void update(float delta);

private:
	float		m_fTimeout;

};

