#include "ADScene.h"
#include "DataMgr/DataMgr.h"
#include "tool/MyTools.h"

Scene* ADScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = ADScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool ADScene::init()
{
	if (!Layer::init())	// cyhol 透明层 [9/20/2017 pc-ah]
		return false;

	auto bg = Sprite::create("ad/background.jpg");
	bg->setPosition(VisibleRect::center());
	this->addChild(bg);
	bg->setContentSize(VisibleRect::size());

#if 0	//修改引擎不可
	BinkVedio* vedio = BinkVedio::createVedio("ad/ad_001.bik", 0.2f);
	this->addChild(vedio);
	vedio->setPosition(VisibleRect::top() + Vec2(0, -vedio->getAni()->getContentSize().height / 2));
#endif

	m_pSpr = nullptr;
	m_iCurMap = 0;

	ScanFile(".\\ad\\contain", m_lstFile);

	int nMap = m_lstFile.size();
	if (nMap > 0)
	{
		m_pSpr = new Sprite*[nMap];

		for (int i = 0; i < nMap; ++i)
		{
			m_pSpr[i] = Sprite::create(m_lstFile.at(i));
			this->addChild(m_pSpr[i]);
			m_pSpr[i]->setPosition(VisibleRect::top() + Vec2(0, -264));
			m_pSpr[i]->setContentSize(Size(747, 416));
			m_pSpr[i]->setOpacity(0);
		}

		m_iCurMap = nMap - 1;
		schedule(schedule_selector(ADScene::timerSwitch), 10, CC_REPEAT_FOREVER, 0);	 //一定时间后再生成道具
	}


	//触摸监测
	auto listenerTouch = EventListenerTouchOneByOne::create();
	listenerTouch->onTouchBegan = [this](Touch *, Event *){
		Director::getInstance()->popScene();
		return false;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, this);

	return true;
}

void ADScene::release()
{
	if (m_pSpr)
	{
		delete[] m_pSpr;
		m_pSpr = nullptr;
	}
	SceneMgr::release();
}

void ADScene::timerSwitch( float delta )
{
	m_pSpr[m_iCurMap]->runAction(FadeOut::create(1));
	++m_iCurMap %= m_lstFile.size();
	m_pSpr[m_iCurMap]->runAction(FadeIn::create(1));
}

#if 0
void ADScene::onEnter()
{
	SceneMgr::onEnter();

}

void ADScene::onExit()
{
#ifndef SCAN_MUSIC
	MySound::stopMusic();
#endif
	SceneMgr::onExit();
}
#endif