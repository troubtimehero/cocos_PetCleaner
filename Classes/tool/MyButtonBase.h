#pragma once
#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
USING_NS_CC;
#include "InputNumber.h"


//////////////////////////////////////////////////////////////////////////
class MyButtonInputNumber : public ui::Button, public InputNumberGetting
{
	MyButtonInputNumber(int iMin, int iMax, InputNumberOnMessage* target) : InputNumberGetting(iMin, iMax, target) {}
public:
	static MyButtonInputNumber* MyCreate(int iMin, int iMax, InputNumberOnMessage* target,
		const std::string& normalImage,
		const std::string& selectedImage = "",
		const std::string& disableImage = "",
		TextureResType texType = TextureResType::LOCAL);
};
