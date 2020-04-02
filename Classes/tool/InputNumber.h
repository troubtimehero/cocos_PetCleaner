#pragma once

#include "cocos2d.h"
USING_NS_CC;
#include "GameData.h"

/*
InputNumber 是调用出来的软键盘图层，
操作完后结果传给InputNumberGetting，
SetNumber()中可以调用最外的 InputNumberOnMessage
*/
class InputNumberGetting;

class InputNumberOnMessage
{
public:
	InputNumberOnMessage(){}
	~InputNumberOnMessage(){}

	virtual void OnMessage(InputNumberGetting* caller, int number, int exponent) = 0;
};

//////////////////////////////////////////////////////////////////////////


class InputNumberGetting
{
public:
	InputNumberGetting(int iMin, int iMax, InputNumberOnMessage* target) : m_target(target), m_inputMin(iMin), m_inputMax(iMax) {}
	~InputNumberGetting(){}

	virtual void SetNumber(int number, int exponent);

	int GetInputNumberMin(){ return m_inputMin; }
	int GetInputNumberMax(){ return m_inputMax; }
protected:
	InputNumberOnMessage*	m_target;
	int		m_inputMin;
	int		m_inputMax;
};

//////////////////////////////////////////////////////////////////////////

class InputNumber : public LayerColor
{
public:
	static InputNumber* MyCreate(InputNumberGetting* pCaller, int inputMin, int inputMax, int exponent = 0);//exponent 为小数位：0 - 3
	virtual bool init(InputNumberGetting* pCaller, int inputMin, int inputMax, int exponent);

private:
	void onBtnNumber(int num);
	void onBtnDel();
	void onBtnOK();
	void onBtnBack();
	void updateNumber();

	unsigned int	m_input;
	InputNumberGetting*			m_caller;
	int		m_min;
	int		m_max;
	bool	m_bCent;
	int		m_exponent;
};

