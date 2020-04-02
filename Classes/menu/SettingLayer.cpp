#include "SettingLayer.h"
#include "tool/MyTools.h"
#include "tool/MyMenuBase.h"
#include "RecordLayer.h"
#include "helloScene/HelloWorldScene.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "SubSettingLayer.h"
//////////////////////////////////////////////////////////////////////////
string getSaleModeName(int d)
{
	string str;
	if (d > 0)
		str = LanguageChoose("是", "Yes");
	else
		str = LanguageChoose("否", "No");
	return str;
}

string getCentName(int d)
{
	string str;
	if (d > 0)
		str = CentToChar(d);
	else
		str = LanguageChoose("禁止", "Limit");
	return str;
}

bool SettingLayer::init()
{
	if (!LayerColor::initWithColor(MENU_BACKGROUND_COLOR, GAME_WIDTH, GAME_HEIGHT))
	{
		return false;
	}


#ifdef USE_KNOB
	m_bLockItem = false;
#endif // USE_KNOB

	//-------------------------- menu ---------------------
	MenuItemFont::setFontSize(68);

	//static menu
	{
		auto cleanRecord = MyMenuStaticLang::create("Clean Record", "清洗记录");
		auto saleMode = MyMenuStaticLang::create("Sale Mode", "售货模式");
		auto gameCoin = MyMenuStaticLang::create("Game Coin", "游戏币数");
		auto buyCoin = MyMenuStaticLang::create("Buy Price", "购买价格");
		auto addGift = MyMenuStaticLang::create("Add Gfit", "补货卸货");
		auto openTest = MyMenuStaticLang::create("Open Test", "出货测试");
		auto openRecord = MyMenuStaticLang::create("Open Record", "出货记录");

		m_menuStatic = Menu::create(cleanRecord, saleMode, gameCoin, buyCoin, addGift, openTest, openRecord, nullptr);
		m_menuStatic->alignItemsVerticallyWithPadding(10);
		m_menuStatic->setEnabled(false);
		m_menuStatic->setColor(Color3B(120, 120, 120));
	}


	//can change
	{
		auto cleanRecord = MenuItemFont::create("SET", CC_CALLBACK_1(SettingLayer::menuCallback, this));
		cleanRecord->setTag(e_tag_cleanRecord);


		auto saleMode = MenuItemFont::create(
			getSaleModeName(g_pData->get(eDataKey::set_saleMode)),
			CC_CALLBACK_1(SettingLayer::menuCallback, this));
		saleMode->setTag(e_tag_saleMode);


		auto gameCent = MenuItemFont::create(
			getCentName(dataGet_GameCent()),
			CC_CALLBACK_1(SettingLayer::menuCallback, this));
		gameCent->setTag(e_tag_gameCoin);


		auto buyCent = MenuItemFont::create("SET", CC_CALLBACK_1(SettingLayer::menuCallback, this));
		buyCent->setTag(e_tag_buyCoin);


		auto addGift = MenuItemFont::create("SET", CC_CALLBACK_1(SettingLayer::menuCallback, this));
		addGift->setTag(e_tag_addGift);


		auto openTest = MenuItemFont::create("SET", CC_CALLBACK_1(SettingLayer::menuCallback, this));
		openTest->setTag(e_tag_openTest);


		auto openRecord = MenuItemFont::create("SET", CC_CALLBACK_1(SettingLayer::menuCallback, this));
		openRecord->setTag(e_tag_openRecord);

		//禁止开启售卖模式时，全部按钮都要禁用
		if(0 == MyIniGetInt("SETTING", "ENABLE_SALE", 0))
		{
			saleMode->setEnabled(false);
// 			gameCent->setEnabled(false);
// 			buyCent->setEnabled(false);
// 			addGift->setEnabled(false);
// 			openTest->setEnabled(false);
// 			openRecord->setEnabled(false);
		}

		m_menu = Menu::create(cleanRecord, saleMode, gameCent, buyCent, addGift, openTest, openRecord, nullptr);
		m_menu->alignItemsVerticallyWithPadding(10);
	}

	this->addChild(m_menuStatic);
	m_menuStatic->setPosition(VisibleRect::size().width * 1 / 4, VisibleRect::size().height / 2);

	this->addChild(m_menu);
	m_menu->setPosition(VisibleRect::size().width * 3 / 4, VisibleRect::size().height / 2);

	//-------------------------- button --------------------------
	//back
	auto btnBack = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnBack->setPosition(VisibleRect::leftBottom() + Vec2(150, 45));
	btnBack->setTag(e_tag_back);
	btnBack->setTitleFontSize(38);
	btnBack->addClickEventListener([this](Ref*){
		this->onBtnBack();
	});
	addChild(btnBack);


	//record
	auto btnRecord = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnRecord->setPosition(VisibleRect::rightBottom() + Vec2(-150, 45));
	btnRecord->setTag(e_tag_record);
	btnRecord->setTitleFontSize(38);
	btnRecord->addClickEventListener([this](Ref*){
		this->onBtnRecord();
	});
	addChild(btnRecord);

	//-------------------------- music slider --------------------
// 	ui::Slider::create("image/menu/MusicIcon.png", "image/menu/MusicBack.png", ui::TextureResType::LOCAL);

	//创建一个滑块条
	auto sl = ui::Slider::create();
	sl->loadBarTexture("image/menu/MusicBack.png");
	sl->loadSlidBallTextures("image/menu/MusicIcon.png");
	sl->loadProgressBarTexture("image/menu/MusicFront.png");
	sl->setPosition(VisibleRect::top() + Vec2(0, -VisibleRect::size().height / 11));
	sl->setPercent(g_pData->get(eDataKey::set_Volume));
	//回调函数，带两个参数，每改变百分比的时候回调用一次  
	sl->addEventListener(CC_CALLBACK_2(SettingLayer::SliderCallBack, this));
	this->addChild(sl);


	m_pItems[12] = sl;
	m_pItems[13] = btnRecord;
	m_pItems[14] = btnBack;

	m_iCurIndex = 14;

	return true;
}

void SettingLayer::onEnter()
{
	KnobLayerBase::onEnter();
	updateMenuEnable();
	updateLanguage();

// 	scheduleUpdate();
}

void SettingLayer::menuCallback(cocos2d::Ref* sender)
{
	int tag = ((MenuItemFont*)sender)->getTag();
	switch (tag)
	{
	case e_tag_buyCoin:
		this->addChild(SettingBuyPrice::create());
		break;

	case e_tag_addGift:
		this->addChild(SettingSaleOnOff::create());
		break;

	case e_tag_openTest:
		this->addChild(SettingOpenTest::create());
		break;

	case e_tag_openRecord:
		this->addChild(SettingOpenRecord::create());
		break;

	case e_tag_cleanRecord:
		this->addChild(SettingCleanRecord::create());
		break;

	default:
		setMenuData((MenuItemFont*)sender, true);
		break;
	}
}

void SettingLayer::setMenuData(MenuItemFont* item, bool bIncrease)
{
	switch (item->getTag())
	{
	case e_tag_gameCoin:
		item->setString(getCentName(dataGet_NextSetting(eDataKey::set_GameCent, bIncrease)));
		break;

	case e_tag_saleMode:
		item->setString(getSaleModeName(dataGet_NextSetting(eDataKey::set_saleMode, bIncrease)));
		updateMenuEnable();
		break;
	}
}

void SettingLayer::updateMenuEnable()
{
	bool bEnable = (g_pData->get(eDataKey::set_saleMode) > 0);
	const int iTags[] = { e_tag_gameCoin, e_tag_buyCoin, e_tag_addGift, e_tag_openTest, e_tag_openRecord };
	for (int i = 0; i < ARRAYSIZE(iTags); ++i)
	{
		auto m = (MyMenuStaticLang*)m_menu->getChildByTag(iTags[i]);
		m->setEnabled(bEnable);
	}
}

void SettingLayer::onBtnRecord()
{
	static_cast<LayerMultiplex*>(this->_parent)->switchTo(2);
}

void SettingLayer::onBtnBack()
{
	static_cast<LayerMultiplex*>(this->_parent)->switchTo(0);
}

void SettingLayer::updateLanguage()
{
	for (const auto &child : m_menuStatic->getChildren())
	{
		auto c = (MyMenuStaticLang*)child;
		c->updateLanguage(IsChinese() ? MyMenuStaticLang::langCn : MyMenuStaticLang::langEn);
	}

	auto saleMode = (MyMenuStaticLang*)m_menu->getChildByTag(e_tag_saleMode);
	saleMode->setString(getSaleModeName(g_pData->get(eDataKey::set_saleMode)));


	auto gameCent = (MyMenuStaticLang*)m_menu->getChildByTag(e_tag_gameCoin);
	gameCent->setString(getCentName(dataGet_GameCent()));

	for (int i = e_tag_cleanRecord; i <= e_tag_openRecord; ++i)
	{
		auto m = (MyMenuStaticLang*)m_menu->getChildByTag(i);
		switch (i)
		{
		case e_tag_openRecord:
		case e_tag_cleanRecord:
			m->setString(GBKToUTF(string(IsChinese() ? "查看" : "Check")));
			break;
		default:
			m->setString(GBKToUTF(string(IsChinese() ? "设置" : "Set")));
			break;
		}
	}
	
	auto back = (ui::Button*)getChildByTag(e_tag_back);
	back->setTitleText(GBKToUTF(std::string(IsChinese() ? "返回" : "Back")));

	auto record = (ui::Button*)getChildByTag(e_tag_record);
	record->setTitleText(GBKToUTF(std::string(IsChinese() ? "账单" : "Bill")));
}

void SettingLayer::SliderCallBack(Ref *pSender, ui::Slider::EventType type)
{
	auto item = (ui::Slider*)(pSender);
	g_pData->set(eDataKey::set_Volume, item->getPercent());
	MySound::setVolume(item->getPercent() / 100.0f);
	if (type == ui::Slider::EventType::ON_SLIDEBALL_UP)
	{
		MySound::playEffect("sound/Button.mp3");
	}
}

#ifdef USE_KNOB
void SettingLayer::update(float delta)
{
	if (m_bLockItem)
	{
		switch (m_knob.getKey())
		{
		case KnobSmooth::e_key_l:
			updateItemData(false);
			break;
		case KnobSmooth::e_key_r:
			updateItemData(true);
			break;
		}
	}
	else
	{
		switch (m_knob.getKey())
		{
		case KnobSmooth::e_key_l:
			updateItemIndex(-1);
			break;

		case KnobSmooth::e_key_r:
			updateItemIndex(1);
			break;
		}
	}


	if (g_pIoCtrlLayer->isKeyDown(IoCtrlLayer::e_key_fire, 0.5f))
	{
		switch (m_iCurIndex)
		{
		case 14:
			onBtnBack();
			break;

		case 13:
			onBtnRecord();
			break;

		default:
			if (12 == m_iCurIndex && m_bLockItem)
			{
				MySound::setVolume(g_pData->get(eDataKey::set_Volume) / 100.0f);
				MySound::playEffect("sound/Button.mp3");
			}

			auto btn = dynamic_cast<MenuItemFont*>(m_pItems[m_iCurIndex]);
			if (btn && !btn->isEnabled())
				break;
			m_bLockItem = !m_bLockItem;
			m_pItems[m_iCurIndex]->setColor(m_bLockItem ? Color3B::RED : Color3B::YELLOW);
			m_pItems[m_iCurIndex]->setScale(m_bLockItem ? 1.3f : 1.1f);
			break;
		}
	}
}

void SettingLayer::updateItemData(bool bIncrease)
{
	switch (m_iCurIndex)
	{
	case 14:
	case 13:
		//按键无操作
		break;

	case 12:
		((ui::Slider*)m_pItems[m_iCurIndex])->setPercent(dataNextOfLimit(eDataKey::set_Volume, bIncrease, 0, 100, false));
		break;

	default:
		setMenuData((MenuItemFont*)m_pItems[m_iCurIndex], bIncrease);
		break;
	}
}

#endif // USE_KNOB

