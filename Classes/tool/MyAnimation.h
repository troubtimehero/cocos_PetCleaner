#pragma once
#include <cocos2d.h>
USING_NS_CC;

class MyAnimation
{
public:
	MyAnimation();
	~MyAnimation();

	//【注】此版本的Dictionary排序有问题，在这里纠正
	static Animation* create(std::string plist, float spf = 0.1f);
};

