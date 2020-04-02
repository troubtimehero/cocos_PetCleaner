#include "ComThread.h"
#include "debug/DebugInfoLayer.h"
#include "GameData.h"
#include "tool/MyTools.h"

#ifdef USE_COM
static const bool sc_bDoNotUseCom = false;
#define FUNC_HEAD_COM
#define FUNC_HEAD_COM_X(x)
#else
static const bool sc_bDoNotUseCom = true;
#define FUNC_HEAD_COM		if (sc_bDoNotUseCom){return;}
#define FUNC_HEAD_COM_X(x)		if (sc_bDoNotUseCom){return x;}
#endif // USE_COM


//投币、退票、退币，只使用一个控制板，只使用1P位
static const int ctIo = 0;
static const int ctPid = 0;


CComThread::CComThread(int cIoCount) : m_nNewCoinIn(0), m_bEnableTicket(false), m_bEnableCoin(false), m_nIoCount(cIoCount)
{
	ZeroMemory(m_btn, sizeof(m_btn));
	ZeroMemory(m_btnLast, sizeof(m_btnLast));
	ZeroMemory(m_btnKeepTime, sizeof(m_btnKeepTime));
	ZeroMemory(m_btnTimeSpace, sizeof(m_btnTimeSpace));
	ZeroMemory(m_btnUpTime, sizeof(m_btnUpTime));
	ZeroMemory(m_btnUpTimeLast, sizeof(m_btnUpTimeLast));

	FUNC_HEAD_COM;

	for (int i = 0; i < m_nIoCount; ++i)
	{
		m_io[i] = io_def::CreateInstance(i+1, 60);	//从串口1开始顺延
		m_io[i]->setDebugLogFunc(debugOut);
		m_io[i]->Start();
		m_nKnobL[i] = 0;
		m_nKnobR[i] = 0;
		m_lstPoll[i].clear();
	}


}


CComThread::~CComThread()
{
	FUNC_HEAD_COM;
	for (int i = 0; i < m_nIoCount; ++i)
	{
		io_def::Release(m_io[i]);
	}
}

void CComThread::SetEnable(bool bTick, bool bCoin)
{
	m_bEnableTicket = bTick;
	m_bEnableCoin = bCoin;
}

void CComThread::Update()
{
	FUNC_HEAD_COM;
	ClearFrameData();

	float fps = Director::getInstance()->getAnimationInterval();
	for (int i = 0; i < m_nIoCount; ++i)
	{
		m_io[i]->ClearFrameInfo();
		m_io[i]->RunAyn(fps);
	}


	// coin in， 只使用一个投币口
	if (m_io[ctIo]->IsCoinIn(ctPid))
	{
		m_nNewCoinIn = m_io[ctIo]->GetCoinAdd(ctPid);
	}

	// button
	McuAnalysis();

	UpdateCoinOut();
	UpdateTicketOut();
}

void CComThread::ClearFrameData()
{
	m_nNewCoinIn = 0;
}


//---------------------------------- 只使用一个退票机 ------------------------------
void CComThread::UpdateTicketOut()
{
	FUNC_HEAD_COM;
	if (!m_bEnableTicket)	return;

	static bool s_bTicketOutStartUp[PLAYER_NUM] = { 0 };
// 	if (!Opt()->IsForFree())
	{
		// 开机时，如果有剩余彩票，则立即退票
		if (!s_bTicketOutStartUp[ctPid] && !m_io[ctIo]->NotReady())
		{
			if (m_io[ctIo]->GetTicketOut(ctPid) > 0)
				m_io[ctIo]->SetTicketOutValue(ctPid, m_io[ctIo]->GetTicketOut(ctPid));
			s_bTicketOutStartUp[ctPid] = true;
		}

		// 彩票数更新
		if (m_io[ctIo]->IsTicketOut(ctPid))
		{
			m_nRemainTickOut = m_io[ctIo]->GetTicketOut(ctPid);
		}
	}
}

//---------------------------------- 只使用一个退币机 ------------------------------
void CComThread::UpdateCoinOut()
{
	FUNC_HEAD_COM;
	if (!m_bEnableCoin)	return;

	static bool s_bCoinOutStartUp[PLAYER_NUM] = { 0 };
// 	if (!Opt()->IsForFree())
	{
		// 开机时，如果有剩余彩票，则立即退票
		if (!s_bCoinOutStartUp[ctPid] && !m_io[ctIo]->NotReady())
		{
			if (m_io[ctIo]->GetCoinOut(ctPid) > 0)
				m_io[ctIo]->SetCoinOutValue(ctPid, m_io[ctIo]->GetCoinOut(ctPid));
			s_bCoinOutStartUp[ctPid] = true;
		}

		// 彩票数更新
		if (m_io[ctIo]->IsCoinOut(ctPid))
		{
			m_nRemainCoinOut = m_io[ctIo]->GetCoinOut(ctPid);
		}
	}
}

void CComThread::RetryOuting()
{
	FUNC_HEAD_COM;
	// 	ErrorClean(pid);
	if (m_bEnableTicket && m_io[ctIo]->IsTicketError(ctPid))
		m_io[ctIo]->RetryTicketOut(ctPid);
	if (m_bEnableCoin && m_io[ctIo]->IsCoinError(ctPid))
		m_io[ctIo]->RetryCoinOut(ctPid);
}

void CComThread::ClearOuting()
{
	FUNC_HEAD_COM;
	if (m_io[ctIo]->GetTicketOut(ctPid) > 0)
		m_io[ctIo]->ZeroTicketOut(ctPid);
	if (m_io[ctIo]->GetCoinOut(ctPid) > 0)
		m_io[ctIo]->ZeroCoinOut(ctPid);
	m_io[ctIo]->SetMark(ctPid, 0);
	ErrorClean();
}

void CComThread::SetDevOut(int pid, int dev, bool bEnable)
{
	FUNC_HEAD_COM;
	if (pid >= 0 && pid < 4 * m_nIoCount && dev >= 0 && dev < 4)
	{
		m_io[pid/4]->EnableDevOut(pid%4, io_def::DevOutInfo(dev), bEnable);
	}
}

void CComThread::CloseAllDev()
{
	FUNC_HEAD_COM;
	for (int i = 0; i < m_nIoCount; ++i)
		m_io[i]->EnableDevOut(0);
}

void CComThread::ErrorClean()
{
	FUNC_HEAD_COM;
	if (m_bEnableCoin) m_io[ctIo]->CoinErrorClean(ctPid);
	if (m_bEnableTicket) m_io[ctIo]->TickErrorClean(ctPid);
}

void CComThread::McuAnalysis()
{
	FUNC_HEAD_COM;
	memcpy_s(m_btnLast, sizeof(m_btnLast), m_btn, sizeof(m_btn));

	const tagComButton* pEvent = nullptr;
	float dt = Director::getInstance()->getAnimationInterval();


	for (int iIo = 0; iIo < m_nIoCount; ++iIo)
	{
		//旋钮事件每次事先清空
		m_nKnobL[iIo] = 0;
		m_nKnobR[iIo] = 0;

		while (pEvent = m_io[iIo]->GetButtonState()) //防止数据丢失，应全部读取，并记录在list中
		{
			tagComButton btns = *pEvent;

			//先筛选掉旋钮信号
			unsigned char tmp = 0;
			unsigned char knob = 0;
			if (USE_KNOB_ADDR >= e_addr_mgr)
			{
				unsigned char uc = (btns.mgr >> (sc_nBtnBits*(USE_KNOB_ADDR - e_addr_mgr)));
				tmp = uc & (~c_bitKnob);
				knob = uc & c_bitKnob;
				btns.mgr = tmp;
			}
			else
			{
				tmp = btns.player[USE_KNOB_ADDR] & (~c_bitKnob);
				knob = btns.player[USE_KNOB_ADDR] & c_bitKnob;
				btns.player[USE_KNOB_ADDR] = tmp;
			}

			do {
				if (m_lstPoll[iIo].size() > 0 && equalComPolling(&m_lstPoll[iIo].back(), &btns))
					break;
				m_lstPoll[iIo].push_back(btns);
			} while (false);

			do {
				if (knob & c_bitKnobL)
					m_nKnobL[iIo]++;
				if (knob & c_bitKnobR)
					m_nKnobR[iIo]++;
			} while (false);
		}//while(pEvent)

		//按键信息储存起来后，每帧只取用一个
		if (m_lstPoll[iIo].size() > 0)
		{
			tagComButton btns = m_lstPoll[iIo].front();
			for (int i = 0; i < sc_nBtnBits; ++i)
			{
				m_btn[iIo][e_addr_p1*sc_nBtnBits + i] = (btns.player[0] & (1 << i)) > 0;
				m_btn[iIo][e_addr_p2*sc_nBtnBits + i] = (btns.player[1] & (1 << i)) > 0;
				m_btn[iIo][e_addr_p3*sc_nBtnBits + i] = (btns.player[2] & (1 << i)) > 0;
				m_btn[iIo][e_addr_p4*sc_nBtnBits + i] = (btns.player[3] & (1 << i)) > 0;
				m_btn[iIo][e_addr_mgr*sc_nBtnBits + i] = (btns.mgr & (1 << (i + sc_nBtnBits))) > 0;
				m_btn[iIo][e_addr_mgr2*sc_nBtnBits + i] = (btns.mgr & (1 << i)) > 0;
			}
			m_lstPoll[iIo].pop_front();
		}


		for (int i = 0; i < emax_addr * sc_nBtnBits; ++i)
		{
			m_btnUpTimeLast[iIo][i] = m_btnUpTime[iIo][i];
			if (m_btn[iIo][i])
			{
				m_btnKeepTime[iIo][i] += dt;
				m_btnUpTime[iIo][i] = 0;
			}
			else
			{
				m_btnKeepTime[iIo][i] = 0;
				m_btnUpTime[iIo][i] += dt;
			}

			m_btnTimeSpace[iIo][i] += dt;
		}//for(i)
	}// for(iIo)
}

int CComThread::GetKnob(eKnobDirc dirc)
{
	FUNC_HEAD_COM_X(0);

	switch (dirc)
	{
	case CComThread::e_knob_l:
		return m_nKnobL[0];
	case CComThread::e_knob_r:
		return m_nKnobR[0];
	}
	return 0;
}


static int tmpIO;
static int tmpPid;
bool CComThread::isAddrBitWrong(int addr, int bit, bool bMgr)
{
	if (bit < 0 || bit >= CComThread::sc_nBtnBits || addr < 0)
		return true;
	if (bMgr && addr >= 2 * m_nIoCount || !bMgr && addr >= 4 * m_nIoCount)
		return true;

	tmpIO = bMgr ? (addr / 2) : (addr / 4);
	tmpPid = bMgr ? (addr % 2) : (addr % 4);
	return false;
}
bool CComThread::Check(eBtnAddr addr, eBtnBit bit, bool bMgr)
{
	FUNC_HEAD_COM_X(false);

	if (isAddrBitWrong(addr, bit, bMgr))
		return false;

	return m_btn[tmpIO][tmpPid * sc_nBtnBits + bit];
}

bool CComThread::CheckOnce(eBtnAddr addr, eBtnBit bit, bool bMgr)
{
	FUNC_HEAD_COM_X(false);

	if (isAddrBitWrong(addr, bit, bMgr))
		return false;

	return m_btn[tmpIO][tmpPid * sc_nBtnBits + bit] && !m_btnLast[tmpIO][tmpPid * sc_nBtnBits + bit];
}

bool CComThread::CheckSpace(eBtnAddr addr, eBtnBit bit, float time, bool bMgr)
{
	FUNC_HEAD_COM_X(false);

	if (isAddrBitWrong(addr, bit, bMgr))
		return false;

	if (m_btn[tmpIO][tmpPid * sc_nBtnBits + bit] && m_btnTimeSpace[tmpIO][tmpPid * sc_nBtnBits + bit] >= time)
	{
		m_btnTimeSpace[tmpIO][tmpPid * sc_nBtnBits + bit] = 0;
		return true;
	}

	return false;
}

bool CComThread::CheckUp(eBtnAddr addr, eBtnBit bit, bool bMgr)
{
	FUNC_HEAD_COM_X(false);

	if (isAddrBitWrong(addr, bit, bMgr))
		return false;

	bool bUp = !m_btn[tmpIO][tmpPid * sc_nBtnBits + bit] && m_btnLast[tmpIO][tmpPid * sc_nBtnBits + bit];
	return bUp;
}

bool CComThread::CheckUpDelay(eBtnAddr addr, eBtnBit bit, float time, bool bMgr)
{
	FUNC_HEAD_COM_X(false);

	if (isAddrBitWrong(addr, bit, bMgr))
		return false;

	bool bLast = m_btnUpTimeLast[tmpIO][tmpPid * sc_nBtnBits + bit] < time;
	bool bCur = m_btnUpTime[tmpIO][tmpPid * sc_nBtnBits + bit] >= time;
	return bLast && bCur;
}

const bool* CComThread::GetButtonArray(int io) const
{
	return m_btn[io];
}

std::string CComThread::GetStringOfButtonStage(int io)
{
	string str = "";
	for (int i = 0; i < emax_addr * sc_nBtnBits; ++i)
	{
		str += (m_btn[io][i] ? StringUtils::format("%d", i%sc_nBtnBits) : "_");
	}
	return str;
}

void CComThread::SetEggOut(int pid, int value)
{
	FUNC_HEAD_COM;
	if (isAddrBitWrong(pid, 0, false))
		return;

	if (m_bEnableCoin)
		m_io[tmpIO]->SetCoinOutValue(tmpPid, m_io[tmpIO]->GetCoinOut(tmpPid) + value);
	else
		CCLOG("coin out disable!");

	dataUse_OutEgg(pid, value);
}

void CComThread::SetTickOut(int pid, int value)
{
	FUNC_HEAD_COM;
	if (isAddrBitWrong(pid, 0, false))
		return;

	if (m_bEnableTicket)
		m_io[tmpIO]->SetTicketOutValue(tmpPid, m_io[tmpIO]->GetTicketOut(tmpPid) + value);
	else
		CCLOG("ticket out disable!");

	dataUse_OutTick(pid, value);
}

bool CComThread::IsEggOutError(int pid)
{
	FUNC_HEAD_COM_X(false);
	if (isAddrBitWrong(pid, 0, false))
		return false;

	return m_bEnableCoin && m_io[tmpIO]->IsCoinError(tmpPid);
}

int CComThread::GetTicketOut(int pid)
{
	if (isAddrBitWrong(pid, 0, false))
		return 0;

	return m_io[tmpIO]->GetTicketOut(tmpPid);
}

bool CComThread::IsTickOutError(int pid)
{
	FUNC_HEAD_COM_X(false);
	if (isAddrBitWrong(pid, 0, false))
		return false;

	return m_bEnableTicket && m_io[tmpIO]->IsTicketError(tmpPid);
}

bool CComThread::ioIsError(int io)
{
	if (io < 0 || io >= m_nIoCount)	return false;

	return m_io[io]->IsError();
}

bool equalComPolling(tagComButton* a, tagComButton* b)
{
	if (a->mgr != b->mgr)
		return false;
	for (int i = 0; i < COM_ADDR_COUNT; ++i)
	{
		if (a->player[i] != b->player[i])
			return false;
	}
	return true;
}

unsigned char ioErrorStatus()
{
#ifdef USE_COM
	unsigned char sta = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (g_pCom->ioIsError(i))
			sta |= (1 << i);
	}
	return sta;
#else
	return false;
#endif // USE_COM
}

#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
void ioOpenMotor(int index)
{
	CCLOG("open: [%d, %d]", def_devMotor[index].pid, def_devMotor[index].bit);
	g_pCom->SetDevOut(def_devMotor[index].pid, def_devMotor[index].bit, true);
}
#else
void ioOpenLock(int v, int h)
{
	CCLOG("open: [%d, %d] + [%d, %d]", def_devRelayOut_V[v].pid, def_devRelayOut_V[v].bit, def_devRelayOut_H[h].pid, def_devRelayOut_H[h].bit);
	g_pCom->SetDevOut(def_devRelayOut_V[v].pid, def_devRelayOut_V[v].bit, true);
	g_pCom->SetDevOut(def_devRelayOut_H[h].pid, def_devRelayOut_H[h].bit, true);
}
#endif


#undef FUNC_HEAD_COM