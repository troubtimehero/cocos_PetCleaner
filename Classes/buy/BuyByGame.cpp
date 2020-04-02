#include "BuyByGame.h"
#include "tool/MyTools.h"
#include "ui/CocosGUI.h"
#include "MachineDefined.h"
#include "GameData.h"
#include "ioCtrl/ComThread.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "ioCtrl/LeYaoYao.h"
#include "BuyScene.h"
#include "GiftWinning.h"
#include "GameLayer.h"
#include "tool/MyBtnTimeoutBack.h"

bool BuyByGame::init()
{
	if (!Layer::init())
	{
		return false;
	}

	this->setName(LAYER_NAME_WANT_GAME);

	auto cover = LayerColor::create(Color4B(0, 0, 0, 255), GAME_WIDTH, GAME_HEIGHT);
	this->addChild(cover);
	DisableBelowTouch(this);


	for (int i = 0; i < COUNT_BOX; ++i)
	{
		auto btn = m_btns[i] = ui::Button::create("image2/buy/enable.png", "image2/buy/selected.png", "image2/buy/disable.png");
		this->addChild(btn);

		POINT vh = box_GetPidBit(i);

		btn->setTitleText(GBKToUTF(StringUtils::format("%c%d\n%s", 'A' + vh.x, vh.y + 1, CentToChar(dataGet_GameCent(i)).c_str())));

		btn->setPosition(box_GetPosition(i));
		btn->setScale(box_GetSize(i).width, box_GetSize(i).height);
		Sprite* f = setBoxTitleFrame(btn, true);

		if (f)
		{
			f->setTag(DEF_TAG_ONSALE_FRAME + i);
		}

		updateBoxContain(i);

		btn->addClickEventListener([this, i](Ref* ref){
			this->callbackClickBtn(i);
		});
	}

	this->addChild(MyBtnTimeoutBack::MyCreate("image/menu/button_empty.png", CONTROL_TIMEOUT, this));

	scheduleUpdate();

	return true;
}

void BuyByGame::updateBoxContain(int index)
{
	bool bOn = dataGet_OnSale(index) > 0;
	m_btns[index]->setEnabled(bOn);
	Sprite* f = (Sprite*)m_btns[index]->getParent()->getChildByTag(DEF_TAG_ONSALE_FRAME + index);
	if (f)
	{
		f->setVisible(bOn);
	}
}

void BuyByGame::callbackClickBtn(int index)
{
	// 	devOut_score = 0,	//得分灯
	// 	devOut_ticket = 1,	//退票灯
	// 	devOut_power = 2,	//加力灯
	// 	devOut_fire = 3		//发射灯

	//防止其他位置不停电
	if (g_pIoCtrlLayer->isLockOpening())
		return;

	POINT vh = box_GetPidBit(index);
	//币不够，显示提示
	if (ShowInsertMoreCoin(this, GBKToUTF(StringUtils::format(IsChinese() ? "游戏获取 %c%d" : "Win %c%d by Game", 'A' + vh.x, 1 + vh.y)), dataGet_GameCent(index) - dataGet_RemainCent(), this, LAYER_NAME_PAY))
	{
		return;
	}

	m_wantIndex = index;

	//扣币
	g_pIoCtrlLayer->useCoinByGame(index);

	//开始游戏
	this->addChild(GameLayer::MyCreate(index));	//游戏创建时计算是否可得奖（奖品、币数）
}

void BuyByGame::callWin()
{
	//开门
	MySound::playEffect("sound/a_win.mp3");
	g_pBox->BoxReportStatus(io_LYY_Good::eBoxStatus::e_box_free);
	//上报游戏交易结果（有）
	g_pLeyaoyao->SetTradeTemp(m_wantIndex, true);

	auto gw = GiftWinning::create();
	this->addChild(gw);
	gw->setName("GiftWinning");
	gw->mySetGiftPoint(m_wantIndex);
	
	this->updateBoxContain(m_wantIndex);
}

void BuyByGame::callbackPayOK()
{
	if (m_wantIndex >= 0)
		callbackClickBtn(m_wantIndex);
}

void BuyByGame::callbackCancel()
{
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), RemoveSelf::create()));
}

void BuyByGame::update(float delta)
{
	MyBtnTimeoutBack* b = (MyBtnTimeoutBack*)this->getChildByName(BUTTON_NAME_BACK_TIMEOUT);
	if (b)
	{
		b->enableTimeout(!getChildByName(LAYER_NAME_GAME_ING) && !getChildByName(LAYER_NAME_PAY));
	}
}
