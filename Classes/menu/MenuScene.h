#pragma once

#include "cocos2d.h"
#include "tool/SceneMgr.h"

class Scene_Menu : public SceneMgr
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	
	virtual void onEnter();
	virtual void onExit();

	// implement the "static create()" method manually
	CREATE_FUNC(Scene_Menu);
};