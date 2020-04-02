#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "cocos/ui/CocosGUI.h"
#include "GameData.h"
#include "tool/InputNumber.h"

#if ( VERSION_BUY_TYPE == BUY_NONE )
const bool cKnobItemCountSetting_Pet[15] = { true, true, true, true, false, false, false, false, false, false, false, false, true, true, true };
#else
const bool cKnobItemCountSetting_Pet[15] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
#endif
class SettingLayer_Pet : public LayerColor, public InputNumberOnMessage
{
private:
	LayerMultiplex*		m_lm;
// 	int					m_iCurLm;	//外部需要

public:
	CREATE_FUNC(SettingLayer_Pet);
	virtual bool init();
	virtual void onEnter();

	virtual void OnMessage(InputNumberGetting* caller, int number, int exponent);

	void onSelectRadioButton(ui::RadioButton* radioButton, ui::RadioButton::EventType type);

private:
#ifdef CAN_CHOOSE_WORK_TIME
	static const int	c_layerMultiCount = 2;
	void initFreeChoose();
#else
	static const int	c_layerMultiCount = 3;
	void initLimitChoose();
#endif // CAN_CHOOSE_WORK_TIME
	MenuItemFont* makeMenuItemWithData(int dataIndex, int iMin, int iMax, int exponent);//bCent：小数


	void menuInputCallback(Ref* sender, int iMin, int iMax, int exponent);
	void languageCallback(Ref* sender);

	void onBtnRecord();
	void onBtnBack();
	void onBtnFillWater();


	void updateLanguage();
};

