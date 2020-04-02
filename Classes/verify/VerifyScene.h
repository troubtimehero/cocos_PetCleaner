#pragma once

#include "cocos2d.h"
USING_NS_CC;
#include "tool/SceneMgr.h"
#include "ioCtrl/KnobLayerBase.h"
/************************************************************************/
/*                                                                      */
/************************************************************************/

#ifdef USE_VERIFY
class Scene_Verify : public SceneMgr
{
public:
	static cocos2d::Scene* createScene();

	// implement the "static create()" method manually
	CREATE_FUNC(Scene_Verify);

	virtual bool init();
	virtual void onEnter();
// 	virtual void onExit();

public:
	bool IsCanPlay();
private:
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
const bool cKnobItemCountVerifyDisplay[2] = { true, true };
class VerifyDisplayLayer : public KnobLayerBase
{
	VerifyDisplayLayer() : KnobLayerBase(2, cKnobItemCountVerifyDisplay){}
public:
	CREATE_FUNC(VerifyDisplayLayer);

	virtual bool init();
	virtual void update(float delta);

	virtual void updateItemIndex(int iDirection);

private:
	void checkBtnEnable();
	void setActionMove(bool bIn);

	void onBtnBack();
	void onBtnSetting();
};

//////////////////////////////////////////////////////////////////////////

const bool cKnobItemCountVerifySetting[15] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
class VerifySettingLayer : public KnobLayerBase
{
	VerifySettingLayer() : KnobLayerBase(2 + 10 + 3, cKnobItemCountVerifySetting){}
public:
	CREATE_FUNC(VerifySettingLayer);

	virtual bool init();
	virtual void update(float delta);

	virtual void updateItemIndex(int iDirection);

private:
	unsigned int	m_input[2];
	int		m_curInput;

	void onBtnInput(int index);
	void onBtnNumber(int num);
	void onBtnDel();
	void onBtnOK();
	void onBtnBack();

	void updateNumber();
	void timerEnableClick(float delta);
	void setActionMove(bool bIn);
};

#else

class Scene_Verify : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	// implement the "static create()" method manually
	CREATE_FUNC(Scene_Verify);

	virtual bool init(){ return true; }

	bool IsCanPlay(){ return true; }
};


#endif // USE_VERIFY
