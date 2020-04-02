#pragma once
#include <cocos2d.h>
USING_NS_CC;
#include "tool/RandomController.h"
#include "ui/CocosGUI.h"


//////////////////////////////////////////////////////////////////////////
//����ʼ���������ٶ��������ӣ���һ���ٶȲſ��ԡ�ֹͣ��
class RollGame : public Layer
{
public:
	static RollGame* MyCreate(Node** pElements, int nElement, float fDistY, float speedSpeed);	//�˴����� addChild(pElement[])

	bool init(Node** pElements, int nElement, float distY, float speedSpeed);
	virtual void update(float delta);

	void Start(int winIndex);
	bool Stop();	//���� true �Ѿ�ֹͣ�� ���� false ������ֹͣ


private:
	~RollGame();
	void nextIndex();
	void render();

	Node**	pElementArr;
	int		nElementCount;
	float	fDistY;
	float	fSpeedOfSpeed;

	int		iForceIndex;

	bool	bRun;
	int		iCurIndex;
	int		iLastIndex;
	int		iNextIndex;
	bool	bExcept;

	float	fCurY;
	float	fSpeedY;

	RandomController<int, 1>*	pRc = nullptr;
};


//////////////////////////////////////////////////////////////////////////

class RollGameMgr : public Layer
{
public:
	static RollGameMgr* MyCreate(RollGame** pRoll, int nCount);	//����ֻ������û�� addChild()
	bool init(RollGame** pRoll, int nCount);

	void Start(int iWinIndex = -1);
	bool Stop();

private:
	~RollGameMgr();
	RollGame**	m_pRolls;
	int			m_nCount;
	int			m_nStoped;
};


//////////////////////////////////////////////////////////////////////////
// �㷨
class AlgWinType
{
public:
	AlgWinType(){};
	~AlgWinType(){};

	enum eType
	{
		e_none,
		e_box,
		e_coin_1,
		e_coin_2,
		e_coin_5,
		e_coin_10,
		e_coin_20
	};

	static eType algType(int wantIndex);
};

//////////////////////////////////////////////////////////////////////////

class GameLayer : public LayerColor
{
	enum eStatus
	{
		e_ready,
		e_playing,
		e_stop,
		e_win,
		e_loss,
		e_end
	};
	eStatus		m_eStatus;
	float		m_fDelay;
public:
	static GameLayer* MyCreate(int wantIndex);	//��Ҫ����Ʒ���

	virtual bool init(int wantIndex);

	virtual void update(float delta);

private:
	AlgWinType::eType		m_winType;

	void callbackClick();
	RollGameMgr*	m_rgm;
	ui::Button*		m_btn;
	bool			m_bRun;
};
