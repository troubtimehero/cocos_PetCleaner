#pragma once
#include "cocos2d.h"
USING_NS_CC;
#include "ui/CocosGUI.h"

#include "D:\Pro_share\libs\io_LYY_base.h"
#pragma comment(lib, "D:/Pro_share/libs/io_LYY_base.lib")


/************************************************************************/
/*                                                                      */
/************************************************************************/

class io_lyy_Pet : public io_LYY_Good
{
public:
	io_lyy_Pet(int port);

public:
	virtual int OnMsgUpSetting();
	virtual void OnMsgDownSetting();

public:
	//【上报】本地有更新就上报，等查询时再发送
	virtual void	ReportSetting();
	virtual bool	IsNewSetting();	//
	virtual void	DownSetting();

	bool willEnterSetting(){ return m_enterSetting > 0; }
	void clearEnterSettingSignal(){ m_enterSetting = 0; }

protected:
	BYTE m_washBaseTime;
	BYTE m_washIncTime;
	USHORT m_washBasePrice_10;
	BYTE m_washIncPrice_10;

	BYTE m_dryBaseTime;
	BYTE m_dryIncTime;
	USHORT m_dryBasePrice_10;
	BYTE m_dryIncPrice_10;

	BYTE m_saleMode;
	BYTE m_gamePrice_10;

	BYTE m_enterSetting;

	bool	m_bNewSetting;

};

/************************************************************************/
/* 说明：
*/
/************************************************************************/

class LeYaoYao : public Node
{
public:
	CREATE_FUNC(LeYaoYao);
	virtual bool init();
	virtual void onEnter();
	virtual void update(float delta);

public:
	void timerReport(float delta);
private:
	int		m_iTimerStep;
	float	m_fNoTouchTime;

	void ReportActiveStatus();
	void ReportSetting();
	void ReportAll();

private:
	ui::Slider*			m_signal;

public:
	void SetTradeTemp(int iBoxOutIndex, bool bGame);
	void UpTrade(bool bSucceed);
	void UpTradeGameFailed(int cent, int iBoxOutIndex, int nBoxOut, int nBoxOutTotall, int buyPrice, int gamePrice);


	/** Creates a DrawNode with row, col, width, height and lineSize.
	* @param iWork 工作模式——0:洗烘	1:净洗		2:净烘
	* @param iPrice 价格段——0:<1		1:1-10		2:10-20		3:20-30		4:>30
	*/
	void UpPetTrade(int iWork, int nPay);
private:
	int		tmp_game;
	int		tmp_index;
};

extern	io_lyy_Pet*		g_pBox;
extern	LeYaoYao*			g_pLeyaoyao;
