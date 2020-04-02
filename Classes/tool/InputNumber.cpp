#include "InputNumber.h"
#include "cocos/ui/CocosGUI.h"
#include "GlobalDefined.h"
#include "tool/MyTools.h"


void InputNumberGetting::SetNumber(int number, int exponent)
{
	if (number >= m_inputMin && number <= m_inputMax)
		m_target->OnMessage(this, number, exponent);
	else
		CCLOG("number wrong!");
}


//////////////////////////////////////////////////////////////////////////

InputNumber* InputNumber::MyCreate(InputNumberGetting* pCaller, int inputMin, int inputMax, int exponent)
{
	InputNumber* ret = new (std::nothrow) InputNumber();
	if (ret && ret->init(pCaller, inputMin, inputMax, exponent))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}

	return ret;
}


#if (ROTATE_SCREEN == 1)
static const float c_dist = GAME_WIDTH / 4;	// 竖屏
static const Point sc_posNumber[10] = {
	{ GAME_WIDTH - c_dist * 2, c_dist * 1 },
	{ GAME_WIDTH - c_dist * 3, c_dist * 4 }, { GAME_WIDTH - c_dist * 2, c_dist * 4 }, { GAME_WIDTH - c_dist * 1, c_dist * 4 },
	{ GAME_WIDTH - c_dist * 3, c_dist * 3 }, { GAME_WIDTH - c_dist * 2, c_dist * 3 }, { GAME_WIDTH - c_dist * 1, c_dist * 3 },
	{ GAME_WIDTH - c_dist * 3, c_dist * 2 }, { GAME_WIDTH - c_dist * 2, c_dist * 2 }, { GAME_WIDTH - c_dist * 1, c_dist * 2 }
};
static const Point sc_posTips = Point(GAME_WIDTH / 2, GAME_HEIGHT*0.9f);
static const Point sc_posFrame = Point(GAME_WIDTH / 2, GAME_HEIGHT*0.75f);
static const Point sc_posCtrl[2] = { { GAME_WIDTH - c_dist * 3, c_dist * 1 }, { GAME_WIDTH - c_dist * 1, c_dist * 1 }};
#else
static const float c_dist = GAME_HEIGHT / 5;	// 横屏
static const Point sc_posNumber[10] = {
	{ GAME_WIDTH - c_dist * 2, c_dist * 1 },
	{ GAME_WIDTH - c_dist * 3, c_dist * 4 }, { GAME_WIDTH - c_dist * 2, c_dist * 4 }, { GAME_WIDTH - c_dist * 1, c_dist * 4 },
	{ GAME_WIDTH - c_dist * 3, c_dist * 3 }, { GAME_WIDTH - c_dist * 2, c_dist * 3 }, { GAME_WIDTH - c_dist * 1, c_dist * 3 },
	{ GAME_WIDTH - c_dist * 3, c_dist * 2 }, { GAME_WIDTH - c_dist * 2, c_dist * 2 }, { GAME_WIDTH - c_dist * 1, c_dist * 2 }
};
static const Point sc_posTips = Point((GAME_WIDTH - c_dist*4) / 2, GAME_HEIGHT / 3 * 2);
static const Point sc_posFrame = Point((GAME_WIDTH - c_dist*4) / 2, GAME_HEIGHT / 3 * 1);
static const Point sc_posCtrl[2] = { { GAME_WIDTH - c_dist * 3, c_dist * 1 }, { GAME_WIDTH - c_dist * 1, c_dist * 1 }};
#endif // HORIZONTAL_SCREEN

bool InputNumber::init(InputNumberGetting* pCaller, int inputMin, int inputMax, int exponent)
{
	LayerColor::initWithColor(Color4B(0, 0, 0, 200), GAME_WIDTH, GAME_HEIGHT);
	DisableBelowTouch(this);
	this->setZOrder(1000000);

	m_caller = pCaller;
	m_min = inputMin;
	m_max = inputMax;
	m_exponent = exponent;

	//----------------------------限制
	Label* limit = nullptr;
	limit = Label::createWithSystemFont(StringUtils::format("%s - %s", IntToFloat(m_min, exponent).c_str(), IntToFloat(m_max, exponent).c_str()), "黑体", 60);
	this->addChild(limit);
	limit->setPosition(sc_posTips);
	limit->setColor(Color3B::GRAY);

	//----------------------------输入框
	ui::Button* codeFrame = ui::Button::create("image/menu/edit_code.png", "image/menu/edit_code.png");
	this->addChild(codeFrame);
	codeFrame->setName("text");
	codeFrame->setTitleFontSize(53);
	codeFrame->setColor(Color3B::YELLOW);
	codeFrame->setPosition(sc_posFrame);


	//----------------------------数字
	for (int i = 0; i < 10; ++i)
	{
		ui::Button* num = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
		this->addChild(num);
		num->setTitleFontSize(60);
		num->setTitleText(StringUtils::format("%d", i));
		num->setTitleColor(Color3B::BLUE);
		num->setPosition(sc_posNumber[i]);
		num->addClickEventListener([this, i](Ref* ref){
			this->onBtnNumber(i);
		});
	}

	//----------------------------回删
	ui::Button* del = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
	this->addChild(del);
	del->setTitleFontSize(45);
	del->setTitleText("del");
	del->setTitleColor(Color3B::RED);
	del->setPosition(sc_posCtrl[0]);
	del->addClickEventListener([this](Ref* ref){
		this->onBtnDel();
	});

	//----------------------------OK
	ui::Button* ok = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
	this->addChild(ok);
	ok->setTitleFontSize(45);
	ok->setTitleText("OK");
	ok->setTitleColor(Color3B::GREEN);
	ok->setPosition(sc_posCtrl[1]);
	ok->addClickEventListener([this](Ref* ref){
		this->onBtnOK();
	});


	//----------------------------返回
	auto back = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png", "image/menu/button_disable.png");
	this->addChild(back);
	back->setPosition(VisibleRect::leftBottom() + back->getContentSize()/2);
	back->setTitleFontSize(45);
	back->setTitleText("back");
	back->setName("back");
	back->addClickEventListener([this](Ref* ref){
		this->onBtnBack();
	});

	m_input = 0;

	return true;
}

void InputNumber::onBtnNumber(int num)
{
	int tmp = m_input * 10 + num;
	if (tmp <= m_max)	//最大输入值
	{
		m_input = tmp;
		updateNumber();
	}
	else
	{
		MySound::playEffect("sound/judge_wrong.mp3");
	}
}

void InputNumber::onBtnDel()
{
	m_input /= 10;
	updateNumber();
}

void InputNumber::onBtnOK()
{
	if (m_input >= m_min && m_input <= m_max)
	{
		m_caller->SetNumber(m_input, m_exponent);
		MySound::playEffect("sound/judge_right.mp3");
	}
	else
	{
		MySound::playEffect("sound/judge_wrong.mp3");
	}
	onBtnBack();
}

void InputNumber::updateNumber()
{
	string str;
	switch (m_exponent)
	{
	case 1:	str = "0.000";	break;
	case 2:	str = "0.00";	break;
	case 3:	str = "0.0";	break;
	default: str = "0";		break;
	}

	if (m_input > 0)
	{
		str = IntToFloat(m_input, m_exponent);
	}
	((ui::Button*)getChildByName("text"))->setTitleText(str);
}

void InputNumber::onBtnBack()
{
	//回调到输入框
	removeFromParent();
}

