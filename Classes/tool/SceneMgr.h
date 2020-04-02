#pragma once

#include "cocos2d.h"

class SceneMgr : public cocos2d::Layer
{
public:
	SceneMgr(){}
	~SceneMgr(){};
	virtual void onEnter();
	virtual void onExit();

	void captureScreen();
	void captureScreen2();
	void afterCapture(bool succeed, const std::string& outputFile);
};

#define USE_SCENE_MGR