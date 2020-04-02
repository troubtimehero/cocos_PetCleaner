#include "LayerPassword.h"
#include "tool/MyTools.h"
#include "GlobalDefined.h"
#include "MachineDefined.h"

//////////////////////////////////////////////////////////////////////////

LayerPassword* LayerPassword::MyCreate(LayerNeedPassword* caller, std::string password)
{
	LayerPassword* ret = new (std::nothrow) LayerPassword();
	if (ret && ret->init(caller, password))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}

	return ret;
}

bool LayerPassword::init(LayerNeedPassword* caller, std::string password)
{
	if (!LayerColor::initWithColor(MENU_BACKGROUND_COLOR, GAME_WIDTH, GAME_HEIGHT))
	{
		return false;
	}
	DisableBelowTouch(this);


	m_caller = caller;
	m_password = password;


	auto btnBack = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnBack->setPosition(VisibleRect::leftBottom() + Vec2(100, 30));
	btnBack->setTitleFontSize(38);
	btnBack->addClickEventListener([this](Ref*){
		this->removeFromParent();
	});
	btnBack->setTitleText("back");
	this->addChild(btnBack);


	initPassword();	//×°Òþ²ØÔ­°´¼ü£¬ÊäÈëÃÜÂëºóÔÙÏÔÊ¾

	return true;
}

void LayerPassword::initPassword()
{
	Label* tips = Label::createWithSystemFont(LanguageChoose("ÇëÊäÈëÃÜÂë", "Please Enter Password"), "ºÚÌå", 60);
	tips->setPosition(GAME_WIDTH / 2, GAME_HEIGHT * 3 / 4);
	this->addChild(tips);

	m_input = Label::createWithSystemFont("", "ºÚÌå", 60);
	m_input->setPosition(VisibleRect::center());
	this->addChild(m_input);


	//----------------------------Êý×Ö
	for (int i = 0; i < 10; ++i)
	{
		ui::Button* num = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
		this->addChild(num);
		num->setTitleFontSize(80);
		num->setTitleText(StringUtils::format("%d", i));
		num->setTitleColor(Color3B::BLUE);
		num->setPosition(Point(GAME_WIDTH / 11 * (i + 1), GAME_HEIGHT / 6 * 2));
		num->addClickEventListener([this, i](Ref* ref){
			this->onBtnNumber(i);
		});
	}

	//----------------------------»ØÉ¾
	ui::Button* del = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
	this->addChild(del);
	del->setTitleFontSize(60);
	del->setTitleText("del");
	del->setTitleColor(Color3B::RED);
	del->setPosition(Size(GAME_WIDTH / 2, GAME_HEIGHT / 8));
	del->addClickEventListener([this](Ref* ref){
		this->onBtnDel();
	});
}

void LayerPassword::onBtnNumber(int num)
{
	string str = m_input->getString();
	if (str.length() < 20)
	{
		str.push_back('0' + num);
		m_input->setString(str);

		if (m_input->getString() == m_password)
		{
			m_caller->onPasswordOK(m_password);
			//Òþ²ØÃÜÂë²ã
			this->removeFromParent();
		}
	}
}

void LayerPassword::onBtnDel()
{
	string str = m_input->getString();
	if (str.length() > 0)
	{
		str.pop_back();
		m_input->setString(str);
	}
}
