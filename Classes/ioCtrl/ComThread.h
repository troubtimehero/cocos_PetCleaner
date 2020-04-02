#pragma once
#include "cocos2d.h"
USING_NS_CC;

#include "D:\Pro_share\io_4p_fast\io_4p\io_4p.h"
#pragma comment(lib, "D:/Pro_share/io_4p_fast/Release/io_4p_fast.lib")
#define io_def	io_4p_fast

/************************************************************************/
/* ˵����
��ť�źŵ�������
�����޶�����ť�����ڹ���Ա��λ����ֻʹ��������λ��ÿ֡ͳ�Ʊ������������
�������а����仯����ŵ�list�У�ÿֻ֡ʹ����ǰһ������ֹ�źŶ�ʧ��
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
	io_def*		m_io[4];	//���֧��4��壿
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
	static const int sc_nBtnBits = 8;//ÿPλ
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
		e_bit_UP,		//0 �� 0x01
		e_bit_DOWN,		//1 ��
		e_bit_LEFT,		//2 �� 0x04
		e_bit_RIGHT,	//3	�� 0x08
		e_bit_FIRE,		//4	���� 0x10
		e_bit_POWER,	//5 ����
		e_bit_ADD,		//6 ��Ʊ 0x40
		e_bit_SCORE		//7 �÷� 0x80
#else
		e_bit_UP,	//0 �˵� 0x01
		e_bit_DOWN,	//1 �ϻ���
		e_bit_LEFT,	//2 �� 0x04
		e_bit_RIGHT,	//3	�� 0x08
		e_bit_FIRE,	//4	���� 0x10
		e_bit_POWER,	//5 �ϻ���
		e_bit_ADD,		//6 ��Ʊ 0x40
		e_bit_SCORE	//7 ��Ʊ 0x80
#endif
	};
	struct tagPidBit
	{
		tagPidBit(int p, int b) :pid(p), bit(b){}
		int pid;
		int bit;
	};
	//����Ա:
	//���ڷ���	  00~000000000000000000000000000~00
	//��Ӧ����		�� 0 1 2 3 4 5 6 7 0 x x x x
	enum eKnobDirc
	{
		e_knob_l,
		e_knob_r
	};
	static const unsigned char c_bitKnob = 0x04 + 0x08;
	static const unsigned char c_bitKnobL = 0x04;
	static const unsigned char c_bitKnobR = 0x08;

	int	GetKnob(eKnobDirc dirc);	//ֻ��1P
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
unsigned char ioErrorStatus();//��λ��Ӧ ����1:0x01������2:0x02������3:0x04������4:0x08
extern CComThread*		g_pCom;

#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
void ioOpenMotor(int index);
#else
void ioOpenLock(int v, int h);
#endif
