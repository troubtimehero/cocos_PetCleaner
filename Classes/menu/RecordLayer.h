#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "cocos/ui/CocosGUI.h"
#include "GameData.h"
#include "ioCtrl/KnobSmooth.h"
#include "ioCtrl/KnobLayerBase.h"

const bool cKnobItemCountRecord[15] = { true, true, true, true };
class RecordLayer : public KnobLayerBase
{
	RecordLayer() : KnobLayerBase(4, cKnobItemCountRecord){}
public:
	bool init();
	CREATE_FUNC(RecordLayer);
	void onEnter();

private:
	void updateLanguage();
	void updateRecord();
	bool m_bFirst;
	void updateRemainCoin();

private:
	Menu*	m_menu;
	Label*	m_title[3]; ////0：总收益、1：货品收益、2：剩余金额
	Layer*	m_lTotal;
	Layer*	m_lLast;
	Label*	m_remainCoin;

	ui::Button*		m_btnSelectTotal;
	ui::Button*		m_btnSelectLast;

	ui::Button*		m_btnBack;
	ui::Button*		m_btnClearCoin;
	ui::Button*		m_btnClearRecord;
	ui::Button*		m_btnVerify;

	void onBtnClearCoin();
	void onBtnClearRecord();
	void onBtnBack();
	void onBtnVerify();

#ifdef USE_KNOB
	virtual void update(float delta);
#endif // USE_KNOB

};

