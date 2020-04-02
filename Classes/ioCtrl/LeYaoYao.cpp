#include "LeYaoYao.h"
#include "debug/DebugInfoLayer.h"
#include "GameData.h"
#include "tool/MyTools.h"
#include "ioCtrl/ComThread.h"
#include "IoCtrlLayer.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/


io_lyy_Pet::io_lyy_Pet(int port) : io_LYY_Good(4180, port)
{
	m_washBaseTime = 0;
	m_washIncTime = 0;
	m_washBasePrice_10 = 0;
	m_washIncPrice_10 = 0;

	m_dryBaseTime = 0;
	m_dryIncTime = 0;
	m_dryBasePrice_10 = 0;
	m_dryIncPrice_10 = 0;

	m_saleMode = 0;
	m_gamePrice_10 = 0;

	m_enterSetting = 0;

	m_bNewSetting = false;
}

int io_lyy_Pet::OnMsgUpSetting()
{
	m_szSendData[0] = m_washBaseTime;
	m_szSendData[1] = m_washIncTime;
	StrMakeShort(2, m_washBasePrice_10);
	m_szSendData[4] = m_washIncPrice_10;

	m_szSendData[5] = m_dryBaseTime;
	m_szSendData[6] = m_dryIncTime;
	StrMakeShort(7, m_dryBasePrice_10);
	m_szSendData[9] = m_dryIncPrice_10;

	m_szSendData[10] = m_saleMode;
	m_szSendData[11] = m_gamePrice_10;

	m_szSendData[12] = 0;	//上传始终为0，不用 m_enterSetting

	return 13;
}

void io_lyy_Pet::OnMsgDownSetting()
{
	m_bNewSetting = true;

	m_washBaseTime = StrGetByte(0);
	m_washIncTime = StrGetByte(1);
	m_washBasePrice_10 = StrGetShort(2);
	m_washIncPrice_10 = StrGetByte(4);

	m_dryBaseTime = StrGetByte(5);
	m_dryIncTime = StrGetByte(6);
	m_dryBasePrice_10 = StrGetShort(7);
	m_dryIncPrice_10 = StrGetByte(9);

	m_saleMode = StrGetByte(10);
	m_gamePrice_10 = StrGetByte(11);

	m_enterSetting = StrGetByte(12);

	DownSetting();
}

void io_lyy_Pet::ReportSetting()
{
	m_washBaseTime		= g_pData->get(eDataKey::set_PetWash_BaseTime);
	m_washIncTime		= g_pData->get(eDataKey::set_PetWash_IncTime);
	m_washBasePrice_10	= g_pData->get(eDataKey::set_PetWash_BasePrice);
	m_washIncPrice_10	= g_pData->get(eDataKey::set_PetWash_IncPrice);

	m_dryBaseTime		= g_pData->get(eDataKey::set_PetDry_BaseTime);
	m_dryIncTime		= g_pData->get(eDataKey::set_PetDry_IncTime);
	m_dryBasePrice_10	= g_pData->get(eDataKey::set_PetDry_BasePrice);
	m_dryIncPrice_10	= g_pData->get(eDataKey::set_PetDry_IncPrice);

	m_saleMode			= g_pData->get(eDataKey::set_saleMode);
	m_gamePrice_10		= dataGet_GameCent() / 10;

	m_enterSetting		= 0;
}

bool io_lyy_Pet::IsNewSetting()
{
	return m_bNewSetting;
}

void io_lyy_Pet::DownSetting()
{
	g_pData->set(eDataKey::set_PetWash_BaseTime,	m_washBaseTime);
	g_pData->set(eDataKey::set_PetWash_IncTime,		m_washIncTime);
	g_pData->set(eDataKey::set_PetWash_BasePrice,	m_washBasePrice_10);
	g_pData->set(eDataKey::set_PetWash_IncPrice,	m_washIncPrice_10);

	g_pData->set(eDataKey::set_PetDry_BaseTime,		m_dryBaseTime);
	g_pData->set(eDataKey::set_PetDry_IncTime,		m_dryIncTime);
	g_pData->set(eDataKey::set_PetDry_BasePrice,	m_dryBasePrice_10);
	g_pData->set(eDataKey::set_PetDry_IncPrice,		m_dryIncPrice_10);

	g_pData->set(eDataKey::set_saleMode,			m_saleMode);
	g_pData->set(eDataKey::set_GameCent,			m_gamePrice_10 * 10);

	//远程控制进入菜单
}


/************************************************************************/
/*                                                                      */
/************************************************************************/

#ifdef LYY_USE_BOXWAY_FOR_PET
static io_LYY_Good::tagWayInfo s_wayInfo[LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT + COUNT_BOX];
const char* petCleanRecName[LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT] = {
	"洗烘(0-1元)", "洗烘(1-10元)", "洗烘 (10-20元)", "洗烘(20-30元)", "洗烘(30以上)",
	"净洗(0-1元)", "净洗(1-10元)", "净洗 (10-20元)", "净洗(20-30元)", "净洗(30以上)",
	"净烘(0-1元)", "净烘(1-10元)", "净烘 (10-20元)", "净烘(20-30元)", "净烘(30以上)"
};
#else
static io_LYY_Good::tagWayInfo s_wayInfo[COUNT_BOX];
#endif // LYY_USE_BOXWAY_FOR_PET

bool LeYaoYao::init()
{
	Node::init();

#ifdef LYY_USE_BOXWAY_FOR_PET

	//清洗记录
	for (int i = 0; i < LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT; ++i)
	{
		s_wayInfo[i].buyCent = 5000;
		s_wayInfo[i].gameCent = 0;
		s_wayInfo[i].remain = 9999;
		s_wayInfo[i].contain = 9999;
		s_wayInfo[i].nameLen = strlen(petCleanRecName[i]);
		sprintf(s_wayInfo[i].name, petCleanRecName[i]);
	}

	//售货记录
	for (int i = 0; i < COUNT_BOX; ++i)
	{
		int k = i + LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT;

		s_wayInfo[k].buyCent = dataGet_BuyCent(i);
		s_wayInfo[k].gameCent = dataGet_GameCent();
		s_wayInfo[k].remain = dataGet_OnSale(i);
		s_wayInfo[k].contain = BOX_CONTAIN_GOODS;
		s_wayInfo[k].nameLen = 2;
		POINT vh = box_GetPidBit(i);
		sprintf(s_wayInfo[k].name, "%c%d", 'A' + vh.x, 1 + vh.y);
	}

	g_pBox->BoxReportWayInfo(s_wayInfo, LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT);

#else

	for (int i=0; i<COUNT_BOX; ++i)
	{
		s_wayInfo[i].buyPrice = dataGet_BuyCent(i);
		s_wayInfo[i].gamePrice = dataGet_GameCent();
		s_wayInfo[i].remain = dataGet_OnSale(i);
		s_wayInfo[i].contain = BOX_CONTAIN_GOODS;
		s_wayInfo[i].nameLen = 2;
		POINT vh = box_GetPidBit(i);
		sprintf(s_wayInfo[i].name, "%c%d", 'A' + vh.x, 1 + vh.y);
	}

	g_pBox->BoxReportWayInfo(s_wayInfo, COUNT_BOX);
#endif // LYY_USE_BOXWAY_FOR_PET


#if 0
	auto signalBack = Sprite::create("image/tips/signal_back.png");
	this->addChild(signalBack);
	signalBack->setName("signal");
	signalBack->setPosition(VisibleRect::rightTop() - signalBack->getContentSize() / 2);

	m_signal = ui::Slider::create("image/tips/signal.png", "");
	signalBack->addChild(m_signal);
	m_signal->setPosition(signalBack->getContentSize() / 2);
#endif


	// 触摸响应
	m_fNoTouchTime = 0;
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch *, Event *){
		this->m_fNoTouchTime = 0;
		return false;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void LeYaoYao::onEnter()
{
	Node::onEnter();

	scheduleUpdate();

	schedule(schedule_selector(LeYaoYao::timerReport), 30, CC_REPEAT_FOREVER, 0);
	m_iTimerStep = 0;
}

void LeYaoYao::update(float delta)
{
	g_pBox->FrameFunc(delta);

	if (g_pData->get(eDataKey::tmp_RemainCent) > 0)
	{
		m_fNoTouchTime += delta;
	}

	// 0x03	云上分
// 	if (g_pBox->DownInsertCoin() > 0)
// 	{
// 		debugOut("second coin to pay");
// 		g_pIoCtrlLayer->myAddCent(g_pBox->DownInsertCoin() * CENT_P_COIN, true);
// 	}

	// BOX 0x03
	if (g_pBox->BoxDownPayCent() > 0)
	{
		debugOut("scan to pay");
		g_pIoCtrlLayer->myAddCent(g_pBox->BoxDownPayCent());
	}

	// 0x06 更改游戏设置，直接在LYY中更新

#if 0
	// 0x10 更新二维码
	if (g_pBox->IsNewQRCode())
	{
		if (m_qrCode)
		{
			m_qrCode->removeFromParent();
		}

		m_qrCode = QRSprite::create((char*)g_pBox->DownQRCode());
		m_payBackground->addChild(m_qrCode);
		if (m_qrCode->getContentSize().width > 0)
		{
			m_qrCode->setAnchorPoint(Point::ANCHOR_MIDDLE);
			m_qrCode->setPosition(VisibleRect::center());
		}
		else
		{
			m_qrCode->setPosition(VisibleRect::leftBottom());
		}
	}
#endif
}

static io_LYY_Good::eErrorType		s_lastError = io_LYY_Good::errNone;
static io_LYY_Good::eErrorType		s_newError = io_LYY_Good::errNone;
void funcUpAccountSuccess()
{
	g_pData->set(eDataKey::report_giftOut, g_pData->get(eDataKey::rec_giftOut));
	g_pData->set(eDataKey::report_centIn, g_pData->get(eDataKey::rec_CentIn));
	s_lastError = s_newError;
}
void LeYaoYao::timerReport(float delta)
{
	switch (m_iTimerStep)
	{
	case 0:	// 0x13 上传账目、故障码
			// 以及所有状态上报
		do {
			int giftOut = g_pData->get(eDataKey::rec_giftOut) - g_pData->get(eDataKey::report_giftOut);
//			int coinIn = g_pData->get(eDataKey::rec_CentIn) - g_pData->get(eDataKey::report_centIn);
			int coinIn = 0;

			// 查看错误信息 [11/20/2018 pc-ah]
			if (ioErrorStatus() > 0)
				s_newError = io_LYY_Good::errIo;
			else if (m_fNoTouchTime > 3600)	//一小时没触摸
				s_newError = io_LYY_Good::errMachine;
			else if (dataGet_CountOnSale() <= 0)
				s_newError = io_LYY_Good::errNoGoods;
			else if (dataGet_CountOnSale() < COUNT_BOX / 2)
				s_newError = io_LYY_Good::errLessGoods;
			else
				s_newError = io_LYY_Good::errNone;


			if (giftOut > 0 || coinIn > 0 || s_newError != s_lastError)
			{
				g_pBox->UpAccountStatus(s_newError, coinIn, giftOut, &funcUpAccountSuccess);
			}

		} while (false);

		ReportAll();

		break;


#if 0
	case 1:	// 0x19
		do {
			int signal;
			if (!g_pBox->GetSignalValue(&signal))
				signal = 0;
			m_signal->setPercent(signal / 31.0f * 100);
		} while (false);
#endif
		break;
	}
	++m_iTimerStep %= 2;
}

void LeYaoYao::ReportActiveStatus()
{
	g_pBox->ReportActiveStatus(1, 1, 1);
}

void LeYaoYao::ReportSetting()
{
	g_pBox->ReportSetting();
}

void LeYaoYao::ReportAll()
{
	ReportActiveStatus();
	ReportSetting();
// 		giftOut[i] = g_pData->get(eDataKey::rec_EggOut_01 + i);
}

void LeYaoYao::SetTradeTemp(int iBoxOutIndex, bool bGame)
{
	tmp_game = bGame;
	tmp_index = iBoxOutIndex;
}

void LeYaoYao::UpTrade(bool bSucceed)
{
	int cent = tmp_game ? dataGet_GameCent() : dataGet_BuyCent(tmp_index);

	//出货失败，相应的记录要恢复
	if (!bSucceed)
	{
		g_pData->decrease(eDataKey::rec_CentIn, cent);
		g_pData->decrease(eDataKey::rec_EggOut_01 + tmp_index, 1);
		g_pData->decrease(eDataKey::rec_LastEggOut_01 + tmp_index, 1);
	}

	int nPay = 0;
	int nRefund = bSucceed ? 0 : cent;
	int nOut = bSucceed ? 1 : 0;
	int nOutTotal = g_pData->get(eDataKey::rec_EggOut_01 + tmp_index);
	int buy = tmp_game ? 0 : cent;
	int game = tmp_game ? cent : 0;
	CCLOG("up online : %d, %d, %d, %d, %d, %d, %d, %d", nPay, nRefund, nOut, tmp_index, nOut, nOutTotal, buy, game);

#ifdef LYY_USE_BOXWAY_FOR_PET
	g_pBox->BoxUpTradeOnline(nPay, nRefund, nOut, tmp_index + LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT, nOut, nOutTotal, buy, game);
#else
	g_pBox->BoxUpTradeOnline(nPay, nRefund, nOut, tmp_index, nOut, nOutTotal, buy, game);
#endif // LYY_USE_BOXWAY_FOR_PET
}

void LeYaoYao::UpTradeGameFailed(int cent, int index, int nBoxOut, int nBoxOutTotall, int buyPrice, int gamePrice)
{
	int nPay = 0;
#ifdef LYY_USE_BOXWAY_FOR_PET
	g_pBox->BoxUpTradeOnline(nPay, 0, 0, index + LYY_PET_PRICE_COUNT * LYY_PET_WORK_COUNT, 0, g_pData->get(eDataKey::rec_EggOut_01 + index), 0, dataGet_GameCent());
#else
	g_pBox->BoxUpTradeOnline(nPay, 0, 0, index, 0, g_pData->get(eDataKey::rec_EggOut_01 + index), 0, dataGet_GameCent());
#endif // LYY_USE_BOXWAY_FOR_PET
}

extern bool s_bCoinIsUpdate;
void LeYaoYao::UpPetTrade(int iWork, int nPay)
{
	int iPrice = 0;
	if (nPay > 3000)
		iPrice = 4;
	else if (nPay > 2000)
		iPrice = 3;
	else if (nPay > 1000)
		iPrice = 2;
	else if (nPay > 100)
		iPrice = 1;

	int index = LYY_PET_PRICE_COUNT * iWork + iPrice;
	int nRefund = 0;
	int nOut = 1;
	int nOutTotal = g_pData->get(eDataKey::rec_clean_01 + index);
	int buy = nPay;
	int game = 0;
	CCLOG("up online : %d, %d, %d, %d, %d, %d, %d, %d", nPay, nRefund, nOut, index, nOut, nOutTotal, buy, game);
	g_pBox->BoxUpTradeOnline(nPay, nRefund, nOut, index, nOut, nOutTotal, buy, game);


	//扣币
	g_pData->decrease(eDataKey::tmp_RemainCent, nPay);

	s_bCoinIsUpdate = true;


	g_pData->increase(eDataKey::rec_clean_01 + index, 1);
	g_pData->increase(eDataKey::rec_lastClean_01 + index, 1);
	g_pData->increase(eDataKey::rec_cleanPay_01 + index, nPay);
	g_pData->increase(eDataKey::rec_lastCleanPay_01 + index, nPay);
}
