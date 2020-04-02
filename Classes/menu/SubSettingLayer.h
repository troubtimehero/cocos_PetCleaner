#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "GameData.h"
#include "tool/InputNumber.h"
#include "ui/CocosGUI.h"

//////////////////////////////////////////////////////////////////////////
class SubSettingBase : public LayerColor
{
public:
	virtual bool init();
protected:
	Label*		m_title;
};

//////////////////////////////////////////////////////////////////////////

class SettingBuyPrice : public SubSettingBase, public InputNumberOnMessage
{
public:
	CREATE_FUNC(SettingBuyPrice);

	virtual bool init();

	virtual void OnMessage(InputNumberGetting* caller, int number, int exponent);

private:
	void menuCallback(Ref* sender);

	bool	m_bTotal;
	int		m_nTotalPrice;
};


//////////////////////////////////////////////////////////////////////////

class SettingSaleOnOff : public SubSettingBase
{
public:
	CREATE_FUNC(SettingSaleOnOff);

	virtual bool init();

private:
	void callbackSingle(Node* caller);
	void callbackTotal(Node* caller);

	Node*	m_yes;
	Node*	m_no;
};


//////////////////////////////////////////////////////////////////////////

class SettingOpenTest : public SubSettingBase
{
public:
	CREATE_FUNC(SettingOpenTest);
	virtual bool init();

	void update(float delta);

private:
	void callbackClickBtn(int v, int h);
	void timerBtnStatus(float delta);
	// 	void updateComPressBtn();

private:
	ui::Button*	m_btns[COUNT_BOX];
};


//////////////////////////////////////////////////////////////////////////

class SettingOpenRecord : public SubSettingBase
{
public:
	CREATE_FUNC(SettingOpenRecord);
	virtual bool init();

private:

};


//////////////////////////////////////////////////////////////////////////

class SettingCleanRecord : public SubSettingBase
{
public:
	CREATE_FUNC(SettingCleanRecord);
	virtual bool init();
	virtual void onEnter();

private:
	Layer*	m_lTotal;
	Layer*	m_lLast;
	ui::Button*		m_btnSelectTotal;
	ui::Button*		m_btnSelectLast;
	ui::Button*		m_btnClearRecord;

	void updateRecord();
	void onBtnClearRecord();
};
