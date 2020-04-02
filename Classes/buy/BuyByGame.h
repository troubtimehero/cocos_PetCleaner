#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "ui/CocosGUI.h"
#include "tool/PayByQRCode.h"
#include "MachineDefined.h"
#include "tool/MyBtnTimeoutBack.h"

class BuyByGame : public TimeoutBackTarget, public PayTarget
{
public:
	CREATE_FUNC(BuyByGame);
	virtual bool init();
	virtual void update(float delta);

	virtual void callbackPayOK();
	virtual void callbackCancel();

	void callWin();

private:
	void updateBoxContain(int index);
	void callbackClickBtn(int index);

	ui::Button*	m_btns[COUNT_BOX];
	int		m_wantIndex;
};

