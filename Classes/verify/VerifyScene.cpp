#include "VerifyScene.h"
#include "GlobalDefined.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "ioCtrl/ComThread.h"
#include "tool/MyTools.h"
#include "GameData.h"
#include "ui/CocosGUI.h"
#include "ioCtrl/KnobSmooth.h"
#include "menu/MenuScene.h"

#ifdef USE_VERIFY

#include "D:/Pro_share/libs/Verify_cc.h"
#pragma comment(lib, "D:/Pro_share/libs/verify_cc.lib")

class MyVerify : public CVerify
{
public:
	MyVerify(){}
	~MyVerify(){}
	virtual DWORD IGetTotalCoinIn()
	{
		int coin = g_pData->get(eDataKey::rec_CentIn) / CENT_P_COIN;
		int cent = g_pData->get(eDataKey::rec_CentIn) % CENT_P_COIN;
		if (cent > 0)
			coin++;
		return coin;
	}
};
static MyVerify	s_verify;
static const float sLayerActionTime = 0.25f;
cocos2d::Scene* Scene_Verify::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Scene_Verify::create();

	// add layer as a child to scene
	scene->addChild(layer);
	layer->setName("verify");

	// return the scene
	return scene;
}

bool Scene_Verify::init()
{
	SceneMgr::init();

	s_verify.Init();

	return true;
}

bool Scene_Verify::IsCanPlay()
{
	return s_verify.IsCanPlay();
}

void Scene_Verify::onEnter()
{
	SceneMgr::onEnter();
	g_pIoCtrlLayer->setVisible(false);

	auto colLayer = LayerColor::create(Color4B::GRAY, GAME_WIDTH, GAME_HEIGHT);
	this->addChild(colLayer);

	auto disp = VerifyDisplayLayer::create();
	this->addChild(disp);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

bool VerifyDisplayLayer::init()
{
	KnobLayerBase::init();

	if (s_verify.m_Data.bCoin)
	{
		auto remainCoin = Label::createWithSystemFont(LanguageChoose("剩余币数: %d", "Remain Coins: %d", s_verify.m_Data.tarCoin - s_verify.IGetTotalCoinIn()), "黑体", 50);
		this->addChild(remainCoin);
		remainCoin->setPosition(VisibleRect::top() + Vec2(0, -100));
	}

	if (s_verify.m_Data.bTime)
	{
		auto remainTime = Label::createWithSystemFont(LanguageChoose("剩余时间: %d", "Remain Time: %d", (s_verify.m_Data.tarTime - s_verify.m_Data.runTime) / 6), "黑体", 50);
		this->addChild(remainTime);
		remainTime->setPosition(VisibleRect::top() + Vec2(0, -300));
	}


	ui::Button* back = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png", "image/menu/button_disable.png");
	this->addChild(back);
	back->setPosition(Size(GAME_WIDTH / 3, GAME_HEIGHT / 3));
	back->setTitleFontSize(45);
	back->setTitleText(LanguageChoose("返回菜单", "Menu"));
	back->addClickEventListener([this](Ref* ref){
		this->onBtnBack();
	});
	bool bNotRemain = (s_verify.m_Data.bCoin && s_verify.IGetTotalCoinIn() >= s_verify.m_Data.tarCoin ||
		s_verify.m_Data.bTime && s_verify.m_Data.runTime >= s_verify.m_Data.tarTime);
	back->setEnabled(!bNotRemain);


	ui::Button* setting = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png", "image/menu/button_disable.png");
	this->addChild(setting);
	setting->setPosition(Size(GAME_WIDTH / 3 * 2, GAME_HEIGHT / 3));
	setting->setTitleFontSize(45);
	setting->setTitleText(LanguageChoose("打码", "Verify"));
	setting->addClickEventListener([this](Ref* ref){
		this->onBtnSetting();
	});

	m_pItems[0] = back;
	m_pItems[1] = setting;

	m_iCurIndex = 0;

	setActionMove(true);
	return true;
}

void VerifyDisplayLayer::setActionMove(bool bIn)
{
	if (bIn)
	{
		this->setPosition(0, -GAME_HEIGHT);
		this->runAction(MoveTo::create(sLayerActionTime, Point::ZERO));
	}
	else
	{
		this->runAction(Sequence::createWithTwoActions(MoveTo::create(sLayerActionTime, Point(0, -GAME_HEIGHT)), RemoveSelf::create()));
	}
}

void VerifyDisplayLayer::update(float delta)
{
	KnobLayerBase::update(delta);

	if (g_pIoCtrlLayer->isKeyDown(IoCtrlLayer::e_key_fire, 0.5f))
	{
		switch (m_iCurIndex)
		{
		case 0:
			if (((ui::Button*)m_pItems[m_iCurIndex])->isEnabled())
				onBtnBack();
			break;
		case 1:
			onBtnSetting();
			break;
		}
	}
}

void VerifyDisplayLayer::onBtnBack()
{
	Director::getInstance()->replaceScene(TransitionJumpZoom::create(0.3f, HorizontalScreen(Scene_Menu::createScene())));
}

void VerifyDisplayLayer::onBtnSetting()
{
	this->getParent()->addChild(VerifySettingLayer::create());
	this->setActionMove(false);
}

void VerifyDisplayLayer::updateItemIndex(int iDirection)
{
	m_pItems[m_iCurIndex]->setColor(Color3B::WHITE);
	m_pItems[m_iCurIndex]->setScale(1);

	if (iDirection > 0)
		++m_iCurIndex %= c_nItemCount;
	else if (iDirection < 0)
		m_iCurIndex = (m_iCurIndex + c_nItemCount - 1) % c_nItemCount;

	ui::Button* btn = (ui::Button*)m_pItems[m_iCurIndex];
	if (btn->isEnabled())
	{
		m_pItems[m_iCurIndex]->setColor(Color3B::YELLOW);
		m_pItems[m_iCurIndex]->setScale(1.15f);
	}
}


//////////////////////////////////////////////////////////////////////////
const char* sCodeName[] = { "codeFrame1", "codeFrame2" };
bool VerifySettingLayer::init()
{
	KnobLayerBase::init();

	//----------------------------显示
	auto no1 = Label::createWithSystemFont(StringUtils::format("No.1: %s", s_verify.m_strMAC), "Hobo Std", 50);
	this->addChild(no1);
	no1->setPosition(VisibleRect::top() + Vec2(0, -40));
	no1->setTextColor(Color4B::YELLOW);

	auto no2 = Label::createWithSystemFont(StringUtils::format("No.2: %u", s_verify.m_Data.runTime + 10000), "Hobo Std", 50);
	this->addChild(no2);
	no2->setPosition(GAME_WIDTH/4, GAME_HEIGHT-100);
	no2->setTextColor(Color4B::GREEN);

	auto coin = Label::createWithSystemFont(StringUtils::format("Coin: %u", s_verify.IGetTotalCoinIn()), "Hobo Std", 50);
	this->addChild(coin);
	coin->setPosition(GAME_WIDTH*3/4, GAME_HEIGHT-100);
	coin->setTextColor(Color4B::ORANGE);

	int iItem = 0;
	//----------------------------输入框
	for (int i = 0; i < 2; ++i)
	{
		ui::Button* codeFrame = ui::Button::create("image/menu/edit_code.png", "image/menu/edit_code.png");
		this->addChild(codeFrame);
		codeFrame->setName(sCodeName[i]);
		codeFrame->setScale(1, 0.8f);
		codeFrame->setTitleFontSize(75);
		codeFrame->setPosition(Size(GAME_WIDTH / 2 + 100, GAME_HEIGHT / 7 * (5 - i)-50));
		codeFrame->addClickEventListener([this, i](Ref* ref){
			this->onBtnInput(i);
		});

		m_pItems[iItem++] = codeFrame;

		auto name = Label::createWithSystemFont(StringUtils::format("CODE:%d", i + 1), "黑体", 50, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
		this->addChild(name);
		name->setPosition(200, codeFrame->getPositionY());
	}


	//----------------------------数字
	for (int i = 0; i < 10; ++i)
	{
		ui::Button* num = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
		this->addChild(num);
		num->setTitleFontSize(60);
		num->setTitleText(StringUtils::format("%d", i));
		num->setTitleColor(Color3B::BLUE);
		num->setPosition(Point(GAME_WIDTH / 11 * (i + 1), GAME_HEIGHT / 6 * 2 - 30));
		num->addClickEventListener([this, i](Ref* ref){
			this->onBtnNumber(i);
		});
		m_pItems[iItem++] = num;
	}

	//----------------------------回删
	ui::Button* del = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
	this->addChild(del);
	del->setTitleFontSize(45);
	del->setTitleText("del");
	del->setTitleColor(Color3B::RED);
	del->setPosition(Size(GAME_WIDTH / 3, GAME_HEIGHT / 9));
	del->addClickEventListener([this](Ref* ref){
		this->onBtnDel();
	});
	m_pItems[iItem++] = del;

	//----------------------------打码OK
	ui::Button* ok = ui::Button::create("image/menu/button_number.png", "image/menu/button_numberPressed.png");
	this->addChild(ok);
	ok->setTitleFontSize(45);
	ok->setTitleText("OK");
	ok->setTitleColor(Color3B::GREEN);
	ok->setPosition(Size(GAME_WIDTH / 3 * 2, GAME_HEIGHT / 9));
	ok->addClickEventListener([this](Ref* ref){
		this->onBtnOK();
	});
	m_pItems[iItem++] = ok;


	//----------------------------返回
	auto back = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png", "image/menu/button_disable.png");
	this->addChild(back);
	back->setPosition(VisibleRect::leftBottom() + back->getContentSize()/2);
	back->setTitleFontSize(45);
	back->setTitleText(GBKToUTF(string("返回/back")));
	back->setName("back");
	back->addClickEventListener([this](Ref* ref){
		this->onBtnBack();
	});
	m_iCurIndex = iItem;
	m_pItems[iItem++] = back;


	m_input[0] = m_input[1] = 0;
	onBtnInput(0);

	setActionMove(true);
	return true;
}

void VerifySettingLayer::update(float delta)
{
	KnobLayerBase::update(delta);

	if (g_pIoCtrlLayer->isKeyDown(IoCtrlLayer::e_key_fire, 0.5f))
	{
		const int beginInput = 0;
		const int beginNum = beginInput + 2;
		const int beginFunc = beginNum + 10;
		switch (m_iCurIndex)
		{
		case beginInput + 0:
		case beginInput + 1:
			onBtnInput(m_iCurIndex - beginInput);
			break;

		case beginNum + 0:
		case beginNum + 1:
		case beginNum + 2:
		case beginNum + 3:
		case beginNum + 4:
		case beginNum + 5:
		case beginNum + 6:
		case beginNum + 7:
		case beginNum + 8:
		case beginNum + 9:
			onBtnNumber(m_iCurIndex - beginNum);
			break;

		case beginFunc + 0:
			onBtnDel();
			break;

		case beginFunc + 1:
			onBtnOK();
			break;

		case beginFunc + 2:
			onBtnBack();
			break;
		}
	}
}

void VerifySettingLayer::onBtnInput(int index)
{
	m_curInput = index;
	this->getChildByName(sCodeName[index])->setColor(Color3B::RED);
	this->getChildByName(sCodeName[(index+1)%2])->setColor(Color3B::WHITE);
}

void VerifySettingLayer::onBtnNumber(int num)
{
	if (m_input[m_curInput] < 1000000000)
	{
		m_input[m_curInput] = m_input[m_curInput] * 10 + num;
		updateNumber();
	}
}

void VerifySettingLayer::onBtnDel()
{
	m_input[m_curInput] /= 10;
	updateNumber();
}

void VerifySettingLayer::onBtnOK()
{
	bool rst = s_verify.FireActivationCode(m_input[0], m_input[1]);
	if (rst)
	{
		auto tips = Label::createWithSystemFont(GBKToUTF(string("成功")), "黑体", 300, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
		this->addChild(tips);
		tips->setPosition(VisibleRect::center());
		this->setTouchEnabled(false);
		onBtnBack();
	}
	else
	{
		auto wrong = Label::createWithSystemFont(GBKToUTF(string(IsChinese() ? "错误" : "ERROR")), "黑体", 300, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
		this->addChild(wrong);
		wrong->setTextColor(Color4B::RED);
		wrong->setPosition(VisibleRect::center());
		wrong->runAction(Sequence::createWithTwoActions(DelayTime::create(2), RemoveSelf::create()));

		this->setTouchEnabled(false);
		schedule(schedule_selector(VerifySettingLayer::timerEnableClick), 2, 0, 0);
	}
}

void VerifySettingLayer::updateNumber()
{
	string str = "";
	if (m_input[m_curInput] > 0)
	{
		str = StringUtils::format("%u", m_input[m_curInput]);
	}
	((ui::Button*)getChildByName(sCodeName[m_curInput]))->setTitleText(str);
}

void VerifySettingLayer::timerEnableClick(float delta)
{
	this->setTouchEnabled(true);
}

void VerifySettingLayer::onBtnBack()
{
	this->getParent()->addChild(VerifyDisplayLayer::create());
	setActionMove(false);
}

void VerifySettingLayer::setActionMove(bool bIn)
{
	if (bIn)
	{
		this->setPosition(0, GAME_HEIGHT);
		this->runAction(MoveTo::create(sLayerActionTime, Point::ZERO));
	}
	else
	{
		this->runAction(Sequence::create(MoveTo::create(sLayerActionTime, Point(0, GAME_HEIGHT)),
		RemoveSelf::create(),
		nullptr));
	}
}

void VerifySettingLayer::updateItemIndex(int iDirection)
{
	if (0 == m_iCurIndex || 1 == m_iCurIndex)
	{
		m_pItems[m_iCurIndex]->setColor(m_iCurIndex == m_curInput ? Color3B::RED : Color3B::WHITE);
	}
	else
	{
		m_pItems[m_iCurIndex]->setColor(Color3B::WHITE);
	}
	m_pItems[m_iCurIndex]->setScale(1);

	if (iDirection > 0)
		++m_iCurIndex %= c_nItemCount;
	else if (iDirection < 0)
		m_iCurIndex = (m_iCurIndex + c_nItemCount - 1) % c_nItemCount;

	if (0 == m_iCurIndex || 1 == m_iCurIndex)
	{
		m_pItems[m_iCurIndex]->setColor(m_iCurIndex == m_curInput ? Color3B::ORANGE : Color3B::YELLOW);
	}
	else
	{
		m_pItems[m_iCurIndex]->setColor(Color3B::YELLOW);
	}
	m_pItems[m_iCurIndex]->setScale(1.15f);
}

#else

cocos2d::Scene* Scene_Verify::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Scene_Verify::create();

	// add layer as a child to scene
	scene->addChild(layer);
	layer->setName("verify");

	// return the scene
	return scene;
}

#endif // USE_VERIFY
