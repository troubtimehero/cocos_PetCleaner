#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "cocos/ui/CocosGUI.h"
#include "GameData.h"
#include "ioCtrl/KnobSmooth.h"
#include "ioCtrl/KnobLayerBase.h"

#if ( VERSION_BUY_TYPE == BUY_NONE )
const bool cKnobItemCountSetting[15] = { true, true, true, true, false, false, false, false, false, false, false, false, true, true, true };
#else
const bool cKnobItemCountSetting[15] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
#endif
class SettingLayer : public KnobLayerBase
{
	SettingLayer() : KnobLayerBase(eDataKey::set_Volume + 1 + 2, cKnobItemCountSetting){}
private:
	enum eTag
	{
		e_tag_saleMode = 1,
		e_tag_gameCoin = 2,
		e_tag_cleanRecord = 3,
		e_tag_buyCoin = 4,
		e_tag_addGift = 5,
		e_tag_openTest = 6,
		e_tag_openRecord = 7,
		e_tag_volume = 21,
		e_tag_record = 22,
		e_tag_back = 23
	};
	Menu*	m_menuStatic;
	Menu*	m_menu;

public:
	bool init();
	CREATE_FUNC(SettingLayer);
	void onEnter();

private:
	void SliderCallBack(Ref *pSender, ui::Slider::EventType type);

	void menuCallback(Ref* sender);
	void setMenuData(MenuItemFont* item, bool bIncrease);
	void updateMenuEnable();

	void onBtnRecord();
	void onBtnBack();


	void updateLanguage();

#ifdef USE_KNOB
	virtual void update(float delta);
	void updateItemData(bool bIncrease);
private:
	bool		m_bLockItem;
#endif // USE_KNOB
};

