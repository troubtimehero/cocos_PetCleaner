#include "DevSingleTest.h"
#include "ioCtrl/ComThread.h"
#include "MachineDefined.h"
#include "tool/MyTools.h"
#include "helloScene/WorkingLayer.h"

//////////////////////////////////////////////////////////////////////////
static CComThread::tagPidBit sDevSingle[] = { DEV_WORKING_LOTION, DEV_WORKING_WASH, DEV_WORKING_BLOW, DEV_WORKING_HOT, DEV_LIGHT_NORMAL, DEV_LIGHT_PURPLE };
static const char* sDevName[] = { "洗洁精", "清洗", "吹风", "加热", "照明", "紫外灯" };

bool DevSingleTest::init()
{
	if (!LayerColor::initWithColor(MENU_BACKGROUND_COLOR, GAME_WIDTH, GAME_HEIGHT))
	{
		return false;
	}
	DisableBelowTouch(this);

	g_pCom->CloseAllDev();

	m_lBtns = Layer::create();
	this->addChild(m_lBtns);

	for (int i = 0; i < 6; ++i)
	{
		m_bOn[i] = false;

		auto btn = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
		btn->setTag(i);
		btn->setPosition(Vec2(GAME_WIDTH/4 * (i%3+1), GAME_HEIGHT/3 * (i/3+1)));
		btn->setTitleFontSize(40);
		btn->setTitleText(GBKToUTF(string(sDevName[i])));
		btn->addClickEventListener([this](Ref* ref){
			ui::Button* b = (ui::Button*)ref;
			int tag = b->getTag();
			this->m_bOn[tag] = !this->m_bOn[tag];
			g_pCom->SetDevOut(sDevSingle[tag].pid, sDevSingle[tag].bit, this->m_bOn[tag]);
			b->setColor(this->m_bOn[tag] ? Color3B::RED : Color3B::WHITE);
		});
		m_lBtns->addChild(btn);
	}


	auto btnBack = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnBack->setPosition(VisibleRect::leftBottom() + Vec2(100, 30));
	btnBack->setTitleFontSize(38);
	btnBack->addClickEventListener([this](Ref*){
		this->removeFromParent();
	});
	btnBack->setTitleText("back");
	this->addChild(btnBack);


	initPassword();	//装隐藏原按键，输入密码后再显示

	return true;
}

void DevSingleTest::initPassword()
{
	m_lBtns->setVisible(false);

	auto lPasswordLayer = Node::create();
	this->addChild(lPasswordLayer);

	Label* tips = Label::createWithSystemFont(LanguageChoose("请输入密码", "Please Enter Password"), "黑体", 60);
	tips->setPosition(GAME_WIDTH / 2, GAME_HEIGHT * 3 / 4);
	lPasswordLayer->addChild(tips);

	m_password = Label::createWithSystemFont("", "黑体", 60);
	m_password->setPosition(VisibleRect::center());
	lPasswordLayer->addChild(m_password);


	//----------------------------数字
	for (int i = 0; i < 10; ++i)
	{
		ui::Button* num = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
		lPasswordLayer->addChild(num);
		num->setTitleFontSize(80);
		num->setTitleText(StringUtils::format("%d", i));
		num->setTitleColor(Color3B::BLUE);
		num->setPosition(Point(GAME_WIDTH / 11 * (i + 1), GAME_HEIGHT / 6 * 2));
		num->addClickEventListener([this, i](Ref* ref){
			this->onBtnNumber(i);
		});
	}

	//----------------------------回删
	ui::Button* del = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
	lPasswordLayer->addChild(del);
	del->setTitleFontSize(60);
	del->setTitleText("del");
	del->setTitleColor(Color3B::RED);
	del->setPosition(Size(GAME_WIDTH / 2, GAME_HEIGHT / 8));
	del->addClickEventListener([this](Ref* ref){
		this->onBtnDel();
	});
}

void DevSingleTest::onBtnNumber(int num)
{
	string str = m_password->getString();
	if (str.length() < 20)
	{
		str.push_back('0' + num);
		m_password->setString(str);

		if (m_password->getString() == "20120631")
		{
			//隐藏密码层
			m_password->getParent()->removeFromParent();
			//显示操作层
			m_lBtns->setVisible(true);
		}
	}
}

void DevSingleTest::onBtnDel()
{
	string str = m_password->getString();
	if (str.length() > 0)
	{
		str.pop_back();
		m_password->setString(str);
	}
}
