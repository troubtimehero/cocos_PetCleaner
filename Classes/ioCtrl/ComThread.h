#pragma once
#include "cocos2d.h"
USING_NS_CC;

#include "D:\Pro_share\io_4p_fast\io_4p\io_4p.h"
#pragma comment(lib, "D:/Pro_share/io_4p_fast/Release/io_4p_fast.lib")
#define io_def	io_4p_fast

/************************************************************************/
/* 说明：
旋钮信号单独处理。
本类限定用旋钮输入在管理员脚位，且只使用其中两位。每帧统计本轮脉冲次数。
其他所有按键变化都存放到list中，每帧只使用最前一个，防止信号丢失。
*/
/************************************************************************/
#define PLAYER_NUM	1
class CComThread
{
	static const int c_nMaxIO = 4;
public:
	CComThread(int cPcbCount = 1);
	~CComThread();

	bool ioIsError(int io);

// 	void SetKeyLogic(EventKeyboard::KeyCode keyCode);
	void Update();

	void SetEnable(bool bTick, bool bCoin);

	bool IsTickOutError(int pid);
	bool IsEggOutError(int pid);
	int GetCoinIn(){ return m_nNewCoinIn; }
	int GetTicketOut(int pid);
	
	void SetTickOut(int pid, int value);
	void SetEggOut(int pid, int value);
	void RetryOuting();
	void ClearOuting();

	//	score = 0, ticket = 1, power = 2, fire = 3
	void SetDevOut(int pid, int dev, bool bEnable);
	void CloseAllDev();
private:
	int			m_nIoCount;
	io_def*		m_io[4];	//最多支持4块板？
	bool		m_bEnableTicket;
	bool		m_bEnableCoin;

	void UpdateTicketOut();
	void UpdateCoinOut();
	void ErrorClean();
private:

	void ClearFrameData();
	int		m_nNewCoinIn;
	int		m_nRemainCoinOut;
	int		m_nRemainTickOut;

public:
	// button
	static const int sc_nBtnBits = 8;//每P位
	enum eBtnAddr
	{
		e_addr_p1,
		e_addr_p2,
		e_addr_p3,
		e_addr_p4,
		e_addr_mgr,
		e_addr_mgr2,
		emax_addr
	};
	enum eBtnBit
	{
#if 1
		e_bit_UP,		//0 上 0x01
		e_bit_DOWN,		//1 下
		e_bit_LEFT,		//2 左 0x04
		e_bit_RIGHT,	//3	右 0x08
		e_bit_FIRE,		//4	发射 0x10
		e_bit_POWER,	//5 加力
		e_bit_ADD,		//6 补票 0x40
		e_bit_SCORE		//7 得分 0x80
#else
		e_bit_UP,	//0 菜单 0x01
		e_bit_DOWN,	//1 上或下
		e_bit_LEFT,	//2 左 0x04
		e_bit_RIGHT,	//3	右 0x08
		e_bit_FIRE,	//4	发射 0x10
		e_bit_POWER,	//5 上或下
		e_bit_ADD,		//6 补票 0x40
		e_bit_SCORE	//7 清票 0x80
#endif
	};
	struct tagPidBit
	{
		tagPidBit(int p, int b) :pid(p), bit(b){}
		int pid;
		int bit;
	};
	//管理员:
	//开口方向	  00~000000000000000000000000000~00
	//对应数据		地 0 1 2 3 4 5 6 7 0 x x x x
	enum eKnobDirc
	{
		e_knob_l,
		e_knob_r
	};
	static const unsigned char c_bitKnob = 0x04 + 0x08;
	static const unsigned char c_bitKnobL = 0x04;
	static const unsigned char c_bitKnobR = 0x08;

	int	GetKnob(eKnobDirc dirc);	//只用1P
	bool Check(eBtnAddr addr, eBtnBit bit, bool bMgr = false);
	bool CheckOnce(eBtnAddr addr, eBtnBit bit, bool bMgr = false);
	bool CheckSpace(eBtnAddr addr, eBtnBit bit, float time, bool bMgr = false);
	bool CheckUp(eBtnAddr addr, eBtnBit bit, bool bMgr = false);
	bool CheckUpDelay(eBtnAddr addr, eBtnBit bit, float time, bool bMgr = false);
	const bool* GetButtonArray(int io) const;
	string GetStringOfButtonStage(int io);
private:
	bool isAddrBitWrong(int addr, int bit, bool bMgr);

	list<tagComButton>	m_lstPoll[c_nMaxIO];
	int			m_nKnobL[c_nMaxIO];
	int			m_nKnobR[c_nMaxIO];

public:
private:

	bool		m_btn[c_nMaxIO][emax_addr * sc_nBtnBits];
	bool		m_btnLast[c_nMaxIO][emax_addr * sc_nBtnBits];
	float		m_btnKeepTime[c_nMaxIO][emax_addr * sc_nBtnBits];
	float		m_btnTimeSpace[c_nMaxIO][emax_addr * sc_nBtnBits];
	float		m_btnUpTime[c_nMaxIO][emax_addr * sc_nBtnBits];
	float		m_btnUpTimeLast[c_nMaxIO][emax_addr * sc_nBtnBits];
	void McuAnalysis();
};

bool equalComPolling(tagComButton* a, tagComButton* b);
unsigned char ioErrorStatus();//按位对应 串口1:0x01，串口2:0x02，串口3:0x04，串口4:0x08
extern CComThread*		g_pCom;

#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
void ioOpenMotor(int index);
#else
void ioOpenLock(int v, int h);
#endif
