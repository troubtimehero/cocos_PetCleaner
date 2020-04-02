#include "GiftWinning.h"
#include "ioCtrl/ComThread.h"
#include "tool/MyTools.h"
#include "MachineDefined.h"
#include "GameData.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "ioCtrl/LeYaoYao.h"


//////////////////////////////////////////////////////////////////////////

bool GiftWinning::init()
{
	LayerColor::initWithColor(Color4B::WHITE);

	DisableBelowTouch(this);

	this->runAction(Sequence::createWithTwoActions(DelayTime::create(10), RemoveSelf::create()));

	MySound::playEffect("voice/win.mp3");

	return true;
}

void GiftWinning::mySetGiftPoint(int index)
{
	//来到这里，应该没有锁是正在开的，不加开锁判断也行
	POINT vh = box_GetPidBit(index);


	//开门
	g_pIoCtrlLayer->openLock(vh.x, vh.y);
	dataSet_DecreaseContain(index);
	dataSet_RecordOpenDoor(vh.x, vh.y);


	auto win = createFontTTF("fonts/dai meng wa wa ti.ttf", 100, 10, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
	this->addChild(win);
	win->setString(LanguageChoose("恭喜中奖", "Congratulations"));
	win->setPosition(GAME_WIDTH / 2, GAME_HEIGHT * 2 / 3);
	win->runAction(RepeatForever::create(Sequence::createWithTwoActions(ScaleTo::create(0.5f, 1.1f), ScaleTo::create(0.5f, 1))));

#if 0
	auto number = createFontTTF("fonts/ThonburiBold.ttf", 100, 10, Color4B(255, 242, 145, 255), Color4B(144, 105, 188, 255), Color4B(197, 167, 255, 180), true);
	win->addChild(number);
	number->setString(StringUtils::format("%c %d", 'A'+vh.x, vh.y+1));
	number->setPosition(win->getLetter(9)->getPositionX(), win->getLetter(7)->getPositionY());
#endif
}


