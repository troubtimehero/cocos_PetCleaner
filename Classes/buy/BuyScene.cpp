#include "BuyScene.h"
#include "tool/MyTools.h"
#include "GameData.h"
#include "ioCtrl/ComThread.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "ioCtrl/KnobSmooth.h"
#include "ioCtrl/LeYaoYao.h"
#include "helloScene/HelloWorldScene.h"

Scene* BuyLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = BuyLayer::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool BuyLayer::init()
{
	if (!Layer::init())	// cyhol 透明层 [9/20/2017 pc-ah]
		return false;

	this->setName(LAYER_NAME_BUY);

	auto bg = Sprite::create("image2/buy/background.png");
	bg->setPosition(VisibleRect::center());
	this->addChild(bg);
	bg->setContentSize(VisibleRect::size());

	//-------------------- 屏蔽下层触摸 -----------------------
	DisableBelowTouch(this);


	//-------------------------- IconRoll --------------------------
	for (int i = 0; i < COUNT_BOX; ++i)
	{
		auto btn = m_btns[i] = ui::Button::create("image2/buy/enable.png", "image2/buy/selected.png", "image2/buy/disable.png");
		this->addChild(btn);

		POINT vh = box_GetPidBit(i);

		btn->setTitleText(GBKToUTF(StringUtils::format("%c%d\n%s", 'A' + vh.x, vh.y + 1, CentToChar(dataGet_BuyCent(i)).c_str())));

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

	//-------------------------- back --------------------------

	this->addChild(MyBtnTimeoutBack::MyCreate("image/menu/button_empty.png", CONTROL_TIMEOUT, this));
	
	scheduleUpdate();
	return true;
}

void BuyLayer::update(float delta)
{
// 	updateComPressBtn();
}

void BuyLayer::callbackClickBtn(int index)
{
// 	devOut_score = 0,	//得分灯
// 	devOut_ticket = 1,	//退票灯
// 	devOut_power = 2,	//加力灯
// 	devOut_fire = 3		//发射灯
	
	MyBtnTimeoutBack* b = (MyBtnTimeoutBack*)this->getChildByName(BUTTON_NAME_BACK_TIMEOUT);
	if (b)
	{
		b->resetTimeout(CONTROL_TIMEOUT);
	}

	//防止其他位置不停电
	if (g_pIoCtrlLayer->isLockOpening())
		return;

	POINT vh = box_GetPidBit(index);
	//币不够，显示提示
	if (ShowInsertMoreCoin(this, GBKToUTF(StringUtils::format(IsChinese() ? "购买 %c%d" : "Buy %c%d", 'A'+vh.x, 1+vh.y)), dataGet_BuyCent(index) - dataGet_RemainCent(), this))
	{
		m_iWwant = index;
		return;
	}

	//扣币
	dataUse_OutEgg(index, 1);


	//开门
	g_pIoCtrlLayer->openLock(vh.x, vh.y);
	dataSet_DecreaseContain(index);
	dataSet_RecordOpenDoor(vh.x, vh.y);
	//上报交易结果
	g_pLeyaoyao->SetTradeTemp(index, false);


	updateBoxContain(index);
}

void BuyLayer::callbackPayOK()
{
	callbackClickBtn(m_iWwant);
}

void BuyLayer::updateBoxContain(int index)
{
	bool bOn = dataGet_OnSale(index) > 0;
	m_btns[index]->setEnabled(bOn);
	Sprite* f = (Sprite*)m_btns[index]->getParent()->getChildByTag(DEF_TAG_ONSALE_FRAME + index);
	if (f)
	{
		f->setVisible(bOn);
	}
}

void BuyLayer::callbackTimeoutBack()
{
	if (g_pData->get(eDataKey::set_GameCent) > 0)
		this->removeFromParent();
	else
		Director::getInstance()->replaceScene(HorizontalScreen(Scene_Game::createScene()));
}

