#include "RecordLayer.h"
#include "tool/MyMenuBase.h"
#include "GameData.h"
#include "tool/MyTools.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "verify/VerifyScene.h"

bool RecordLayer::init()
{
	KnobLayerBase::initWithColor(MENU_BACKGROUND_COLOR, GAME_WIDTH, GAME_HEIGHT);

	
	//-------------------------- title -------------------------- 在 onEnter 设置内容
	for (int i = 0; i < 3; ++i)
	{
		m_title[i] = createFontTTF("fonts/dai meng wa wa ti.ttf", 35, 2, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
		this->addChild(m_title[i]);
	}


	//-------------------------- numbers --------------------------
	m_lTotal = Layer::create();
	this->addChild(m_lTotal);
	m_lTotal->setVisible(false);

	m_lLast = Layer::create();
	this->addChild(m_lLast);
	m_lLast->setVisible(true);

	// 出货量
	Layer* lTmp[2] = { m_lTotal, m_lLast };
	for (int i = 0; i < COUNT_PRICE; ++i)
	{
		POINT vh = box_GetPidBit(i);
		for (int k = 0; k < 2; ++k)
		{
			auto bt = ui::Button::create("image2/buy/enable.png");
			lTmp[k]->addChild(bt);
			bt->setTag(i);
			bt->setPressedActionEnabled(false);
			bt->setPosition(box_GetPosition(i) + Vec2(0, 20));
			bt->setScale(box_GetSize(i).width, box_GetSize(i).height);
			setBoxTitleFrame(bt, true);
			bt->setEnabled(false);
		}
	}

	//总收益
	auto totalCoin = Label::createWithSystemFont("", "黑体", 53);
	m_lTotal->addChild(totalCoin);
	totalCoin->setTag(100);

	auto lastCoin = Label::createWithSystemFont("", "黑体", 53);
	m_lLast->addChild(lastCoin);
	lastCoin->setTag(100);

	//货品收益
	auto goodsPay = Label::createWithSystemFont("", "黑体", 53);
	m_lTotal->addChild(goodsPay);
	goodsPay->setTag(101);

	auto lastGoodsPay = Label::createWithSystemFont("", "黑体", 53);
	m_lLast->addChild(lastGoodsPay);
	lastGoodsPay->setTag(101);

	
	//-------------------------- switch between Total and Last --------------------------
	m_btnSelectTotal = ui::Button::create("image/menu/button_switch.png", "", "image/menu/button_switchDisable.png");
	m_btnSelectTotal->setPosition(VisibleRect::leftTop() + Vec2(m_btnSelectTotal->getContentSize().width/2, -m_btnSelectTotal->getContentSize().height/2));
	m_btnSelectTotal->setTitleFontSize(53);
	m_btnSelectTotal->addClickEventListener([this](Ref*){
		this->m_btnSelectTotal->setEnabled(false);
		this->m_btnSelectLast->setEnabled(true);
		this->m_lTotal->setVisible(true);
		this->m_lLast->setVisible(false);
	});
	this->addChild(m_btnSelectTotal);

	m_btnSelectLast = ui::Button::create("image/menu/button_switch.png", "", "image/menu/button_switchDisable.png");
	m_btnSelectLast->setPosition(m_btnSelectTotal->getPosition() + Vec2(300, 0));
	m_btnSelectLast->setTitleFontSize(53);
	m_btnSelectLast->addClickEventListener([this](Ref*){
		this->m_btnSelectTotal->setEnabled(true);
		this->m_btnSelectLast->setEnabled(false);
		this->m_lTotal->setVisible(false);
		this->m_lLast->setVisible(true);
	});
	m_btnSelectLast->setEnabled(false);
	this->addChild(m_btnSelectLast);



	//-------------------------- remain coin --------------------------
	m_remainCoin = Label::createWithSystemFont("xxx", "Helvetica", 60);
	m_remainCoin->setPosition(VisibleRect::rightBottom() + Vec2(-150, 130));
	this->addChild(m_remainCoin);


	//-------------------------- button --------------------------
	//back
	m_btnBack = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	m_btnBack->setPosition(VisibleRect::leftBottom() + m_btnBack->getContentSize() / 2);
	m_btnBack->setTitleFontSize(38);
	m_btnBack->addClickEventListener([this](Ref*){
		this->onBtnBack();
	});
	this->addChild(m_btnBack);


	//clear coin
	m_btnClearCoin = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	m_btnClearCoin->setPosition(VisibleRect::rightBottom() + Vec2(-m_btnClearCoin->getContentSize().width / 2, m_btnClearCoin->getContentSize().height / 2));
	m_btnClearCoin->setTitleFontSize(38);
	m_btnClearCoin->addClickEventListener([this](Ref*){
		this->onBtnClearCoin();
	});
	this->addChild(m_btnClearCoin);


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



#if (ROTATE_SCREEN == 1)
	m_title[0]->setPosition(Point(GAME_WIDTH / 2, GAME_HEIGHT - 40));//投币数
	m_title[1]->setPosition(Point(GAME_WIDTH / 2, GAME_HEIGHT - 230));//出货量
#else
	m_title[0]->setPosition(GAME_WIDTH/6, 130);//洗烘收益
	m_title[1]->setPosition(VisibleRect::top() + Vec2(0, -90));//货品收益
	m_title[2]->setPosition(GAME_WIDTH * 2 / 3, 130);//剩余金额
#endif
	totalCoin->setPosition(m_title[0]->getPosition() + Vec2(200, 0));
	lastCoin->setPosition(m_title[0]->getPosition() + Vec2(200, 0));

	goodsPay->setPosition(m_title[1]->getPosition() + Vec2(200, 0));
	lastGoodsPay->setPosition(m_title[1]->getPosition() + Vec2(200, 0));

	//verify 打码
#ifdef USE_VERIFY
	m_btnVerify = ui::Button::create("image/menu/button_switch.png", "image/menu/button_switchDisable.png");
	m_btnVerify->setPosition(VisibleRect::bottom() + Vec2(0, m_btnVerify->getContentSize().height/2));
	m_btnVerify->setTitleFontSize(33);
	m_btnVerify->setTitleColor(Color3B::BLUE);
	m_btnVerify->addClickEventListener([this](Ref*){
		this->onBtnVerify();
	});
	this->addChild(m_btnVerify);
#endif // USE_VERIFY

	m_pItems[0] = m_btnClearRecord;
	m_pItems[1] = m_btnClearCoin;
	m_pItems[2] = m_btnBack;
	m_pItems[3] = m_btnVerify;

	m_iCurIndex = 2;

	return true;
}

void RecordLayer::onEnter()
{
	KnobLayerBase::onEnter();

	m_title[0]->setString(LanguageChoose("总收益:", "Total Pay:"));
	m_title[1]->setString(LanguageChoose("货品收益", "Goods Pay"));
	m_title[2]->setString(LanguageChoose("剩余金额:", "Remain:"));

	updateRecord();
	updateRemainCoin();

	m_btnSelectTotal->setTitleText(LanguageChoose("总记录", "Total"));
	m_btnSelectLast->setTitleText(LanguageChoose("当前记录", "Current"));

	m_btnBack->setTitleText(LanguageChoose("返回", "Back"));
	m_btnClearCoin->setTitleText(LanguageChoose("剩余归零", "Clear Remain"));
	m_btnClearRecord->setTitleText(LanguageChoose("清除当前记录", "Clear Record"));
#ifdef USE_VERIFY
	m_btnVerify->setTitleText(LanguageChoose("打码查询", "Check Verify"));
#endif // USE_VERIFY
	
	m_btnClearRecord->setEnabled(true);

// 	scheduleUpdate();
}

void RecordLayer::updateRemainCoin()
{
	m_remainCoin->setString(CentToChar(dataGet_RemainCent()));
}

void RecordLayer::updateRecord()
{
	for (int i = 0; i < COUNT_PRICE; ++i)
	{
		POINT vh = box_GetPidBit(i);
		auto total = (ui::Button*)m_lTotal->getChildByTag(i);
		total->setTitleText(StringUtils::format("%c%d\n%d", 'A' + vh.x, vh.y + 1, g_pData->get(eDataKey::rec_EggOut_01 + i)));
		setBoxTitleFrame(total, false);

		auto last = (ui::Button*)m_lLast->getChildByTag(i);
		last->setTitleText(StringUtils::format("%c%d\n%d", 'A' + vh.x, vh.y + 1, g_pData->get(eDataKey::rec_LastEggOut_01 + i)));
		setBoxTitleFrame(last, false);
	}

	//总收益
	((Label*)m_lTotal->getChildByTag(100))->setString(CentToChar(g_pData->get(eDataKey::rec_CentIn)));
	((Label*)m_lLast->getChildByTag(100))->setString(CentToChar(g_pData->get(eDataKey::rec_LastCentIn)));

	//货品收益
	((Label*)m_lTotal->getChildByTag(101))->setString(CentToChar(g_pData->get(eDataKey::rec_CentFromGoods)));
	((Label*)m_lLast->getChildByTag(101))->setString(CentToChar(g_pData->get(eDataKey::rec_LastCentFromGoods)));
}

void RecordLayer::onBtnClearCoin()
{
	if (dataSet_ClearCent())
	{
		this->updateRemainCoin();
		g_pIoCtrlLayer->updateCoinShow();
	}
}

void RecordLayer::onBtnClearRecord()
{
	int c = 0;
	for (int i = eDataKey::rec_LastCentIn; i < eDataKey::rec_LastEggOut_01 + COUNT_PRICE; ++i)
	{
		if (g_pData->get(i) > 0)
		{
			g_pData->set(i, 0);
			this->updateRemainCoin();
			c++;
		}
	}
	if (c > 0)
	{
		updateRecord();
	}
}

void RecordLayer::onBtnBack()
{
	static_cast<LayerMultiplex*>(this->_parent)->switchTo(1);
}

void RecordLayer::onBtnVerify()
{
	Director::getInstance()->replaceScene(HorizontalScreen(Scene_Verify::createScene()));
}

#ifdef USE_KNOB
void RecordLayer::update(float delta)
{
	KnobLayerBase::update(delta);

	if (g_pIoCtrlLayer->isKeyDown(IoCtrlLayer::e_key_fire, 0.5f))
	{
		switch (m_iCurIndex)
		{
		case 0:
			onBtnClearRecord();
			break;
		case 1:
			onBtnClearCoin();
			break;
		case 2:
			onBtnBack();
			break;
		case 3:
			onBtnVerify();
			break;
		}
	}
}
#endif // USE_KNOB
