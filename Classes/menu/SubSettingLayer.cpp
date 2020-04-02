#include "SubSettingLayer.h"
#include "tool/MyTools.h"
#include "tool/MyMenuBase.h"
#include "RecordLayer.h"
#include "tool/MyButtonBase.h"
#include "ioCtrl/ComThread.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "buy/OpenOneByOne.h"
#include "tool/MyDrawTable.h"

//////////////////////////////////////////////////////////////////////////

bool SubSettingBase::init()
{
	if (!LayerColor::initWithColor(MENU_BACKGROUND_COLOR, GAME_WIDTH, GAME_HEIGHT))
	{
		return false;
	}
	DisableBelowTouch(this);


	//标题
	m_title = Label::createWithSystemFont("", "黑体", 75);
	this->addChild(m_title);
	m_title->setPosition(GAME_WIDTH / 2, GAME_HEIGHT - 40);
	m_title->setColor(Color3B::BLUE);


	//back
	auto btnBack = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnBack->setPosition(VisibleRect::leftBottom() + Vec2(150, 45));
	btnBack->setTitleFontSize(38);
	btnBack->setTitleText(GBKToUTF(string(IsChinese() ? "返回" : "back")));
	btnBack->addClickEventListener([this](Ref*){
		this->removeFromParent();
	});
	this->addChild(btnBack);


	return true;
}



//////////////////////////////////////////////////////////////////////////

void SetBtnCent(ui::Button* btn, int index, int cent)
{
	POINT vh = box_GetPidBit(index);
	btn->setTitleText(LanguageChoose("%c%d\n%s", "%c%d\n%s", 'A' + vh.x, vh.y + 1, CentToChar(cent).c_str()));
	setBoxTitleFrame(btn, false);
}

bool SettingBuyPrice::init()
{
	SubSettingBase::init();

	m_title->setString(LanguageChoose("购买价格(元)", "Buy Price(Yuan)"));

#if 1

	m_bTotal = false;

	//快速设置
	ui::Button* btnTotal = ui::Button::create("image/menu/button_normal.png");
	this->addChild(btnTotal);
	btnTotal->setTag(1000);	//避免与格子冲突
	btnTotal->setPosition(VisibleRect::rightBottom() + Vec2(-btnTotal->getContentSize().width / 2, btnTotal->getContentSize().height / 2));
	btnTotal->setTitleFontSize(53);
	btnTotal->setTitleText(GBKToUTF(string(IsChinese() ? "快速设置" : "Quick Set")));
	btnTotal->addClickEventListener([this](Ref* ref){
		this->m_bTotal = !this->m_bTotal;

		ui::Button* b = (ui::Button*)ref;
		b->setColor(this->m_bTotal ? Color3B::RED : Color3B::WHITE);

		MyButtonInputNumber* btnPrice = (MyButtonInputNumber*)this->getChildByTag(1001);
		btnPrice->setEnabled(this->m_bTotal);
	});


	m_nTotalPrice = 3000;

	auto btnTotalPrice = MyButtonInputNumber::MyCreate(BUY_PRICE_MIN, BUY_PRICE_MAX, this, "image/menu/edit_quick.png");
	this->addChild(btnTotalPrice);
	btnTotalPrice->setTag(1001);
	btnTotalPrice->setTitleFontSize(75);
	btnTotalPrice->setPosition(btnTotal->getPosition() + Vec2(-btnTotalPrice->getContentSize().width, 0));
	btnTotalPrice->setEnabled(false);
	btnTotalPrice->setTitleText(CentToChar(m_nTotalPrice));
	btnTotalPrice->addClickEventListener([this](Ref* ref){
		MyButtonInputNumber* input = (MyButtonInputNumber*)ref;
		this->addChild(InputNumber::MyCreate(input, BUY_PRICE_MIN, BUY_PRICE_MAX));
	});


	//单个设置
	for (int i = 0; i < COUNT_PRICE; ++i)
	{
		int price = dataGet_BuyCent(i);
		auto btn = MyButtonInputNumber::MyCreate(BUY_PRICE_MIN, BUY_PRICE_MAX, this, "image2/buy/enable.png", "image2/buy/selected.png", "image2/buy/disable.png");
		this->addChild(btn);
		btn->setTag(i);		
		
		btn->setPosition(box_GetPosition(i));
		btn->setScale(box_GetSize(i).width, box_GetSize(i).height);
		setBoxTitleFrame(btn, true);

		SetBtnCent(btn, i, price);

		btn->addClickEventListener([this, i](Ref* ref){
			MyButtonInputNumber* input = (MyButtonInputNumber*)ref;
			if (this->m_bTotal)
			{
				g_pData->set(eDataKey::set_BuyCent_01 + input->getTag(), this->m_nTotalPrice);
				SetBtnCent(input, input->getTag(), this->m_nTotalPrice);
			}
			else
			{
				this->addChild(InputNumber::MyCreate(input, BUY_PRICE_MIN, BUY_PRICE_MAX));
			}
		});
	}

#else

	bool bSeparate = MyIniGetInt("SETTING", USER_DEF_PRICE_SEPARATE, 0) != 0;

	MenuItemFont::setFontSize(80);
	//static menu
	MenuItemFont* nameUnified = MenuItemFont::create(GBKToUTF(string(IsChinese() ? "统一" : "Unified")));
	MenuItemFont* nameArea[COUNT_PRICE];
	for (int v = 0; v < COUNT_PRICE; ++v)
	{
		nameArea[v] = MenuItemFont::create(StringUtils::format("%c", 'A'+v));
	}
	Menu* menuStatic = Menu::create(nameUnified, nameArea[0], nameArea[1], nameArea[2], nameArea[3], nameArea[4], nullptr);
	menuStatic->alignItemsVertically();
	menuStatic->setEnabled(false);
	menuStatic->setColor(Color3B(120, 120, 120));
	

	//can change
	MyMenuInputNumber* inputUnified = MyMenuInputNumber::MyCreate(bSeparate ? "--" : StringUtils::format("%d", dataGet_BuyCent(0)), 10, 200, this, CC_CALLBACK_1(SettingBuyPrice::menuCallback, this));
	inputUnified->setTag(10);
	inputUnified->setEnabled(!bSeparate);

	MyMenuInputNumber* inputArea[COUNT_PRICE];
	for (int v = 0; v < COUNT_PRICE; ++v)
	{
		inputArea[v] = MyMenuInputNumber::MyCreate(StringUtils::format("%d", dataGet_BuyCent(v)), BUY_PRICE_MIN, BUY_PRICE_MAX, this, CC_CALLBACK_1(SettingBuyPrice::menuCallback, this));
		inputArea[v]->setTag(v);
		inputArea[v]->setEnabled(bSeparate);
	}
	Menu* menuInput = Menu::create(inputUnified, inputArea[0], inputArea[1], inputArea[2], inputArea[3], inputArea[4], nullptr);
	menuInput->alignItemsVertically();

	this->addChild(menuStatic);
	menuStatic->setPosition(VisibleRect::size().width / 3 , VisibleRect::size().height / 2);

	this->addChild(menuInput);
	menuInput->setPosition(VisibleRect::size().width *2  / 3, VisibleRect::size().height / 2);

#endif

	return true;
}

void SettingBuyPrice::menuCallback(Ref* sender)
{
	MyMenuInputNumber* input = (MyMenuInputNumber*)sender;
	this->addChild(InputNumber::MyCreate(input, BUY_PRICE_MIN, BUY_PRICE_MAX));
}

void SettingBuyPrice::OnMessage(InputNumberGetting* caller, int number, int exponent)
{
	MyButtonInputNumber* input = (MyButtonInputNumber*)caller;
	int index = input->getTag();
	if (index >= 0 && index < COUNT_PRICE)
	{
		g_pData->set(eDataKey::set_BuyCent_01 + index, number);
		SetBtnCent(input, index, number);
	}
	else if (index == 1001)
	{
		m_nTotalPrice = number;
		input->setTitleText(CentToChar(number));
	}
}


//////////////////////////////////////////////////////////////////////////
void SetYesNoStatus(Node* btnYes, Node* btnNo, int index, int contain)
{
	btnYes->setVisible(contain > 0);
	Node* f = btnYes->getParent()->getChildByTag(DEF_TAG_ONSALE_FRAME + btnYes->getTag());
	if(f)
		f->setVisible(btnYes->isVisible());

	btnNo->setVisible(contain <= 0);
	f = btnNo->getParent()->getChildByTag(DEF_TAG_ONSALE_FRAME + btnNo->getTag());
	if(f)
		f->setVisible(btnNo->isVisible());

#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
	if (contain > 0)
	{
		POINT vh = box_GetPidBit(index);
		((ui::Button*)btnYes)->setTitleText(StringUtils::format("%c%d\n%d", 'A' + vh.x, 1 + vh.y, contain));
		setBoxTitleFrame((ui::Button*)btnYes, false);
	}
#endif
}
bool SettingSaleOnOff::init()
{
	SubSettingBase::init();

	m_title->setString(LanguageChoose("补货卸货", "OnSale Setting"));

	m_yes = Node::create();
	this->addChild(m_yes);
	m_no = Node::create();
	this->addChild(m_no);

	const Color3B colorYesNo[2] = { Color3B::WHITE, Color3B::GRAY };
	for (int i = 0; i < COUNT_BOX; ++i)
	{
		ui::Button* yesNo[2] = { nullptr }; // [0] yes, [1] no
		for (int k = 0; k < 2; ++k)
		{
			auto btn = yesNo[k] = MyButtonInputNumber::create(k == 0 ? "image2/buy/enable.png" : "image2/buy/disable.png", "image2/buy/selected.png");
			btn->setTag(i);

			btn->setTitleFontSize(75);
			btn->setColor(colorYesNo[k]);

			btn->setPosition(box_GetPosition(i));
			btn->setScale(box_GetSize(i).width, box_GetSize(i).height);

			btn->addClickEventListener([this](Ref* ref){
				MyButtonInputNumber* b = (MyButtonInputNumber*)ref;
				this->callbackSingle((Node*)ref);
			});
		}
		m_yes->addChild(yesNo[0]);
		m_no->addChild(yesNo[1]);
		
		POINT vh = box_GetPidBit(i);
		yesNo[0]->setTitleText(LanguageChoose("%c%d\n有货", "%c%d\nOn", 'A' + vh.x, vh.y + 1));
		yesNo[1]->setTitleText(LanguageChoose("%c%d\n无货", "%c%d\nOff", 'A' + vh.x, vh.y + 1));
		Sprite* f = setBoxTitleFrame(yesNo[1], true);
		if(f)
			f->setTag(DEF_TAG_ONSALE_FRAME + i);
		f = setBoxTitleFrame(yesNo[0], true);
		if(f)
			f->setTag(DEF_TAG_ONSALE_FRAME + i);
		SetYesNoStatus(yesNo[0], yesNo[1], i, dataGet_OnSale(i));
	}

	for (int i = 0; i < 2; ++i)
	{
		auto total = ui::Button::create(i == 0 ? "image/menu/button_normal.png" : "image/menu/button_disable.png");
		total->setPosition(VisibleRect::rightBottom() + Vec2(-100 - i * 300, 45));
		total->setTitleFontSize(38);
		total->setTitleText(i == 0 ? LanguageChoose("全部装满", "Fill All") : LanguageChoose("全部清空", "Empty All"));
		total->addClickEventListener([this, i](Ref* ref){
			this->callbackTotal((Node*)ref);
		});
		total->setTag(i);
		this->addChild(total);
	}

	//all off
	return true;
}

void SettingSaleOnOff::callbackSingle(Node* caller)
{
	int tag = caller->getTag();
	int contain = dataSet_DecreaseContain(tag, true);

	SetYesNoStatus(m_yes->getChildByTag(tag), m_no->getChildByTag(tag), tag, contain);
}

void SettingSaleOnOff::callbackTotal(Node* caller)
{
	int tag = caller->getTag();
	int contain = tag == 0 ? BOX_CONTAIN_GOODS : 0;

	for (int i = 0; i < COUNT_BOX; ++i)
	{
		if (g_pData->get(eDataKey::status_remainGoods_01 + i) != contain)
		{
			g_pData->set(eDataKey::status_remainGoods_01 + i, contain);
			SetYesNoStatus(m_yes->getChildByTag(i), m_no->getChildByTag(i), i, contain);
		}
	}

}


//////////////////////////////////////////////////////////////////////////

bool SettingOpenTest::init()
{
	SubSettingBase::init();

	m_title->setString(LanguageChoose("出货测试", "Open Test"));

	//-------------------------- IconRoll --------------------------
	for (int i = 0; i < COUNT_BOX; ++i)
	{
		auto btn = m_btns[i] = ui::Button::create("image2/buy/enable.png", "image2/buy/selected.png", "image2/buy/disable.png");
		this->addChild(btn);

		POINT vh = box_GetPidBit(i);
		btn->setTag(vh.x * 10 + vh.y);

		btn->setTitleText(GBKToUTF(StringUtils::format("%c%d", 'A' + vh.x, vh.y + 1)));

		btn->setPosition(box_GetPosition(i));
		btn->setScale(box_GetSize(i).width, box_GetSize(i).height);
		setBoxTitleFrame(btn, true);

		btn->addClickEventListener([this, vh](Ref* ref){
			this->callbackClickBtn(vh.x, vh.y);
		});
	}


	//-------------------------- auto test --------------------------
#if (BOX_CONTAIN_GOODS == BOX_EACH_ONE)
	auto btnTest = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnTest->setPosition(VisibleRect::rightBottom() + Vec2(-btnTest->getSize().width / 2, btnTest->getSize().height / 2));
	btnTest->setTitleFontSize(60);
	btnTest->setTitleText(GBKToUTF(string(IsChinese() ? "自动" : "Auto")));
	btnTest->addClickEventListener([this](Ref*){
		//购买层，进入后屏蔽游戏层
		Director::getInstance()->pushScene(HorizontalScreen(OpenOneByOne::createScene()));
	});
	addChild(btnTest);
#endif

	schedule(schedule_selector(SettingOpenTest::timerBtnStatus), 1, CC_REPEAT_FOREVER, 0);

	timerBtnStatus(0);
	scheduleUpdate();
	return true;
}

void SettingOpenTest::update(float delta)
{
	// 	updateComPressBtn();
}

void SettingOpenTest::callbackClickBtn(int v, int h)
{
	//防止其他位置不停电
	if (g_pIoCtrlLayer->isLockOpening())
		return;

	//开门
	g_pIoCtrlLayer->openLock(v, h);

	timerBtnStatus(0);
}

void SettingOpenTest::timerBtnStatus(float delta)
{
	for (int i = 0; i < COUNT_BOX; ++i)
	{
		m_btns[i]->setEnabled(!g_pIoCtrlLayer->isLockOpening());
	}
}


//////////////////////////////////////////////////////////////////////////

bool SettingOpenRecord::init()
{
	SubSettingBase::init();

	m_title->setString(LanguageChoose("出货记录", "Open Record"));


	time_t sysTime = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), IsChinese() ? "当前系统时间： %Y-%m-%d %H:%M" : "System Time： %Y-%m-%d %H:%M", localtime(&sysTime));

	auto lSys = Label::createWithSystemFont(GBKToUTF(string(tmp)), "黑体", 68);
	this->addChild(lSys);
	lSys->setPosition(GAME_WIDTH / 2, GAME_HEIGHT / 11.0f * 9);
	lSys->setColor(Color3B::YELLOW);

	for (int i = 0; i < 6; ++i)
	{
		time_t t = g_pData->get(eDataKey::list_open1_time + i * 2);
		if (t == 0)
			break;
		int pos = g_pData->get(eDataKey::list_open1_pos + i * 2);
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M", localtime(&t));
		string str = StringUtils::format("%c%d   --   %s", 'A' + pos / 10, pos % 10, tmp);

		auto l = Label::createWithSystemFont(str, "黑体", 68);
		this->addChild(l);
#if (ROTATE_SCREEN == 1)
		l->setPosition(GAME_WIDTH / 2, GAME_HEIGHT / 11.0f * (8 - i));
#else
		l->setPosition(GAME_WIDTH / 2, GAME_HEIGHT / 11.0f * (7.5f - i));
#endif
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////

bool SettingCleanRecord::init()
{
	SubSettingBase::init();

	m_title->setString(LanguageChoose("清洗记录(次)", "Clean Record(times)"));

#ifdef CAN_CHOOSE_WORK_TIME

	//查看提示
	auto tips = Label::createWithSystemFont(LanguageChoose("每格内数字，上方为次数，下方为收益", "Number in each grid, times above, income below"), "黑体", 50);
	this->addChild(tips);
	tips->setPosition(VisibleRect::top() + Vec2(0, -130));
	tips->setTextColor(Color4B::BLACK);

	

	auto table = MyDrawTable::MyCreate(4, 6, GAME_WIDTH * 6 / 7, GAME_HEIGHT * 4 / 7, 4, Color4F::BLUE);
	this->addChild(table);
	table->setAnchorPoint(Point::ANCHOR_MIDDLE);
	table->setPosition(VisibleRect::center());


	const char* tableV[2][3] = {
		{ "洗烘", "清洗", "烘干" },
		{ "W+D", "Wash", "Dry" }
	};
	const char* tableH[5] = { "<1", "1-10", "10-20", "20-30", ">30" };


	for (int i = 0; i < 3; ++i)
	{
		auto v = createFontTTF("fonts/dai meng wa wa ti.ttf", 53, 2, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
		this->addChild(v);
		v->setPosition(GAME_WIDTH / 7, GAME_HEIGHT / 7 * (4 - i));
		v->setString(LanguageChoose(tableV[0][i], tableV[1][i]));

		table->myAddLine(Point(GAME_WIDTH / 7, GAME_HEIGHT / 7 * (0.5f + i)),
			Point(GAME_WIDTH * 6 / 7, GAME_HEIGHT / 7 * (0.5f+i)),
			2,
			Color4F::BLUE);
	}



	for (int i = 0; i < 5; ++i)
	{
		auto h = createFontTTF("fonts/dai meng wa wa ti.ttf", 53, 2, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
		this->addChild(h);
		h->setPosition(GAME_WIDTH * (2+i) / 7, GAME_HEIGHT / 7 * 5);
		h->setString(LanguageChoose(tableH[i], tableH[i]));
	}
	

	//------------------ 区分 total / last ---------
	m_lTotal = Layer::create();
	this->addChild(m_lTotal);
	m_lTotal->setVisible(false);

	m_lLast = Layer::create();
	this->addChild(m_lLast);
	m_lLast->setVisible(true);

	// 洗烘、洗、烘 记录
	Layer* lTmp[2] = { m_lTotal, m_lLast };

	for (int iTable = 0; iTable < 2; ++iTable)
	{
		for (int k = 0; k < LYY_PET_PRICE_COUNT; ++k)
		{
			for (int i = 0; i < LYY_PET_WORK_COUNT; ++i)
			{
				int index = LYY_PET_PRICE_COUNT * i + k;

				Label* l = Label::createWithSystemFont("", "黑体", 40);
				l->setPosition(GAME_WIDTH * (2 + k) / 7, GAME_HEIGHT / 7 * (4 - i) + 30);
				l->setTag(index);
				lTmp[iTable]->addChild(l);

				Label* l2 = Label::createWithSystemFont("", "黑体", 40);
				l2->setPosition(GAME_WIDTH * (2+k) / 7, GAME_HEIGHT / 7 * (4 - i) - 30);
				l2->setTag(index + LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT);
				lTmp[iTable]->addChild(l2);

				Color4B col = Color4B::GREEN;
				switch (i)
				{
				case 1:	col = Color4B::MAGENTA;	break;
				case 2:	col = Color4B::ORANGE;	break;
				}
				l->setTextColor(col);
				l2->setTextColor(col);
			}
		}
	}


	//-------------------------- switch between Total and Last --------------------------
	m_btnSelectTotal = ui::Button::create("image/menu/button_switch.png", "", "image/menu/button_switchDisable.png");
	m_btnSelectTotal->setPosition(VisibleRect::rightBottom() + Vec2(-m_btnSelectTotal->getContentSize().width/2, m_btnSelectTotal->getContentSize().height/2));
	m_btnSelectTotal->setTitleFontSize(53);
	m_btnSelectTotal->addClickEventListener([this](Ref*){
		this->m_btnSelectTotal->setEnabled(false);
		this->m_btnSelectLast->setEnabled(true);
		this->m_lTotal->setVisible(true);
		this->m_lLast->setVisible(false);
	});
	this->addChild(m_btnSelectTotal);

	m_btnSelectLast = ui::Button::create("image/menu/button_switch.png", "", "image/menu/button_switchDisable.png");
	m_btnSelectLast->setPosition(m_btnSelectTotal->getPosition() + Vec2(-300, 0));
	m_btnSelectLast->setTitleFontSize(53);
	m_btnSelectLast->addClickEventListener([this](Ref*){
		this->m_btnSelectTotal->setEnabled(true);
		this->m_btnSelectLast->setEnabled(false);
		this->m_lTotal->setVisible(false);
		this->m_lLast->setVisible(true);
	});
	m_btnSelectLast->setEnabled(false);
	this->addChild(m_btnSelectLast);


	//clear record
	m_btnClearRecord = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	m_btnClearRecord->setPosition(VisibleRect::rightTop() - m_btnClearRecord->getContentSize() / 2);
	m_btnClearRecord->setTitleFontSize(38);
	m_btnClearRecord->addClickEventListener([this](Ref* ref){
		this->onBtnClearRecord();
		auto btn = (ui::Button*)ref;
		btn->setEnabled(false);
	});
	this->addChild(m_btnClearRecord);

#else

	const char* tableH[2][3] = {
		{ "短", "中", "长" },
		{ "Short", "Medium", "Long" }
	};
	const char* tableV[2][3] = {
		{ "洗+吹", "洗", "吹" },
		{ "W+B", "Wash", "Blow" }
	};


	auto table = MyDrawTable::MyCreate(4, 4, GAME_WIDTH * 4 / 5, GAME_HEIGHT * 4 / 7, 4, Color4F::BLUE);
	this->addChild(table);
	table->setAnchorPoint(Point::ANCHOR_MIDDLE);
	table->setPosition(VisibleRect::center());

	for (int i = 0; i < 3; ++i)
	{
		auto v = createFontTTF("fonts/dai meng wa wa ti.ttf", 53, 2, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
		this->addChild(v);
		v->setPosition(GAME_WIDTH / 5 * (2 + i), GAME_HEIGHT / 7 * 5);
		v->setString(LanguageChoose(tableV[0][i], tableV[1][i]));

		auto h = createFontTTF("fonts/dai meng wa wa ti.ttf", 53, 2, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
		this->addChild(h);
		h->setPosition(GAME_WIDTH / 5, GAME_HEIGHT / 7 * (4 - i));
		h->setString(LanguageChoose(tableH[0][i], tableH[1][i]));
	}

	for (int i=0; i<3; ++i)
	{
		Label* l = Label::createWithSystemFont(StringUtils::format("%d", g_pData->get(eDataKey::note_clean_WB_S + i)), "黑体", 53);
		l->setPosition(GAME_WIDTH*2/5, GAME_HEIGHT*(4-i)/7);
		this->addChild(l);

		Label* w = Label::createWithSystemFont(StringUtils::format("%d", g_pData->get(eDataKey::note_clean_W_S + i)), "黑体", 53);
		w->setPosition(GAME_WIDTH*3/5, GAME_HEIGHT*(4-i)/7);
		this->addChild(w);

		Label* b = Label::createWithSystemFont(StringUtils::format("%d", g_pData->get(eDataKey::note_clean_B_S + i)), "黑体", 53);
		b->setPosition(GAME_WIDTH*4/5, GAME_HEIGHT*(4-i)/7);
		this->addChild(b);
	}
#endif // CAN_CHOOSE_WORK_TIME

	return true;
}

void SettingCleanRecord::onEnter()
{
	SubSettingBase::onEnter();

	updateRecord();

	m_btnSelectTotal->setTitleText(LanguageChoose("总记录", "Total"));
	m_btnSelectLast->setTitleText(LanguageChoose("当前记录", "Current"));

	m_btnClearRecord->setTitleText(LanguageChoose("清除当前记录", "Clear Record"));
	m_btnClearRecord->setEnabled(true);
}

void SettingCleanRecord::updateRecord()
{
	for (int i = 0; i < LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT * 2; ++i)
	{
		auto total = (Label*)m_lTotal->getChildByTag(i);
		total->setString(i<LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT
			? StringUtils::format("%d", g_pData->get(eDataKey::rec_clean_01 + i))
			: CentToChar(g_pData->get(eDataKey::rec_clean_01 + i))
			);

		auto last = (Label*)m_lLast->getChildByTag(i);
		last->setString(i < LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT
			? StringUtils::format("%d", g_pData->get(eDataKey::rec_lastClean_01 + i))
			: CentToChar(g_pData->get(eDataKey::rec_lastClean_01 + i))
			);
	}
}

void SettingCleanRecord::onBtnClearRecord()
{
	int c = 0;
	for (int i = eDataKey::rec_lastClean_01; i <= eDataKey::rec_lastCleanPay_15; ++i)
	{
		if (g_pData->get(i) > 0)
		{
			g_pData->set(i, 0);
			c++;
		}
	}
	if (c > 0)
	{
		updateRecord();
	}
}

