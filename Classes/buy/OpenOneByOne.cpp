#include "OpenOneByOne.h"
#include "ioCtrl/ComThread.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "tool/MyFont.h"

Scene* OpenOneByOne::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = OpenOneByOne::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool OpenOneByOne::init()
{
	if (!Layer::init())	// cyhol Í¸Ã÷²ã [9/20/2017 pc-ah]
		return false;

	//-------------------------- IconRoll --------------------------
	for (int i = 0; i < COUNT_BOX; ++i)
	{
		auto btn = m_spr[i] = Sprite::create("image2/buy/enable.png");
		this->addChild(btn);
		btn->setPosition(box_GetPosition(i));
		btn->setScale(box_GetSize(i).width, box_GetSize(i).height);
		btn->setColor(Color3B::GRAY);
	}

	g_pCom->CloseAllDev();
	m_curIndex = 0;
	m_step = 0;
	schedule(schedule_selector(OpenOneByOne::timerOpenNext), 0.1f, COUNT_BOX * 4, 0.5f);

	return true;
}

void OpenOneByOne::timerOpenNext(float delta)
{
	int v = 0;
	int h = 0;

	if (m_curIndex >= 0 && m_curIndex < COUNT_BOX)
	{
		m_spr[m_curIndex]->setColor(Color3B::GRAY);
	}

	switch (m_step)
	{
	case 0:
		if (m_curIndex < COUNT_BOX)
		{
			m_spr[m_curIndex]->setColor(Color3B::WHITE);

			POINT vh = box_GetPidBit(m_curIndex);
			g_pIoCtrlLayer->openLock(vh.x, vh.y, 0.1f);
		}
		break;

	case 1:
		m_curIndex++;
		break;

	case 2:
		if (m_curIndex >= COUNT_BOX)
		{
			Director::getInstance()->popScene();
			return;
		}
		break;
	}

	++m_step %= 4;
}

