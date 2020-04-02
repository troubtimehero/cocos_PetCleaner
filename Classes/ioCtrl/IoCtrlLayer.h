#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "music/GameMusic.h"
#include "tool/LayerPassword.h"

#define NAME_GAME_WIN_COIN	"gameWinCoin"
class IoCtrlLayer : public Layer, public LayerNeedPassword
{
public:
	virtual void onPasswordOK(std::string password);

public:
	IoCtrlLayer();
	~IoCtrlLayer();
	bool init();

	void onEnter();
	void onExit();
	void update(float delta);

	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);

	void useCoinByGame(int index = -1);
	void timerBlinkInsertAndTick(float delta);

	void updateCoinShow();
	void myAddCent(int cent, bool bNet = false);

private:
	void myAddRemainCoin(int coin);//增加玩家币数，返还，不加记录
	void myClearCoin();
	void myCallMenu();

private:
	Sprite*		m_eggError;
	Label*		m_tickError;
	Label*		m_tickNum;
	Label*		m_errorMcu;
	Node*		m_boxOutting;

	int			m_nTick;


public:
	enum eSceneTag
	{
		e_scene_game,
		e_scene_other
	};


	//---------------- 旋钮 -------------------
public:
	enum eKeyTag
	{
		e_key_fire,
		e_key_left,
		e_key_right
	};
	bool isKeyDown(eKeyTag key, float fSpace = 0);
	bool isKeyKeep(eKeyTag key);
	//旋钮振动
	void knobShake(int pid, float time = 0.2f);
	//发射灯
	void lightFire(int pid, bool bOn);

private:
	bool	m_keyDown[4];
	bool	m_keyKeep[4];
	struct tagKnobShake
	{
		int pid;
		float time;
	};
	std::list<tagKnobShake>		m_lstKnobShakeRemain;


	//---------------- 旋钮 -------------------
public:
	void openLock(int v, int h, float openTime = 0);
	bool isLockOpening();
	bool isLockError();
private:
	float	c_iniLockOpeningTime;
	float	m_fLockOpeningTime;
	bool	m_bLockError;	//每帧清除
#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
	int		m_iMotorIndex;
#endif


	//全局音乐
	CGameMusic*		m_music;
	float			m_fMusicDelay;
};

extern IoCtrlLayer*	g_pIoCtrlLayer;


void Gbl_OutTicket(int pid, int nTick);
void Gbl_EggOut(int pid, int nEgg);