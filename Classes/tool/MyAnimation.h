#pragma once
#include <cocos2d.h>
USING_NS_CC;

class MyAnimation
{
public:
	MyAnimation();
	~MyAnimation();

	//��ע���˰汾��Dictionary���������⣬���������
	static Animation* create(std::string plist, float spf = 0.1f);
};

