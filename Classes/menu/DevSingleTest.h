#pragma once
#include "cocos2d.h"
USING_NS_CC;

//////////////////////////////////////////////////////////////////////////
class DevSingleTest : public LayerColor
{
public:
	CREATE_FUNC(DevSingleTest);
	virtual bool init();

private:
	bool	m_bOn[6];

	Layer*	m_lBtns;
	Label*	m_password;

	void initPassword();
	void onBtnNumber(int num);
	void onBtnDel();
};

