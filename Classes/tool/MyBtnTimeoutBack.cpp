#include "MyBtnTimeoutBack.h"
#include "tool/MyTools.h"


void TimeoutBackTarget::callbackTimeoutBack()
{
	this->removeFromParent();
}


//////////////////////////////////////////////////////////////////////////

MyBtnTimeoutBack* MyBtnTimeoutBack::MyCreate(const std::string &normalImage, int timeout, TimeoutBackTarget* target)
{
	MyBtnTimeoutBack *btn = new (std::nothrow) MyBtnTimeoutBack;
	if (btn && btn->init(normalImage, timeout, target))
	{
		btn->autorelease();
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
}

bool MyBtnTimeoutBack::init(const std::string& normalImage, int timeout, TimeoutBackTarget* target)
{
	ui::Button::init(normalImage);
	m_timeout = timeout;
	m_bEnableTimeout = true;

	this->setName(BUTTON_NAME_BACK_TIMEOUT);
	this->setPosition(this->getContentSize() / 2);

	Size s = this->getContentSize();

	this->setTitleFontSize(53);
	this->setTitleText(LanguageChoose("<返回", "<Back"));
	this->setTitleColor(Color3B::BLACK);
	this->addClickEventListener([this](Ref* ref){
		m_target->callbackTimeoutBack();
	});


	m_lNum = Label::createWithSystemFont(StringUtils::format("%d", timeout), "黑体", 53);
	this->addChild(m_lNum);
	m_lNum->setPosition(s.width + s.height, s.height / 2);


	if (target)
	{
		m_target = target;
		schedule(schedule_selector(MyBtnTimeoutBack::timerCheckTimeout), 1, CC_REPEAT_FOREVER, 0);//3秒显示“正在准备”
	}

	return true;
}

void MyBtnTimeoutBack::timerCheckTimeout(float delta)
{
	if (m_bEnableTimeout)
	{
		m_timeout--;
		m_lNum->setString(StringUtils::format("%d", m_timeout));

		if (m_timeout <= 0)
		{
			m_target->callbackTimeoutBack();
		}
	}
}

void MyBtnTimeoutBack::enableTimeout(bool b)
{
	m_bEnableTimeout = b;
}

void MyBtnTimeoutBack::resetTimeout(int t)
{
	m_timeout = t;
}
