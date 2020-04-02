#include "MainLayer.h"
#include "tool/MyTools.h"
#include "MachineDefined.h"
#include "GameData.h"
#include "WorkingLayer.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "ioCtrl/LeYaoYao.h"
#include "ioCtrl/ComThread.h"
#include "buy/GoodsGettingTypeScene.h"


bool MainLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}


	//触摸监测
	auto listenerTouch = EventListenerTouchOneByOne::create();
	listenerTouch->onTouchBegan = [this](Touch *, Event *){
		this->m_fLightTime = 15;
		lightNormalOn(true);
		return false;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, this);


	//提示
	auto tips1 = Label::createWithSystemFont(LanguageChoose("提示1：点开关选择过程", "Tips1: choose program"), "黑体", 50);
	tips1->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
	tips1->setPosition(10, GAME_HEIGHT - 10);
	this->addChild(tips1);

	auto tips2 = Label::createWithSystemFont(LanguageChoose("提示2：用滑条设定时间", "Tips2: select work time"), "黑体", 50);
	tips2->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
	tips2->setPosition(GAME_WIDTH - 10, GAME_HEIGHT - 10);
	this->addChild(tips2);

	auto tips3 = Label::createWithSystemFont(LanguageChoose("提示3：点\"开始\"键开始", "Tips3: start by \"start\" button"), "黑体", 50);
	tips3->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
	tips3->setPosition(GAME_WIDTH - 10, 0);
	this->addChild(tips3);



	const float cCheckBoxX = 350;
	const float cWashY = GAME_HEIGHT * 4 / 5;
	const float cDryY = GAME_HEIGHT * 3 / 5;
	const char* fontName = "Arral";
	const int fontSize = 90;
	const Vec2 vecRadioOff = Vec2(0, 20);
	const Vec2 vecNameOff = Vec2(50, 0);


	//冲洗
	ui::CheckBox* checkWash = ui::CheckBox::create("image/menu/check_box_normal.png", "image/menu/check_box_active.png");
	this->addChild(checkWash);
	checkWash->setSelected(true);
	checkWash->addEventListener([=](Ref* ref, ui::CheckBox::EventType event){
		m_bWash = (event == ui::CheckBox::EventType::SELECTED);
		calNeedPay();
	});
	checkWash->setPosition(Point(cCheckBoxX, GAME_HEIGHT / 5 * 4) + vecRadioOff);

	Label* lWash = Label::createWithSystemFont(LanguageChoose("清洗", "Wash"), fontName, fontSize);
	checkWash->addChild(lWash);
	lWash->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
	lWash->setPosition(-200, checkWash->getContentSize().height/2);
	lWash->setTextColor(Color4B::BLUE);
	lWash->enableShadow(Color4B::WHITE);
	


	//烘干
	ui::CheckBox* checkDry = ui::CheckBox::create("image/menu/check_box_normal.png", "image/menu/check_box_active.png");
	this->addChild(checkDry);
	checkDry->setSelected(true);
	checkDry->addEventListener([=](Ref* ref, ui::CheckBox::EventType event){
		m_bDry = (event == ui::CheckBox::EventType::SELECTED);
#ifdef BLOW_CAN_COLD_WIND
		m_checkBoxHot->setEnabled(m_bDry);
#endif // BLOW_CAN_COLD_WIND
		calNeedPay();
	});
	checkDry->setPosition(Point(cCheckBoxX, GAME_HEIGHT / 5 * 3) + vecRadioOff);

	Label* lDry = Label::createWithSystemFont(LanguageChoose("烘干", "Dry"), fontName, fontSize);
	checkDry->addChild(lDry);
	lDry->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
	lDry->setPosition(-200, checkDry->getContentSize().height / 2);
	lDry->setTextColor(Color4B::RED);
	lDry->enableShadow(Color4B::WHITE);


	m_bWash = true;
	m_bDry = true;



	//加热
#ifdef BLOW_CAN_COLD_WIND
	m_checkBoxHot = ui::CheckBox::create("image/menu/check_box_normal.png", "image/menu/check_box_active.png");
	this->addChild(m_checkBoxHot);
	m_checkBoxHot->setSelected(true);
	m_checkBoxHot->addEventListener([=](Ref* ref, ui::CheckBox::EventType event){
		calNeedPay();
	});
	m_checkBoxHot->setPosition(Point(cCheckBoxX, GAME_HEIGHT / 5 * 3));

	Label* lHot = Label::createWithSystemFont(LanguageChoose("热风", "Hot Wind"), fontName, fontSize);
	m_checkBoxHot->addChild(lHot);
	lHot->setPosition(-200, m_checkBoxHot->getContentSize().height / 2);
#endif // BLOW_CAN_COLD_WIND



	//时间（短、中、长）
#ifdef CAN_CHOOSE_WORK_TIME
	//清洗加时滑块
	m_slAddWash = ui::Slider::create();
	m_slAddWash->loadBarTexture("image/menu/slider_back.png");
	m_slAddWash->loadSlidBallTextures("image/menu/slider_pin.png");
	m_slAddWash->loadProgressBarTexture("image/menu/slider_front.png");
	m_slAddWash->setPosition(Vec2(850, cWashY + 25));
	m_slAddWash->setPercent(0);
	//回调函数，带两个参数，每改变百分比的时候回调用一次
	m_slAddWash->addEventListener(CC_CALLBACK_2(MainLayer::SliderCallBack, this));
	m_slAddWash->setTag(0);
	this->addChild(m_slAddWash);

	//烘干加时滑块
	m_slAddDry = ui::Slider::create();
	m_slAddDry->loadBarTexture("image/menu/slider_back.png");
	m_slAddDry->loadSlidBallTextures("image/menu/slider_pin.png");
	m_slAddDry->loadProgressBarTexture("image/menu/slider_front.png");
	m_slAddDry->setPosition(Vec2(850, cDryY + 25));
	m_slAddDry->setPercent(0);
	//回调函数，带两个参数，每改变百分比的时候回调用一次
	m_slAddDry->addEventListener(CC_CALLBACK_2(MainLayer::SliderCallBack, this));
	m_slAddDry->setTag(1);
	this->addChild(m_slAddDry);

	m_lTimeWash = Label::createWithSystemFont("", fontName, fontSize * 3 / 4);
	m_lTimeWash->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
	m_lTimeWash->setPosition(1000, cWashY - 40);
	this->addChild(m_lTimeWash);

	m_lTimeDry = Label::createWithSystemFont("", fontName, fontSize * 3 / 4);
	m_lTimeDry->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
	m_lTimeDry->setPosition(1000, cDryY - 40);
	this->addChild(m_lTimeDry);


	m_lTimeTotal = Label::createWithSystemFont("", fontName, fontSize * 3 / 4);
	m_lTimeTotal->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
	m_lTimeTotal->setPosition(Point(GAME_WIDTH - 230, 280));
	this->addChild(m_lTimeTotal);


	m_addTimeWash = 0;
	m_addTimeDry = 0;
#else

	ui::RadioButtonGroup* radioTimeGroup = ui::RadioButtonGroup::create();
	this->addChild(radioTimeGroup);

	vector<string> radioButtonName[2] = { { "短毛", "中毛", "长毛" }, { "Short", "Medium", "Long" } };

	for (int i = 0; i < 3; ++i)
	{
		ui::RadioButton* radioButton = ui::RadioButton::create("image/menu/radio_button_off.png", "image/menu/radio_button_on.png");
		radioButton->setPosition(Point(GAME_WIDTH / 6, GAME_HEIGHT / 5 * (4 - i)) + vecRadioOff);
		radioButton->addEventListener(CC_CALLBACK_2(MainLayer::onSelectTime, this));
		radioButton->setTag(i);
		radioTimeGroup->addRadioButton(radioButton);
		this->addChild(radioButton);

		auto label = Label::createWithSystemFont(LanguageChoose(radioButtonName[0].at(i).c_str(), radioButtonName[1].at(i).c_str()), fontName, fontSize);
		this->addChild(label);
		label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
		label->setPosition(radioButton->getPosition() + vecNameOff);
	}
	radioTimeGroup->setSelectedButton(0);

	m_iHairIndex = 0;
	m_nWashTime = g_pData->get(eDataKey::set_PetTimeWash_S + m_iHairIndex);	//获取后台设置的时间（分）
	m_nDryTime = g_pData->get(eDataKey::set_PetTimeBlow_S + m_iHairIndex);
#endif // CAN_CHOOSE_WORK_TIME



	m_btnStart = ui::Button::create("image/tips/start_normal.png", "image/tips/start_selected.png", "image/tips/start_disable.png");
	this->addChild(m_btnStart);
	m_btnStart->setPosition(VisibleRect::rightBottom() + Vec2(-m_btnStart->getContentSize().width * 0.55f, m_btnStart->getContentSize().height * 0.55f + 100));
	m_btnStart->setTitleFontSize(53);
	m_btnStart->setTitleText(LanguageChoose("\n\n开始", "\n\nStart"));
	m_btnStart->setTitleColor(Color3B::BLACK);
	m_btnStart->addClickEventListener([this](Ref*){
		lightNormalOn(true);

		if (g_pIoCtrlLayer->isKeyKeep(IoCtrlLayer::e_key_fire))	//已经关好门了
		{
			this->calNeedPay();

			static bool sbFastTest = (MyIniGetInt("SETTING", "FAST_TEST", 0) > 0);
			if (sbFastTest)
				m_nNeedPayCent = 1;

			if (!ShowInsertMoreCoin(this, LanguageChoose("请扫码付款", "Scan To Pay"), m_nNeedPayCent, this))
			{
				this->callbackPayOK();
			}
		}
		else if(!getChildByName("tipsCloseDoor")) //未关好门
		{
			LayerColor* l = LayerColor::create(Color4B(255, 0, 0, 234), GAME_WIDTH - 50, GAME_HEIGHT / 2);
			this->addChild(l);
			l->setPosition(25, GAME_HEIGHT / 4);
			l->runAction(Sequence::createWithTwoActions(DelayTime::create(2.5f), RemoveSelf::create()));
			l->setName("tipsCloseDoor");

			Label* tipsCloseDoor = createFontTTF("fonts/dai meng wa wa ti.ttf", IsChinese() ? 80 : 60, 5, Color4B::WHITE, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
			l->addChild(tipsCloseDoor);
			tipsCloseDoor->setString(LanguageChoose("请把门关好再开始", "Please Close The Door\nBefore Start"));
			tipsCloseDoor->setPosition(l->getContentSize() / 2);
		}
	});



#if 1
	if (g_pData->get(eDataKey::set_saleMode) > 0)
	{
		//buy
		ui::Button* btnBuy = ui::Button::create("image2/buy/btn_goods.png");
		this->addChild(btnBuy);
		btnBuy->setPosition(VisibleRect::leftBottom() + Vec2(btnBuy->getContentSize().width / 2, btnBuy->getContentSize().height / 2));
		btnBuy->addClickEventListener([this](Ref* r){
			Director::getInstance()->replaceScene(HorizontalScreen(GoodsGettingTypeScene::createScene()));
		});

		auto nameBuy = createButtonTTF(btnBuy, "fonts/dai meng wa wa ti.ttf", 75, 3, Color4B::YELLOW, Color4B::RED, Color4B(74, 0, 0, 255));
		nameBuy->setString(GBKToUTF(string(IsChinese() ? "货品" : "Goods")));
	}
#endif


	//提示所需价格
	m_lNeedCent = Label::createWithSystemFont("", fontName, fontSize);
	this->addChild(m_lNeedCent);
	m_lNeedCent->setPosition(m_btnStart->getPosition() + Vec2(-m_btnStart->getContentSize().width * 0.6f, -40));
	m_lNeedCent->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
	m_lNeedCent->enableShadow(Color4B::YELLOW);


	calNeedPay();
	scheduleUpdate();

	return true;
}

void MainLayer::update(float delta)
{
	if (!this->getChildByName("work"))
	{
		m_fLightTime -= delta;
		if (m_fLightTime <= 0)
		{
			lightNormalOn(false);
		}
	}
}

void MainLayer::callbackPayOK()
{
#if 0
	if (this->getChildByName("work"))
	{
		g_pCom->CloseAllDev();
		this->getChildByName("work")->removeFromParent();
		return;
	}
#endif


	//清洗记录
#ifdef CAN_CHOOSE_WORK_TIME

	if (m_nWashTime > 0 && m_nDryTime > 0)
	{
		g_pLeyaoyao->UpPetTrade(0, m_nNeedPayCent);
	}
	else if (m_nWashTime > 0)
	{
		g_pLeyaoyao->UpPetTrade(1, m_nNeedPayCent);
	}
	else if (m_nDryTime > 0)
	{
		g_pLeyaoyao->UpPetTrade(2, m_nNeedPayCent);
	}

#else

	//计算时间
	m_nWashTime = m_bWash ? g_pData->get(eDataKey::set_PetTimeWash_S + m_iHairIndex) : 0;	//获取后台设置的时间（分）
	m_nDryTime = m_bDry ? g_pData->get(eDataKey::set_PetTimeBlow_S + m_iHairIndex) : 0;

	if (m_nWashTime > 0 && m_nDryTime > 0)
	{
		g_pData->increase(eDataKey::note_clean_WB_S + m_iHairIndex, 1);
	}
	else if (m_nWashTime > 0)
	{
		g_pData->increase(eDataKey::note_clean_W_S + m_iHairIndex, 1);
	}
	else if (m_nDryTime > 0)
	{
		g_pData->increase(eDataKey::note_clean_B_S + m_iHairIndex, 1);
	}

#endif // CAN_CHOOSE_WORK_TIME



	WorkingLayer* work = WorkingLayer::create();
	this->addChild(work);
	DisableBelowTouch(work);

	static bool sbFastTest = (MyIniGetInt("SETTING", "FAST_TEST", 0) > 0);
	if (sbFastTest)
		work->Start(m_nWashTime, m_nDryTime, true);
	else
		work->Start(m_nWashTime * 60, m_nDryTime * 60, true);
	
	work->setName("work");
}

void MainLayer::calNeedPay()
{
	//计算价格
	m_nNeedPayCent = 0;
	
#ifdef CAN_CHOOSE_WORK_TIME
	//此处的是角
	int nPayWash = g_pData->get(eDataKey::set_PetWash_BasePrice) + g_pData->get(eDataKey::set_PetWash_IncPrice) * m_addTimeWash;
	int nPayDry = g_pData->get(eDataKey::set_PetDry_BasePrice) + g_pData->get(eDataKey::set_PetDry_IncPrice) * m_addTimeDry;

	if (m_bWash)
		m_nNeedPayCent += nPayWash * 10;
	if (m_bDry)
		m_nNeedPayCent += nPayDry * 10;

	m_slAddWash->setEnabled(m_bWash);
	m_slAddDry->setEnabled(m_bDry);
	m_slAddWash->setColor(m_bWash ? Color3B::WHITE : Color3B::GRAY);
	m_slAddDry->setColor(m_bDry ? Color3B::WHITE : Color3B::GRAY);


	int nTimeWash = g_pData->get(eDataKey::set_PetWash_BaseTime) + m_addTimeWash;
	int nTimeDry = g_pData->get(eDataKey::set_PetDry_BaseTime) + m_addTimeDry;
	m_lTimeWash->setString(LanguageChoose("时长:%d", "Time:%d", nTimeWash));
	m_lTimeDry->setString(LanguageChoose("时长:%d", "Time:%d", nTimeDry));
	m_lTimeWash->setTextColor(m_bWash ? Color4B::WHITE : Color4B::GRAY);
	m_lTimeDry->setTextColor(m_bDry ? Color4B::WHITE : Color4B::GRAY);


	m_nWashTime = m_bWash ? nTimeWash : 0;
	m_nDryTime = m_bDry ? nTimeDry : 0;
	m_lTimeTotal->setString(LanguageChoose("总时长: %d 分钟", "Total Time: %d minute", m_nWashTime + m_nDryTime));
	m_lTimeTotal->setTextColor((m_bWash || m_bDry) ? Color4B::YELLOW : Color4B::GRAY);
#else

	if (m_bWash && m_bDry)
		m_nNeedPayCent = g_pData->get(eDataKey::set_PetPriceAll_S + m_iHairIndex);		//洗+吹 价格
	else if (m_bWash)
		m_nNeedPayCent += g_pData->get(eDataKey::set_PetPriceWash_S + m_iHairIndex);	//洗澡价格
	else if (m_bDry)
		m_nNeedPayCent += g_pData->get(eDataKey::set_PetPricBlowe_S + m_iHairIndex);	//烘干价格
#endif // CAN_CHOOSE_WORK_TIME

	m_btnStart->setEnabled(m_bWash || m_bDry);

	m_lNeedCent->setString(LanguageChoose("价格:%s", "Pay:%s", CentToChar(m_nNeedPayCent).c_str()));
	m_lNeedCent->setTextColor((m_bWash || m_bDry) ? Color4B::RED : Color4B::GRAY);
}

#ifdef CAN_CHOOSE_WORK_TIME
void MainLayer::SliderCallBack(Ref *pSender, ui::Slider::EventType type)
{
	auto item = (ui::Slider*)(pSender);
	switch (type)
	{
	case cocos2d::ui::Slider::EventType::ON_SLIDEBALL_UP:
		MySound::playEffect("sound/Button.mp3");
		//不 break
	case cocos2d::ui::Slider::EventType::ON_PERCENTAGE_CHANGED:
		switch (item->getTag())
		{
		case 0:{
				int last = m_addTimeWash;
				m_addTimeWash = g_pData->get(eDataKey::set_PetWash_IncTime) * item->getPercent() / 100;
				if (m_addTimeWash != last)
					calNeedPay();
			}
			break;

		case 1:{
				int last = m_addTimeDry;
				m_addTimeDry = g_pData->get(eDataKey::set_PetDry_IncTime) * item->getPercent() / 100;
				if (m_addTimeDry != last)
					calNeedPay();
			}
			break;
		}
		break;
	}
}

#else

void MainLayer::onSelectTime(ui::RadioButton* radioButton, ui::RadioButton::EventType type)
{
	if (nullptr == radioButton)
		return;

	switch (type)
	{
	case cocos2d::ui::RadioButton::EventType::SELECTED:
		m_iHairIndex = radioButton->getTag();
		break;
	default:
		break;
	}

	calNeedPay();
}
#endif // CAN_CHOOSE_WORK_TIME
