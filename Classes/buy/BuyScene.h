#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
#include "MachineDefined.h"
#include "tool/PayByQRCode.h"
#include "tool/MyBtnTimeoutBack.h"

class BuyLayer : public TimeoutBackTarget, public PayTarget
{
public:
	static cocos2d::Scene* createScene();

	CREATE_FUNC(BuyLayer);

	bool init();

	virtual void callbackPayOK();

	void update(float delta);

	void updateBoxContain(int index);

	virtual void callbackTimeoutBack();

private:
	void callbackClickBtn(int index);
// 	void updateComPressBtn();

private:
	ui::Button*	m_btns[COUNT_BOX];

	int			m_iWwant;
};
