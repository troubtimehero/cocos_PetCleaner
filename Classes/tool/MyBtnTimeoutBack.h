#pragma once
#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
USING_NS_CC;

#define BUTTON_NAME_BACK_TIMEOUT	"BUTTON_BACK_TIMEOUT"
#define LAYER_NAME_BUY				"LAYER_NAME_BUY"
#define LAYER_NAME_WANT_GAME		"LAYER_NAME_GAME"
#define LAYER_NAME_PAY				"LAYER_NAME_PAY"
#define LAYER_NAME_GAME_ING			"LAYER_NAME_GAME_ING"

//操作超时，直接执行回调函数
class TimeoutBackTarget : public Layer
{
public:
	TimeoutBackTarget(){}
	~TimeoutBackTarget(){}

	virtual void callbackTimeoutBack();
};


//////////////////////////////////////////////////////////////////////////
class MyBtnTimeoutBack : public ui::Button
{
public:
	static MyBtnTimeoutBack* MyCreate(const std::string& normalImage, int timeout, TimeoutBackTarget* target);

	bool init(const std::string& normalImage, int timeout, TimeoutBackTarget* target);
	void enableTimeout(bool b);
	void resetTimeout(int t);

private:
	void timerCheckTimeout(float delta);

	int		m_timeout;
	bool	m_bEnableTimeout;
	Label*	m_lNum;
	TimeoutBackTarget*	m_target;
};
