#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
#include "MachineDefined.h"
#include "tool/SceneMgr.h"
#include "tool/PayByQRCode.h"

class ADScene : public SceneMgr
{
public:
	static cocos2d::Scene* createScene();

	CREATE_FUNC(ADScene);

	bool init();

#if 0
	virtual void onEnter();
	virtual void onExit();
#endif
	virtual void release();

private:
	void timerSwitch(float delta);

	std::vector<std::string>	m_lstFile;
	Sprite**		m_pSpr;
	int				m_iCurMap;
};
