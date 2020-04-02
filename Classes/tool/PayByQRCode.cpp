#include "PayByQRCode.h"
#include "MachineDefined.h"
#include "tool/MyTools.h"
#include "GameData.h"
#include "ioCtrl/LeYaoYao.h"


PayByQRCode* PayByQRCode::MyCreate(std::string boxName, int tarCent, float showTime, PayTarget* payTar)
{
	PayByQRCode* ret = new (std::nothrow) PayByQRCode();
	if (ret && ret->init(boxName, tarCent, showTime, payTar))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}

	return ret;
}

const Point ptCode = { 1000, 555 };
bool PayByQRCode::init(std::string strName, int needCent, float showTime, PayTarget* payTar)
{
	LayerColor::initWithColor(Color4B::WHITE, GAME_WIDTH, GAME_HEIGHT);

	g_pBox->BoxReportStatus(io_LYY_Good::eBoxStatus::e_box_pay);
	g_pBox->BoxRequesQRCode(needCent);

	DisableBelowTouch(this);

	m_payBackground = Sprite::create("image2/buy/payBackground.png");
	this->addChild(m_payBackground);
	m_payBackground->setContentSize(VisibleRect::size());
	m_payBackground->setPosition(VisibleRect::center());


	Label* boxName = Label::createWithSystemFont(strName, "黑体", 75);
	this->addChild(boxName);
	boxName->setName("boxName");
	boxName->setTextColor(Color4B::RED);
#if (ROTATE_SCREEN == 1)
	boxName->setPosition(GAME_WIDTH / 2, GAME_HEIGHT * 4 / 5);
#else
	boxName->setPosition(416, 610);
#endif

	m_nTarCoin = needCent + dataGet_RemainCent();
	Label* need = Label::createWithSystemFont(GBKToUTF(StringUtils::format(IsChinese() ? "共需支付：￥%s元" : "Need to Pay: %s", CentToChar(needCent).c_str())), "黑体", 63);
	this->addChild(need);
	need->setName("need");
	need->setTextColor(Color4B::BLACK);
	need->setPosition(boxName->getPosition() + Vec2(0, -240));


	Label* timer = Label::createWithSystemFont("", "黑体", 60);
	this->addChild(timer);
	timer->setName("timer");
	timer->setTextColor(Color4B::RED);
	timer->setPosition(need->getPosition() + Vec2(0, -160));

	m_fRemainTime = showTime;
	schedule(schedule_selector(PayByQRCode::timerAutoBack), 1, CC_REPEAT_FOREVER, 0);


	m_qrCode = nullptr;

	m_payTarger = payTar;

	auto back = createEmptyBtnBack();
	this->addChild(back);
	back->addClickEventListener([this](Ref*){
		g_pBox->BoxReportStatus(io_LYY_Good::eBoxStatus::e_box_free);
		this->m_payTarger->callbackCancel();
		this->removeFromParent();
	});


#ifdef _DEBUG
// 	m_qrCode = QRSprite::create("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
// 	this->addChild(m_qrCode);
// 	m_qrCode->setAnchorPoint(Point::ANCHOR_MIDDLE);
// 	m_qrCode->setPosition(ptCode);
#endif // _DEBUG


	scheduleUpdate();

	return true;
}

void PayByQRCode::update(float delta)
{
	if (dataGet_RemainCent() >= m_nTarCoin)
	{
		m_payTarger->callbackPayOK();
		g_pBox->BoxReportStatus(io_LYY_Good::eBoxStatus::e_box_game);
		removeFromParent();
	}


	//------------------------------------- 二维码 --------------------------------
	// 0x10 更新二维码
	if (!m_qrCode && g_pBox->BoxIsQRCodeGot())
	{
		CCLOG(g_pBox->BoxGetQRCode());
		m_qrCode = QRSprite::create(g_pBox->BoxGetQRCode());
		this->addChild(m_qrCode);
		m_qrCode->setAnchorPoint(Point::ANCHOR_MIDDLE);
#if (ROTATE_SCREEN == 1)
		m_qrCode->setPosition(VisibleRect::center());
#else
		m_qrCode->setPosition(ptCode);
#endif
	}
}


void PayByQRCode::timerAutoBack(float delta)
{
	int lastTime = static_cast<int>(m_fRemainTime);
	m_fRemainTime -= delta;
	int curTime = static_cast<int>(m_fRemainTime);

	if (curTime != lastTime)
	{
		Label* timer = (Label*)this->getChildByName("timer");
		timer->setString(GBKToUTF(StringUtils::format(IsChinese() ? "%d 秒" : "%d sec", curTime)));
	}

	if (m_fRemainTime <= 0)
	{
		g_pBox->BoxReportStatus(io_LYY_Good::eBoxStatus::e_box_free);
		m_payTarger->callbackCancel();
		this->removeFromParent();
	}
}
