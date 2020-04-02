#include "SettingLayer_Pet.h"
#include "tool/MyTools.h"
#include "tool/MyMenuBase.h"
#include "tool/MyButtonBase.h"
#include "helloScene/HelloWorldScene.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "ioCtrl/ComThread.h"
#include "helloScene/WorkingLayer.h"
#include "DevSingleTest.h"

//////////////////////////////////////////////////////////////////////////

#ifdef CAN_CHOOSE_WORK_TIME
static const vector<const char*> s_radioButtonName[2] = { { "Clean", "Dry" }, { "��ϴ", "���" } };
#else
static const vector<const char*> s_radioButtonName[2] = { { "Short", "Medium", "Long" }, { "��ë", "��ë", "��ë" } };
#endif // CAN_CHOOSE_WORK_TIME

bool SettingLayer_Pet::init()
{
	if (!LayerColor::initWithColor(MENU_BACKGROUND_COLOR, GAME_WIDTH, GAME_HEIGHT))
	{
		return false;
	}

	MenuItemFont::setFontSize(70);

	//-------------------------- ���� ---------------------
	{
		auto lang = MyMenuStaticLang::create(GBKToUTF(string("Language/����")), GBKToUTF(string("����/Language")));
		auto menu = Menu::create(lang, nullptr);
		menu->alignItemsVertically();
		menu->setEnabled(false);
		menu->setColor(Color3B(120, 120, 120));
		menu->setPosition(VisibleRect::size().width * 1 / 4, VisibleRect::size().height / 9 * 8);
		menu->setName("languageName");
		this->addChild(menu);
	}

	{
		auto lang = MenuItemFont::create("English", CC_CALLBACK_1(SettingLayer_Pet::languageCallback, this));
		lang->setName("lang");

		auto menu = Menu::create(lang, nullptr);
		menu->alignItemsVertically();
		menu->setPosition(VisibleRect::size().width * 3 / 4, VisibleRect::size().height / 9 * 8);
		menu->setName("languageData");
		this->addChild(menu);
	}


#ifdef CAN_CHOOSE_WORK_TIME
	initFreeChoose();
#else
	initLimitChoose();
#endif // CAN_CHOOSE_WORK_TIME




	//-------------------------- button --------------------------
	//back
	auto btnBack = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnBack->setPosition(VisibleRect::leftBottom() + Vec2(150, 45));
	btnBack->setTitleFontSize(38);
	btnBack->addClickEventListener([this](Ref*){
		this->onBtnBack();
	});
	btnBack->setName("back");
	addChild(btnBack);


	//record
	auto btnMore = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnMore->setPosition(VisibleRect::rightBottom() + Vec2(-150, 45));
	btnMore->setTitleFontSize(38);
	btnMore->addClickEventListener([this](Ref*){
		this->onBtnRecord();
	});
	btnMore->setName("more");
	addChild(btnMore);


	//fillWater ��������취��ֻ�ܸ������ͨˮ��Ӧ�ñ�����
	auto btnFill = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnFill->setPosition(VisibleRect::bottom() + Vec2(-150, 45));
	btnFill->setTitleFontSize(40);
	btnFill->addClickEventListener([this](Ref*){
		this->onBtnFillWater();
	});
	btnFill->setName("fill");
	addChild(btnFill);


	//������
	if(MyIniGetInt("SETTING", "DEVICE_TEST", 0) > 0)
	{
		auto btnTest = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
		btnTest->setPosition(VisibleRect::bottom() + Vec2(150, 45));
		btnTest->setTitleFontSize(40);
		btnTest->addClickEventListener([this](Ref*){
			this->addChild(DevSingleTest::create());
		});
		btnTest->setTitleText(GBKToUTF(string("ÿ·����")));
		addChild(btnTest);
	}



	return true;
}

void SettingLayer_Pet::onEnter()
{
	LayerColor::onEnter();

	updateLanguage();

	g_pCom->CloseAllDev();

// 	scheduleUpdate();
}

void SettingLayer_Pet::menuInputCallback(cocos2d::Ref* sender, int iMin, int iMax, int exponent)
{
	MyMenuInputNumber* input = (MyMenuInputNumber*)sender;
	this->addChild(InputNumber::MyCreate(input, iMin, iMax, exponent));
}

void SettingLayer_Pet::languageCallback(Ref* sender)
{
	dataGet_NextSetting(eDataKey::set_Language, true);
	updateLanguage();
}

void SettingLayer_Pet::OnMessage(InputNumberGetting* caller, int number, int exponent)
{
	MyMenuInputNumber* input = (MyMenuInputNumber*)caller;
	
	int tag = input->getTag();
	switch (tag)
	{
	case eDataKey::set_PetWash_BaseTime:
	case eDataKey::set_PetWash_IncTime:
	case eDataKey::set_PetDry_BaseTime:
	case eDataKey::set_PetDry_IncTime:
// 		g_pData->set(tag, number);
// 		input->setString(StringUtils::format("%d", g_pData->get(tag)));
// 		break;

	case eDataKey::set_PetWash_BasePrice:
	case eDataKey::set_PetWash_IncPrice:
	case eDataKey::set_PetDry_BasePrice:
	case eDataKey::set_PetDry_IncPrice:
		g_pData->set(tag, number);
		input->setString(IntToFloat(g_pData->get(tag), exponent));
		break;
	}
}

void SettingLayer_Pet::onBtnRecord()
{
	static_cast<LayerMultiplex*>(this->_parent)->switchTo(1);
}

void SettingLayer_Pet::onBtnBack()
{
	Director::getInstance()->replaceScene(TransitionJumpZoom::create(0.3f, HorizontalScreen(Scene_Game::createScene())));
}

void SettingLayer_Pet::updateLanguage()
{
	auto lang = (MenuItemFont*)this->getChildByName("languageData")->getChildByName("lang");
	lang->setString(GBKToUTF(string(IsChinese() ? "����" : "English")));

#ifdef CAN_CHOOSE_WORK_TIME
	for (int i = 0; i < c_layerMultiCount; i++)
	{
		Label* l = (Label*)this->getChildByName(s_radioButtonName[0][i]);
		l->setString(GBKToUTF(string(s_radioButtonName[IsChinese() ? 1 : 0][i])));
	}

	const char* names[3] = { "itemName", "languageName" };
	for (int i = 0; i < 2; ++i)
	{
		Menu* m = (Menu*)this->getChildByName(names[i]);
		for (const auto &child : m->getChildren())
		{
			auto c = (MyMenuStaticLang*)child;
			c->updateLanguage(IsChinese() ? MyMenuStaticLang::langCn : MyMenuStaticLang::langEn);
		}
	}

#else
	for (int i = 0; i < c_layerMultiCount; i++)
	{
		Label* l = (Label*)this->getChildByName(s_radioButtonName[0][i]);
		l->setString(GBKToUTF(string(s_radioButtonName[IsChinese() ? 1 : 0][i])));
	}

	const char* names[3] = { "itemName", "languageName" };
	for (int i = 0; i < 2; ++i)
	{
		Menu* m = (Menu*)this->getChildByName(names[i]);
		for (const auto &child : m->getChildren())
		{
			auto c = (MyMenuStaticLang*)child;
			c->updateLanguage(IsChinese() ? MyMenuStaticLang::langCn : MyMenuStaticLang::langEn);
		}
	}
#endif // CAN_CHOOSE_WORK_TIME

	auto back = (ui::Button*)getChildByName("back");
	back->setTitleText(GBKToUTF(std::string(IsChinese() ? "�˳�" : "Exit")));

	auto more = (ui::Button*)getChildByName("more");
	more->setTitleText(GBKToUTF(std::string(IsChinese() ? "����" : "More")));

	auto fill = (ui::Button*)getChildByName("fill");
	fill->setTitleText(GBKToUTF(std::string(IsChinese() ? "������ͨˮ" : " Fill Water\nBefore Heat")));
}

MenuItemFont* SettingLayer_Pet::makeMenuItemWithData(int dataIndex, int iMin, int iMax, int exponent)
{
	auto item = MyMenuInputNumber::MyCreate(
		IntToFloat(g_pData->get(dataIndex), exponent),
		iMin,
		iMax,
		this,
		CC_CALLBACK_1(SettingLayer_Pet::menuInputCallback, this, iMin, iMax, exponent));
	item->setTag(dataIndex);
	
	return item;
}

void SettingLayer_Pet::onBtnFillWater()
{
	static bool bFill = false;

	bFill = !bFill;
	g_pCom->SetDevOut(DEV_WORKING_WASH.pid, DEV_WORKING_WASH.bit, bFill);

	auto fill = (ui::Button*)getChildByName("fill");
	if (bFill)
		fill->setTitleText(GBKToUTF(std::string(IsChinese() ? "ֹͣ" : "Stop")));
	else
		fill->setTitleText(GBKToUTF(std::string(IsChinese() ? "������ͨˮ" : " Fill Water\nBefore Heat")));

	fill->setColor(bFill ? Color3B::RED : Color3B::WHITE);

}

void SettingLayer_Pet::onSelectRadioButton(ui::RadioButton* radioButton, ui::RadioButton::EventType type)
{
	if (nullptr == radioButton)
		return;

	switch (type)
	{
	case cocos2d::ui::RadioButton::EventType::SELECTED:
		m_lm->switchTo(radioButton->getTag());
		break;
	default:
		break;
	}
}

#ifdef CAN_CHOOSE_WORK_TIME
void SettingLayer_Pet::initFreeChoose()
{
	//-------------------------- �̶����� ---------------------
	{
		auto baseTime = MyMenuStaticLang::create("Base Time(min)", "����ʱ��(����)");
		auto incTime = MyMenuStaticLang::create("Max Extra(min)", "����ʱ(����)");
		auto basePrice = MyMenuStaticLang::create("Base Pay(yuan)", "�����۸�(Ԫ)");
		auto incPrice = MyMenuStaticLang::create("Extra Pay(yuan/min)", "��ʱ�۸�(Ԫ/����)");

		auto itemName = Menu::create(baseTime, incTime, basePrice, incPrice, nullptr);
		itemName->alignItemsVerticallyWithPadding(30);
		itemName->setEnabled(false);
		itemName->setColor(Color3B(120, 120, 120));
		itemName->setPosition(VisibleRect::size().width * 1.2f / 4, VisibleRect::size().height / 2 - 50);
		itemName->setName("itemName");
		this->addChild(itemName);
	}


	//-------------------------- ����ҳ ---------------------
	Layer* lmChildren[c_layerMultiCount];
	//��ϴ
	{
		auto baseTime = makeMenuItemWithData(eDataKey::set_PetWash_BaseTime, DEF_SET_PET_CLEAN_BASE_MIN, DEF_SET_PET_CLEAN_BASE_MAX, 0);	//����ʱ��(����)		3 - 5
		auto incTime = makeMenuItemWithData(eDataKey::set_PetWash_IncTime, 0, 10, 0);//����ʱ(����)			0 - 10
		auto basePrice = makeMenuItemWithData(eDataKey::set_PetWash_BasePrice, 1, 500, 1);//�����۸�(Ԫ)		����
		auto incPrice = makeMenuItemWithData(eDataKey::set_PetWash_IncPrice, 1, 100, 1);//��ʱ�۸�(Ԫ/����)	����

		auto menu = Menu::create(baseTime, incTime, basePrice, incPrice, nullptr);
		menu->alignItemsVerticallyWithPadding(30);
		menu->setPosition(VisibleRect::size().width * 3 / 4, VisibleRect::size().height / 2 - 50);
		menu->setName("menu");

		lmChildren[0] = Layer::create();
		lmChildren[0]->addChild(menu);
	}
	//���
	{
		auto baseTime = makeMenuItemWithData(eDataKey::set_PetDry_BaseTime, DEF_SET_PET_DRY_BASE_MIN, DEF_SET_PET_DRY_BASE_MAX, 0);	//����ʱ��(����)		20��25��30
		auto incTime = makeMenuItemWithData(eDataKey::set_PetDry_IncTime, 0, 60, 0);//����ʱ(����)			0 - 60
		auto basePrice = makeMenuItemWithData(eDataKey::set_PetDry_BasePrice, 1, 500, 1);//�����۸�(Ԫ)		����
		auto incPrice = makeMenuItemWithData(eDataKey::set_PetDry_IncPrice, 1, 100, 1);//��ʱ�۸�(Ԫ/����)		����

		auto menu = Menu::create(baseTime, incTime, basePrice, incPrice, nullptr);
		menu->alignItemsVerticallyWithPadding(30);
		menu->setPosition(VisibleRect::size().width * 3 / 4, VisibleRect::size().height / 2 - 50);
		menu->setName("menu");

		lmChildren[1] = Layer::create();
		lmChildren[1]->addChild(menu);
	}

	m_lm = LayerMultiplex::create(lmChildren[0], lmChildren[1], nullptr);
	this->addChild(m_lm);


	//-------------------------- �л�ͷ ---------------------
	const char* fontName = "Arral";
	const int fontSize = 53;
	const Vec2 vecNameOff = Vec2(50, 0);

	//ʱ�䣨��ϴ����ɣ�
	ui::RadioButtonGroup* radioTimeGroup = ui::RadioButtonGroup::create();
	this->addChild(radioTimeGroup);


	for (int i = 0; i < c_layerMultiCount; ++i)
	{
		ui::RadioButton* radioButton = ui::RadioButton::create("image/menu/radio_button_off.png", "image/menu/radio_button_on.png");
		radioButton->setPosition(Point(GAME_WIDTH / 3 * (0.3f + i), GAME_HEIGHT / 9 * 6.9f));
		radioButton->addEventListener(CC_CALLBACK_2(SettingLayer_Pet::onSelectRadioButton, this));
		radioButton->setTag(i);
		radioTimeGroup->addRadioButton(radioButton);
		this->addChild(radioButton);

		auto label = Label::createWithSystemFont("", fontName, fontSize);
		label->setName(s_radioButtonName[0][i]);
		this->addChild(label);
		label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
		label->setPosition(radioButton->getPosition() + vecNameOff);
	}
	radioTimeGroup->setSelectedButton(0);
}

#else

void SettingLayer_Pet::initLimitChoose()
{
	//-------------------------- �̶����� ---------------------
	{
		auto washTiem = MyMenuStaticLang::create("Wash Time (m)", "��ϴʱ��(��)");
		auto blowTiem = MyMenuStaticLang::create("Dry Time (m)", "���ʱ��(��)");
		auto washPrice = MyMenuStaticLang::create("Wash Price (yuan)", "��ϴ�۸�(Ԫ)");
		auto blowPrice = MyMenuStaticLang::create("Dry Price (yuan)", "��ɼ۸�(Ԫ)");
		auto allPrice = MyMenuStaticLang::create("Wash & Dry Price", "ϴ+���۸�(Ԫ)");

		auto itemName = Menu::create(washTiem, blowTiem, washPrice, blowPrice, allPrice, nullptr);
		itemName->alignItemsVertically();
		itemName->setEnabled(false);
		itemName->setColor(Color3B(120, 120, 120));
		itemName->setPosition(VisibleRect::size().width * 1 / 4, VisibleRect::size().height / 2 - 50);
		itemName->setName("itemName");
		this->addChild(itemName);
	}


	//-------------------------- ����ҳ ---------------------
	Layer* lmChildren[c_layerMultiCount];
	for (int i = 0; i < c_layerMultiCount; ++i)
	{
		auto washTime = makeMenuItemWithData(eDataKey::set_PetTimeWash_S + i, 1, 5, false);	//��ϴʱ�䣨���ӣ�
		auto blowTime = makeMenuItemWithData(eDataKey::set_PetTimeBlow_S + i, 5, 50, false);//���ʱ��
		auto washPrice = makeMenuItemWithData(eDataKey::set_PetPriceWash_S + i, 1, 1000, true);//��ϴ�۸񣨷֣�
		auto blowPrice = makeMenuItemWithData(eDataKey::set_PetPricBlowe_S + i, 1, 1000, true);//�����۸�
		auto allPrice = makeMenuItemWithData(eDataKey::set_PetPriceAll_S + i, 1, 20000, true);//ϴ+���۸�

		auto menu = Menu::create(washTime, blowTime, washPrice, blowPrice, allPrice, nullptr);
		menu->alignItemsVertically();
		menu->setPosition(VisibleRect::size().width * 3 / 4, VisibleRect::size().height / 2 - 50);
		menu->setName("menu");

		lmChildren[i] = Layer::create();
		lmChildren[i]->addChild(menu);
	}

	m_lm = LayerMultiplex::create(lmChildren[0], lmChildren[1], lmChildren[2], nullptr);
	this->addChild(m_lm);


	//-------------------------- �л�ͷ ---------------------
	const char* fontName = "Arral";
	const int fontSize = 53;
	const Vec2 vecNameOff = Vec2(50, 0);

	//ʱ�䣨�̡��С�����
	ui::RadioButtonGroup* radioTimeGroup = ui::RadioButtonGroup::create();
	this->addChild(radioTimeGroup);


	for (int i = 0; i < c_layerMultiCount; ++i)
	{
		ui::RadioButton* radioButton = ui::RadioButton::create("image/menu/radio_button_off.png", "image/menu/radio_button_on.png");
		radioButton->setPosition(Point(GAME_WIDTH / 3 * (0.3f + i), GAME_HEIGHT / 9 * 6.5f));
		radioButton->addEventListener(CC_CALLBACK_2(SettingLayer_Pet::onSelectRadioButton, this));
		radioButton->setTag(i);
		radioTimeGroup->addRadioButton(radioButton);
		this->addChild(radioButton);

		auto label = Label::createWithSystemFont("", fontName, fontSize);
		label->setName(s_radioButtonName[0][i]);
		this->addChild(label);
		label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
		label->setPosition(radioButton->getPosition() + vecNameOff);
	}
	radioTimeGroup->setSelectedButton(0);
}

#endif // CAN_CHOOSE_WORK_TIME

