#include "IoCtrlLayer.h"
#include "GameData.h"
#include "tool/MyTools.h"
#include "ComThread.h"
#include "menu/MenuScene.h"
#include "helloScene/HelloWorldScene.h"
#include "LeYaoYao.h"
#include "QRCode/QRSprite.h"

CComThread*		g_pCom = nullptr;
io_lyy_Pet*	g_pBox = nullptr;
LeYaoYao*		g_pLeyaoyao = nullptr;
#if	(VERSION_BUILD_NUMBER==2)
static bool bDebugIo = true;
#else
static bool bDebugIo = false;
#endif

extern bool g_bWorking;


void IoCtrlLayer::onPasswordOK(std::string password)
{
	myCallMenu();
}

IoCtrlLayer::IoCtrlLayer()
{
	//IO板
	g_pCom = new CComThread(IO_USED_COUNT);
	g_pCom->SetEnable(true, true);

	//监控盒子
	int port = MyIniGetInt("SETTING", USER_DEF_COM_BOX, 2);
	g_pBox = new io_lyy_Pet(port);

	c_iniLockOpeningTime = MyIniGetInt("SETTING", "MOTOR_TIME", 8);
	if (c_iniLockOpeningTime <= 0)
		c_iniLockOpeningTime = 10;
	m_fLockOpeningTime = 0;

	for (int i = 0; i < ARRAYSIZE(m_keyKeep); ++i)
	{
		m_keyDown[i] = false;
		m_keyKeep[i] = false;
	}

	m_nTick = 0;

	m_bLockError = false;
	m_iMotorIndex = 0;

	//音乐
	m_music = new CGameMusic;
	m_music->init();
	m_fMusicDelay = 1;

	//要放最后
	init();
}

IoCtrlLayer::~IoCtrlLayer()
{
	SAFE_DELETE_POINT(g_pCom);
	SAFE_DELETE_POINT(g_pBox);
	SAFE_DELETE_POINT(m_music);
}

bool IoCtrlLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	this->setCascadeColorEnabled(true);
	this->setCascadeOpacityEnabled(true);

	//键盘监测
	auto listenerkeyPad = EventListenerKeyboard::create();
	listenerkeyPad->onKeyReleased = CC_CALLBACK_2(IoCtrlLayer::onKeyReleased, this);
	listenerkeyPad->onKeyPressed= CC_CALLBACK_2(IoCtrlLayer::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerkeyPad, this);


	//乐摇摇
	g_pLeyaoyao = LeYaoYao::create();
	this->addChild(g_pLeyaoyao);


	//退扭蛋错误
#ifdef SINGLE_EGG_OUT
	m_eggError = Sprite::create("image/tips/egg_over.png");
	this->addChild(m_eggError);
	m_eggError->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
	m_eggError->setPosition(VisibleRect::rightTop() - Vec2(0, -100));
	m_eggError->runAction(Sequence::createWithTwoActions(ScaleTo::create(0.1f, 0.5f), ScaleTo::create(1, 1)));
	m_eggError->setVisible(false);
#endif // SINGLE_EGG_OUT


	//退票错误
	m_tickError = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
	this->addChild(m_tickError);
	m_tickError->setVisible(false);
	m_tickError->setPosition(VisibleRect::rightTop() - Vec2(380, 40));
	//正在退票数
	m_tickNum = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
	this->addChild(m_tickNum);
	m_tickNum->setVisible(false);
	m_tickNum->setPosition(VisibleRect::rightTop() - Vec2(120, 40));


	//正在出货
	m_boxOutting = LayerColor::create(Color4B(255, 255, 255, 128), GAME_WIDTH, GAME_HEIGHT/4);
	this->addChild(m_boxOutting);
	m_boxOutting->setZOrder(10);
	m_boxOutting->setVisible(false);

	Label* boxOuttingChar = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::WHITE, Color4B::BLACK, Color4B::RED, true);
	m_boxOutting->addChild(boxOuttingChar);
	boxOuttingChar->setName("boxOuttingChar");
	boxOuttingChar->setPosition(GAME_WIDTH / 2, GAME_HEIGHT / 8);



	//控制板错误
	m_errorMcu = Label::createWithTTF("Lost IO Control", "fonts/Marker Felt.ttf", 60);
	//createWithSystemFont("Lost IO Control", "黑体", 40);
	m_errorMcu->setPosition(VisibleRect::center());
	m_errorMcu->setZOrder(12);
	m_errorMcu->setVisible(false);
	this->addChild(m_errorMcu);

	m_lstKnobShakeRemain.clear();


	return true;
}

void IoCtrlLayer::onEnter()
{
	Layer::onEnter();

	scheduleUpdate();

	schedule(schedule_selector(IoCtrlLayer::timerBlinkInsertAndTick), 0.75f, CC_REPEAT_FOREVER, 0);

	updateCoinShow();

	m_tickError->setString(LanguageChoose("剩余彩票:", "Remain Ticket:"));
	((Label*)m_boxOutting->getChildByName("boxOuttingChar"))->setString(LanguageChoose("正在出货", "Outting Goods"));
}

void IoCtrlLayer::onExit()
{
	Layer::onExit();
}

void IoCtrlLayer::update(float delta)
{
	Layer::update(delta);

	g_pCom->Update();

	static unsigned char errStaLast = 0;
	unsigned char errSta = ioErrorStatus();
	if (!bDebugIo)	errSta = 0;
	m_errorMcu->setVisible(errSta > 0);
	if (errSta > 0)
	{
		if (errSta != errStaLast)
		{
			g_pBox->BoxReportStatus(io_LYY_Good::eBoxStatus::e_box_error);
		}

		if (IO_USED_COUNT > 1 && errSta != errStaLast)
		{
#if 0
			for (int i = 0; i < IO_USED_COUNT; ++i)
			{
				if (errSta & (1 << i))
				{
					if (!m_errorMcu->getChildByTag(i))
					{
						Label* l = Label::createWithTTF(StringUtils::format("COM %d: off line", i + 1), "fonts/Marker Felt.ttf", 60);
						m_errorMcu->addChild(l);
						m_errorMcu->setTag(i);
						l->setPosition(150, -100 * (i + 1));
					}
					else
					{
						m_errorMcu->getChildByTag(i)->setVisible(true);
					}
				}
				else
				{
					if (m_errorMcu->getChildByTag(i))
						m_errorMcu->getChildByTag(i)->setVisible(false);
				}
			}
#else
			string str = StringUtils::format("COM 1: %s,  COM 2: %s", errSta & 0x01 ? "off" : "on", errSta & 0x02 ? "off" : "on");
			m_errorMcu->setString(str);
#endif
		}

		//-------------------- 屏蔽下层触摸 -----------------------
		if (!getChildByName("IoLost"))
		{
			float w = GAME_WIDTH;
			float h = GAME_HEIGHT * 0.3f;
			auto lLostCtrl = LayerColor::create(Color4B(255, 0, 0, 200), w, h);
			lLostCtrl->setPosition(0, GAME_HEIGHT*0.35f);
			DisableBelowTouch(lLostCtrl);
			this->addChild(lLostCtrl, 0, "IoLost");
		}
	}
	else
	{
		if (errSta != errStaLast)
		{
			g_pBox->BoxReportStatus(io_LYY_Good::eBoxStatus::e_box_free);
		}

		auto l = getChildByName("IoLost");
		if (l)
			l->removeFromParent();
	}
	errStaLast = errSta;


	//投币状态
	if (g_pCom->GetCoinIn() > 0)
	{
		myAddCent(g_pCom->GetCoinIn() * CENT_P_COIN);
	}

	if (dataIs_CoinChange())
	{
		dataIs_CoinChange() = false;
		updateCoinShow();
	}

	//按键相应操作
	if (g_pCom->CheckSpace(CComThread::e_addr_p2, CComThread::e_bit_POWER, 1))
	{
		if (!this->getChildByName("password"))
		{
			this->addChild(LayerPassword::MyCreate(this, "01010101"), 99999, "password");
		}
	}
	else if (!g_bWorking && g_pBox->willEnterSetting())
	{
		this->removeChildByName("password");
		myCallMenu();
		g_pBox->clearEnterSettingSignal();
	}

// 	if (g_pCom->CheckSpace(CComThread::e_addr_mgr, CComThread::e_bit_SCORE, 1))
// 	{
// 		g_pCom->ClearOuting();
// 	}

// 	if (g_pCom->CheckSpace(CComThread::e_addr_mgr, CComThread::e_bit_ADD, 1))
// 	{
// 		g_pCom->RetryOuting();
// 	}


	//按键状态清除
	for (int i = 0; i < ARRAYSIZE(m_keyDown); ++i)
	{
		m_keyDown[i] = false;
	}

	//振动旋钮自动停止
	for (auto it = m_lstKnobShakeRemain.begin(); it != m_lstKnobShakeRemain.end(); )
	{
		it->time -= delta;
		if (it->time <= 0)
		{
			g_pCom->SetDevOut(it->pid, 2, false);
			it = m_lstKnobShakeRemain.erase(it);
		}
		else
		{
			it++;
		}
	}


	m_bLockError = false;
	//----------------------- 关锁、电机停止 ------------------------
	static float sfCheckMotor = MyIniGetInt("SETTING", "MOTOR_CHECK", 3) * 0.1f;
	if (m_fLockOpeningTime > 0)
	{
		m_fLockOpeningTime -= delta;

		if (m_fLockOpeningTime <= 0)	//超时错误，微动或电机故障
		{
			m_bLockError = true;


			//出货失败，退款、退钱
			g_pLeyaoyao->UpTrade(false);
			//客服二维码，正在退款，请稍等，如果有问题请联系客服
			LayerColor* showErrorHelp = LayerColor::create(Color4B::WHITE, GAME_WIDTH, GAME_HEIGHT);
			this->addChild(showErrorHelp);
			DisableBelowTouch(showErrorHelp);
			showErrorHelp->setZOrder(11);
#ifdef _DEBUG
			float fShowErrorTime = 2;
#else
			float fShowErrorTime = 30;
#endif // _DEBUG
			showErrorHelp->runAction(Sequence::createWithTwoActions(DelayTime::create(fShowErrorTime), RemoveSelf::create()));

			Label* l = createFontTTF("fonts/dai meng wa wa ti.ttf", 60, 5, Color4B::YELLOW, Color4B::BLACK, Color4B::BLACK, true);
			l->setString(LanguageChoose("出货失败\n正在退款", "Out Goods Failed\nNow Refund"));
			l->setPosition(VisibleRect::center() + Vec2(0, 130));
			showErrorHelp->addChild(l);

			Label* l2 = createFontTTF("fonts/dai meng wa wa ti.ttf", 25, 2, Color4B::YELLOW, Color4B::BLACK, Color4B::BLACK, true);
			l2->setString(LanguageChoose("如有疑问请扫码联系客服", "In case of doubt,\nplease scan the code to contact customer service"));
			l2->setPosition(VisibleRect::bottom() + Vec2(0, 50));
			showErrorHelp->addChild(l2);

			
			DrawNode* qr = QRSprite::create(MyIniGetString("SETTING", "KE_FU", "http://weixin.qq.com/r/H0SWkvzE7zSTKbjxbxGb").c_str());
			showErrorHelp->addChild(qr);
			qr->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
			qr->setPosition(VisibleRect::bottom() + Vec2(0, 120));
		}
		

		bool bFin = false;
#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
		bFin = g_pCom->CheckUpDelay((CComThread::eBtnAddr)def_btnMotor[m_iMotorIndex].pid, (CComThread::eBtnBit)def_btnMotor[m_iMotorIndex].bit, sfCheckMotor); //微动检测转完一圈
		if (bFin)
		{
			//出货成功
			g_pLeyaoyao->UpTrade(true);
		}
#endif

		if (m_fLockOpeningTime <= 0 || bFin)	//时间到自动关闭
		{
			m_fLockOpeningTime = 0;
			g_pCom->CloseAllDev();
		}
	}

	
	//正在出货
	m_boxOutting->setVisible(m_fLockOpeningTime > 0);


	//音乐
#ifndef _DEBUG
	if (!m_music->isPlaying())
	{
		m_fMusicDelay -= delta;
		if (m_fMusicDelay <= 0)
		{
			m_fMusicDelay = 0.5f;
			m_music->play();
		}
	}
#endif // _DEBUG
}

void IoCtrlLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
#if (VERSION_BUILD_NUMBER==2)
//	system("shutdown -r -t 0");
#endif

#ifdef FORBID_KEYBOARD
// 	MessageBox(StringUtils::format("%d", keyCode).c_str(), "onKeyReleased");
	return;
#endif // FORBID_KEYBOARD

	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_ESCAPE:
		Director::getInstance()->end();
		break;

	case EventKeyboard::KeyCode::KEY_Z:
		if(this->getParent())
		{
			SceneMgr* scene = (SceneMgr*)this->getParent();
			scene->captureScreen2();
		}
		break;

	case EventKeyboard::KeyCode::KEY_SPACE:
		if (Director::getInstance()->isPaused())
		{
			Director::getInstance()->resume();
		}
		else
		{
			Director::getInstance()->pause();
		}
		break;

	case EventKeyboard::KeyCode::KEY_KP_PLUS:
		myAddCent(1);
		break;

	case EventKeyboard::KeyCode::KEY_R:
		g_pCom->RetryOuting();

	case EventKeyboard::KeyCode::KEY_KP_MINUS:
		myClearCoin();
		break;

	case EventKeyboard::KeyCode::KEY_S:
		if (!this->getChildByName("password"))
		{
			this->addChild(LayerPassword::MyCreate(this, "111"), 99999, "password");
		}
		break;

	case EventKeyboard::KeyCode::KEY_V:
		g_pIoCtrlLayer->setVisible(!g_pIoCtrlLayer->isVisible());
		break;

		//------------- 旋钮 -------------
	case EventKeyboard::KeyCode::KEY_ENTER:
		m_keyDown[0] = true;
		m_keyKeep[0] = false;
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		m_keyDown[1] = true;
		m_keyKeep[1] = false;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		m_keyDown[2] = true;
		m_keyKeep[2] = false;
		break;

	case EventKeyboard::KeyCode::KEY_L:
		bDebugIo = !bDebugIo;
		break;

#if 0
	case EventKeyboard::KeyCode::KEY_1:
		dataOutEgg(0, 1);
		break;
	case EventKeyboard::KeyCode::KEY_2:
		dataOutEgg(1, 1);
		break;
	case EventKeyboard::KeyCode::KEY_3:
		dataOutEgg(2, 1);
		break;
	case EventKeyboard::KeyCode::KEY_4:
		dataOutEgg(3, 1);
		break;
	case EventKeyboard::KeyCode::KEY_5:
		dataOutEgg(4, 1);
		break;
	case EventKeyboard::KeyCode::KEY_6:
		dataOutEgg(5, 1);
		break;
	case EventKeyboard::KeyCode::KEY_7:
		dataOutEgg(6, 1);
		break;
	case EventKeyboard::KeyCode::KEY_8:
		dataOutEgg(7, 1);
		break;
#endif
	}

// 	g_pCom->SetKeyLogic(keyCode);

}

void IoCtrlLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
#if (VERSION_BUILD_NUMBER==2)
//	system("shutdown -r -t 0");
#endif

#ifdef FORBID_KEYBOARD
	// 	MessageBox(StringUtils::format("%d", keyCode).c_str(), "onKeyReleased");
	return;
#endif // FORBID_KEYBOARD

	switch (keyCode)
	{
		//------------- 旋钮 -------------
	case EventKeyboard::KeyCode::KEY_ENTER:
		m_keyKeep[0] = true;
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		m_keyKeep[1] = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		m_keyKeep[2] = true;
		break;
	}
}

void IoCtrlLayer::updateCoinShow()
{
}

void IoCtrlLayer::myAddCent(int cent, bool bNet)
{
	dataSet_AddCent(cent, bNet);
	MySound::playEffect("sound/CoinIn.mp3");
}

void IoCtrlLayer::myClearCoin()
{
	dataSet_ClearCent();
}

void IoCtrlLayer::myCallMenu()
{
	if (getTag() == e_scene_game)
	{
		Director::getInstance()->replaceScene(TransitionJumpZoom::create(0.3f, HorizontalScreen(Scene_Menu::createScene())));
	}
}

void IoCtrlLayer::myAddRemainCoin(int coin)
{//增加玩家币数，返还，不加记录
	dataSet_AddCentByGame(coin);
	MySound::playEffect("sound/CoinIn.mp3");
}

void IoCtrlLayer::useCoinByGame(int index)
{
	dataUse_PlayGame();	//扣币
	MySound::playEffect("sound/use_coin.mp3");
}

bool IoCtrlLayer::isKeyDown(eKeyTag key, float fSpace)
{
	switch (key)
	{
	case IoCtrlLayer::e_key_fire:
		if (fSpace > 0)
		{
			if (g_pCom->CheckSpace(CComThread::eBtnAddr::e_addr_p1, CComThread::e_bit_FIRE, fSpace))
				return true;
		}
		else
		{
			if (g_pCom->CheckOnce(CComThread::eBtnAddr::e_addr_p1, CComThread::e_bit_FIRE))
				return true;
		}
		break;
	case IoCtrlLayer::e_key_left:
		if (g_pCom->GetKnob(CComThread::e_knob_l) > 0)
			return true;
		break;
	case IoCtrlLayer::e_key_right:
		if (g_pCom->GetKnob(CComThread::e_knob_r) > 0)
			return true;
		break;
	}

#ifdef _DEBUG
	return m_keyDown[key];
#endif // _DEBUG

	return false;
}

bool IoCtrlLayer::isKeyKeep(eKeyTag key)
{
	switch (key)
	{
	case IoCtrlLayer::e_key_fire:
#if 1	//门检测开关，信号是相反的，关门时是断开
		if (!g_pCom->Check(CComThread::e_addr_p1, CComThread::e_bit_FIRE))
#else
		if (g_pCom->Check(CComThread::e_addr_p1, CComThread::e_bit_FIRE))
#endif
			return true;
		break;
#if 1
	case IoCtrlLayer::e_key_left:
		if (g_pCom->GetKnob(CComThread::e_knob_l) > 0)
			return true;
		break;
	case IoCtrlLayer::e_key_right:
		if (g_pCom->GetKnob(CComThread::e_knob_r) > 0)
			return true;
		break;
#else
	case IoCtrlLayer::e_key_left:
		if (g_pCom->Check(CComThread::e_bit_LEFT))
			return true;
		break;
	case IoCtrlLayer::e_key_right:
		if (g_pCom->Check(CComThread::e_bit_RIGHT))
			return true;
		break;
#endif
	}

#ifdef _DEBUG
	return m_keyKeep[key];
#endif // _DEBUG

	return false;
}

void IoCtrlLayer::timerBlinkInsertAndTick(float delta)
{
#if (VERSION_BUY_TYPE == BUY_NONE)
	//投币提示
	if (getTag() == e_scene_other || dataGet_RemainCent() > 0 || EG_Base::isGamePlaying())
	{
		m_insert->setVisible(false);
	}
	else
	{
		m_insert->setVisible(!m_insert->isVisible());
	}

	//退票提示
	{
		if (m_nTick != g_pCom->GetTicketOut(0))
		{
			m_nTick = g_pCom->GetTicketOut(0);
			m_tickNum->setString(GBKToUTF(StringUtils::format("%d", m_nTick)));
		}
		m_tickNum->setVisible(m_nTick > 0);

		m_tickError->setVisible(g_pCom->IsTickOutError(0));
#ifdef SINGLE_EGG_OUT
		m_eggError->setVisible(g_pCom->IsEggOutError(0));
#endif // SINGLE_EGG_OUT
	}
#endif
}

void IoCtrlLayer::knobShake(int pid, float time)
{
#if ( VERSION_BUY_TYPE != BUY_EGG && VERSION_BUY_TYPE != BUY_GIFT)
	g_pCom->SetDevOut(pid, 2, true);
	m_lstKnobShakeRemain.push_back({ pid, time });
#endif
}

void IoCtrlLayer::lightFire(int pid, bool bOn)
{
#if ( VERSION_BUY_TYPE != BUY_EGG && VERSION_BUY_TYPE != BUY_GIFT)
	g_pCom->SetDevOut(pid, 3, bOn);
#endif
}

void IoCtrlLayer::openLock(int v, int h, float openTime)
{
	if (m_fLockOpeningTime > 0)
	{
		CCLOG("Error: 之前的锁还没关闭");
		return;
	}
	
	
#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
	m_iMotorIndex = box_GetIndex(v, h);
	ioOpenMotor(m_iMotorIndex);
	m_fLockOpeningTime = c_iniLockOpeningTime;	// cyhol 根据电机转动速度调节，用于防止微动坏 [11/22/2018 pc-ah]
#else
	ioOpenLock(v, h);
	m_fLockOpeningTime = 0.2f;
#endif

	if (openTime > 0)
	{
		m_fLockOpeningTime = openTime;
	}
}

bool IoCtrlLayer::isLockOpening()
{
	return m_fLockOpeningTime > 0;
}

bool IoCtrlLayer::isLockError()
{
	return m_bLockError;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
#if 0
void SceneMgr::popScene(cocos2d::Scene* s, int tag)
{
	Director::getInstance()->popScene();
	Director::getInstance()->getRunningScene()->addChild(g_pIoCtrlLayer);
	g_pIoCtrlLayer->setTag(tag);
	g_pIoCtrlLayer->setZOrder(10001);
}
#endif

