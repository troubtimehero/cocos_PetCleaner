#include "MyButtonBase.h"
#include "tool/MyTools.h"


//////////////////////////////////////////////////////////////////////////

MyButtonInputNumber* MyButtonInputNumber::MyCreate(int iMin, int iMax, InputNumberOnMessage* target, const std::string& normalImage, const std::string& selectedImage /*= ""*/, const std::string& disableImage /*= ""*/, TextureResType texType /*= TextureResType::LOCAL*/)
{
	MyButtonInputNumber *btn = new (std::nothrow) MyButtonInputNumber(iMin, iMax, target);
	if (btn && btn->init(normalImage, selectedImage, disableImage, texType))
	{
		btn->autorelease();
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;

}
