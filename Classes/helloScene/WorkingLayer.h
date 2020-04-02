#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "ui/CocosGUI.h"
#include "ioCtrl/ComThread.h"

//----------------------- 对应脚位 -----------------
const CComThread::tagPidBit	DEV_WORKING_LOTION(0, 0);	//洗洁精 1P-16
const CComThread::tagPidBit	DEV_WORKING_WASH(0, 1);	//冲洗 1P-11
const CComThread::tagPidBit	DEV_WORKING_BLOW(0, 2);	//吹风 1P-14
const CComThread::tagPidBit	DEV_WORKING_HOT(1, 3);	//加热 2P-13

const CComThread::tagPidBit	DEV_LIGHT_NORMAL(1, 0);	//照明灯 2P-16
const CComThread::tagPidBit	DEV_LIGHT_PURPLE(1, 1);	//紫外灯 2P-11

class WorkingLayer : public LayerColor
{
public:
	CREATE_FUNC(WorkingLayer);
	bool init();
	void Start(int nWashTime, int nBlowTime, bool bHot);
	virtual void update(float delta);

private:
	void timerRemain(float delta);

	enum eWork
	{
		e_workWash,
		e_workLotion,
		e_workBlow,
		e_workHot
	};
	void enableWork(eWork w, bool bEnable);

	Label*	m_lWashing;
	Label*	m_lBlowing;


	enum eStep
	{
		e_stepReady,
		e_stepWet,
		e_stepLotion,
		e_stepClean,
		e_stepWait,
		e_stepWind,
		e_stepStop,
		e_stepFinTips,
		e_stepNull
	};
	eStep	m_step;	//0准备，1洗，2吹，3结束
	float	m_stepTime;

	int		m_lotionTime;
	int		m_washTime;
	int		m_blowTime;
	bool	m_bHotWind;

	float	m_fFinTipsTime;

private:
	ui::Button*		m_btnStop;
	void wantStop();
	void Stop(bool bOpenDoor);
};


void lightNormalOn(bool bOn);
