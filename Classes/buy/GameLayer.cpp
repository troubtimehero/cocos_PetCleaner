#include "GameLayer.h"
#include "ioCtrl/ComThread.h"
#include "tool/MyTools.h"
#include "MachineDefined.h"
#include "GameData.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "ioCtrl/LeYaoYao.h"
#include "BuyByGame.h"


//////////////////////////////////////////////////////////////////////////

RollGame* RollGame::MyCreate(Node** pElements, int nElement, float fDistY, float speedSpeed)
{
	RollGame* ret = new (std::nothrow) RollGame();
	if (ret && ret->init(pElements, nElement, fDistY, speedSpeed))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}

	return ret;
}


#define SPEED_MAX	5000

RollGame::~RollGame()
{
	delete[] pElementArr;
	delete pRc;
}

bool RollGame::init(Node** pElements, int nElement, float distY, float speedSpeed)
{
	Layer::init();

	nElementCount = nElement;
	pElementArr = new Node*[nElementCount];
	for (int i = 0; i < nElementCount; ++i)
	{
		pElementArr[i] = pElements[i];
	}
	fDistY = distY;
	fSpeedOfSpeed = speedSpeed;

	bRun = false;
	iCurIndex = -1;
	iLastIndex = -1;
	iNextIndex = -1;
	fCurY = 0;
	fSpeedY = 100;

	int* indexs = new int[nElementCount];
	for (int i = 0; i < nElementCount; ++i)
	{
		indexs[i] = i;
	}
	pRc = new RandomController<int, 1>(indexs, nElementCount);
	delete[] indexs;


	for (int i = 0; i < nElement; ++i)
	{
		pElementArr[i]->setVisible(false);
		pElementArr[i]->setPositionX(0);
		this->addChild(pElementArr[i]);
	}


	nextIndex();
	nextIndex();
	nextIndex();
	render();

	scheduleUpdate();

	return true;
}

void RollGame::nextIndex()
{
	iLastIndex = iCurIndex;
	iCurIndex = iNextIndex;
	do
	{
		iNextIndex = pRc->GetRandom();
	} while (iNextIndex == iCurIndex || iNextIndex == iLastIndex);
}

void RollGame::update(float delta)
{
	if (bRun)
	{
		fSpeedY += fSpeedOfSpeed;
		if (fSpeedY >= SPEED_MAX)
			fSpeedY = SPEED_MAX;

		fCurY += fSpeedY * delta;
		if (fCurY >= fDistY / 2)
		{
			fCurY -= fDistY;
			nextIndex();
		}

		render();
	}
}

void RollGame::Start(int winIndex)
{
	if(!bRun)
		iForceIndex = winIndex;
	bRun = true;
}

bool RollGame::Stop()
{
	if (bRun && fSpeedY >= SPEED_MAX)
	{
		if (iForceIndex >= 0)
		{
			while (iCurIndex != iForceIndex)
			{
				nextIndex();
			}
		}

		fCurY = 0;
		fSpeedOfSpeed = 0;
		fSpeedY = 0;

		return true;
	}

	return false;
}

void RollGame::render()
{
	for (int i = 0; i < nElementCount; ++i)
	{
		pElementArr[i]->setVisible(false);
	}

	pElementArr[iLastIndex]->setVisible(true);
	pElementArr[iLastIndex]->setPositionY(fCurY + fDistY);

	pElementArr[iCurIndex]->setVisible(true);
	pElementArr[iCurIndex]->setPositionY(fCurY);

	pElementArr[iNextIndex]->setVisible(true);
	pElementArr[iNextIndex]->setPositionY(fCurY - fDistY);
}


//////////////////////////////////////////////////////////////////////////


#define CUR_Y_DIST	300

RollGameMgr* RollGameMgr::MyCreate(RollGame** pRoll, int nCount)
{
	RollGameMgr* ret = new (std::nothrow) RollGameMgr();
	if (ret && ret->init(pRoll, nCount))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}

	return ret;
}

RollGameMgr::~RollGameMgr()
{
	delete[] m_pRolls;
}

bool RollGameMgr::init(RollGame** pRoll, int nCount)
{
	Layer::init();

	m_nCount = nCount;
	m_pRolls = new RollGame*[nCount];
	for (int i = 0; i < nCount; ++i)
	{
		m_pRolls[i] = pRoll[i];
	}

	for (int i = 0; i < nCount; ++i)
	{
		m_pRolls[i]->setPosition(GAME_WIDTH / 2 + 200 * (i - 1), GAME_HEIGHT / 2);
		this->addChild(m_pRolls[i]);
	}

	return true;
}

void RollGameMgr::Start(int iWinIndex /*= -1*/)
{
	for (int i = 0; i < m_nCount; ++i)
	{
		m_pRolls[i]->Start(i);
	}
	m_nStoped = 0;
}

bool RollGameMgr::Stop()
{
	if (m_nStoped >= m_nCount)
		return false;

	for (int i = 0; i < m_nCount; ++i)
	{
		if (m_pRolls[i]->Stop())
		{
			m_nStoped++;
			break;
		}
	}
	if (m_nStoped >= m_nCount)
		return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////

AlgWinType::eType AlgWinType::algType(int wantIndex)
{
	return e_none;
}


//////////////////////////////////////////////////////////////////////////


GameLayer* GameLayer::MyCreate(int wantIndex)
{
	GameLayer* ret = new (std::nothrow) GameLayer();
	if (ret && ret->init(wantIndex))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}

	return ret;
}

bool GameLayer::init(int wantIndex)
{
	m_winType = AlgWinType::algType(wantIndex);

	LayerColor::initWithColor(Color4B::GRAY, GAME_WIDTH, GAME_HEIGHT);
	DisableBelowTouch(this);

	this->setName(LAYER_NAME_GAME_ING);
	this->setAnchorPoint(Point::ANCHOR_MIDDLE);
	this->setScale(5);
	this->setOpacity(0);
	this->runAction(Spawn::createWithTwoActions(ScaleTo::create(0.5f, 1), FadeIn::create(0.5f)));


	//////////////////////////////////////////////////////////////////////////
	// 游戏
	const int nRoll = 3;
	const int nElement = 8;
	Label* rolls[nRoll][nElement] = { nullptr };
	RollGame* rg[nRoll] = { nullptr };

	for (int i = 0; i < nRoll; ++i)
	{
		for (int k = 0; k < nElement; ++k)
		{
			rolls[i][k] = Label::createWithSystemFont(StringUtils::format("%d", k + 1), "Arral", 150);
		}

		rg[i] = RollGame::MyCreate((Node**)rolls[i], nElement, 100, 1000);
	}

	m_rgm = RollGameMgr::MyCreate(rg, nRoll);
	this->addChild(m_rgm);


	//开始停止
	m_btn = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	m_btn->setTitleFontSize(75);
	m_btn->setTitleText("START");
	this->addChild(m_btn);
	m_btn->setPosition(VisibleRect::bottom() + m_btn->getContentSize());
	m_btn->addClickEventListener([this](Ref* ref){
		this->callbackClick();
	});
	m_bRun = false;


	m_eStatus = e_ready;
	m_fDelay = 0;
	scheduleUpdate();

	return true;
}

void GameLayer::update(float delta)
{
	m_fDelay -= delta;
	if (m_fDelay > 0)
		return;

	switch (m_eStatus)
	{
	case GameLayer::e_ready:
		if (this->getScale() <= 1)
			m_eStatus = e_playing;
		break;

	case GameLayer::e_playing:
		break;

	case GameLayer::e_stop:
		m_fDelay = 3;
		if (m_winType != AlgWinType::e_none)
		{
			m_eStatus = e_win;
			MySound::playEffect("sound/a_win.mp3");
		}
		else
		{
			m_eStatus = e_loss;
			MySound::playEffect("sound/lose.mp3");
		}
		break;

	case GameLayer::e_win:
		switch (m_winType)
		{
		case AlgWinType::e_box:
			((BuyByGame*)_parent)->callWin();	//中奖
			break;

			//--------------- 中币 ---------------
		case AlgWinType::e_coin_1:
			break;
		case AlgWinType::e_coin_2:
			break;
		case AlgWinType::e_coin_5:
			break;
		case AlgWinType::e_coin_10:
			break;
		case AlgWinType::e_coin_20:
			break;
		}
		//不能 break

	case GameLayer::e_loss:
		this->runAction(Sequence::create(DelayTime::create(1), FadeOut::create(0.2f), RemoveSelf::create(), nullptr));
		m_eStatus = e_end;

		do{
			MyBtnTimeoutBack* b = (MyBtnTimeoutBack*)_parent->getChildByName(BUTTON_NAME_BACK_TIMEOUT);
			if (b)
			{
				b->resetTimeout(CONTROL_TIMEOUT);
			}
		} while (false);
		break;

	case GameLayer::e_end:	//只是等待结束
		break;
	}
}

void GameLayer::callbackClick()
{
	if (e_playing != m_eStatus)
		return;

	if (m_bRun)
	{
		if (m_rgm->Stop())
		{
			m_eStatus = e_stop;
			m_fDelay = 0.5f;
		}
	}
	else
	{
		m_rgm->Start();
		m_bRun = true;
		m_btn->setTitleText("STOP");
	}
}


