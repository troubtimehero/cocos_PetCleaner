#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "tool/PayByQRCode.h"
#include "ui/CocosGUI.h"
#include "GlobalDefined.h"

// #define BLOW_CAN_COLD_WIND	//只出热风
class MainLayer : public Layer, public PayTarget
{
public:
	CREATE_FUNC(MainLayer);
	virtual bool init();
	virtual void update(float delta);

	virtual void callbackPayOK();

private:
	bool		m_bWash;
	bool		m_bDry;
	int			m_nWashTime;
	int			m_nDryTime;


#ifdef BLOW_CAN_COLD_WIND
	ui::CheckBox*	m_checkBoxHot;
#endif // BLOW_ONLY_HOT_WIND

	float		m_fLightTime;	//点击屏幕触发照明灯，没有其他操作情况下，定时关闭

#ifdef CAN_CHOOSE_WORK_TIME
	void SliderCallBack(Ref *pSender, ui::Slider::EventType type);
	int			m_addTimeWash;
	int			m_addTimeDry;
	ui::Slider*	m_slAddWash;
	ui::Slider*	m_slAddDry;
	Label*		m_lTimeWash;
	Label*		m_lTimeDry;
	Label*		m_lTimeTotal;
#else
	void onSelectTime(ui::RadioButton* radioButton, ui::RadioButton::EventType type);
	int			m_iHairIndex;
#endif // CAN_CHOOSE_WORK_TIME
	
	void calNeedPay();
	int			m_nNeedPayCent;
	Label*		m_lNeedCent;

	ui::Button*		m_btnStart;
};

