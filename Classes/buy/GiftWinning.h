#pragma once
#include <cocos2d.h>
USING_NS_CC;

//////////////////////////////////////////////////////////////////////////

class GiftWinning : public LayerColor
{
public:
	CREATE_FUNC(GiftWinning);

	virtual bool init();

	void mySetGiftPoint(int index);
private:
};

