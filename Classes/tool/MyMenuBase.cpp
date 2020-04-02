#include "MyMenuBase.h"
#include "tool/MyTools.h"

MyMenuStaticLang * MyMenuStaticLang::create(const std::string& valueEn, const std::string& valueCn)
{
	std::string str = valueCn;
	GBKToUTF(str);

	MyMenuStaticLang *ret = new (std::nothrow) MyMenuStaticLang(valueEn, str);
	ret->initWithString(valueEn, (const ccMenuCallback&)nullptr);
	ret->autorelease();
	return ret;
}

MyMenuStaticLang::MyMenuStaticLang(const std::string& valueEn, const std::string& valueCn)
{
	m_valueEn = valueEn;
	m_valueCn = valueCn;
	scheduleUpdate();
}

void MyMenuStaticLang::updateLanguage(eLang lang)
{
	setString(langEn == lang ? m_valueEn : m_valueCn);
}

void MyMenuStaticLang::update(float delta)
{

}


//////////////////////////////////////////////////////////////////////////


MyMenuFontCanChange * MyMenuFontCanChange::create(const std::string& value, const ccMenuCallback& callback, const std::string& arrowRight, Vec2 offset)
{
	MyMenuFontCanChange *ret = new (std::nothrow) MyMenuFontCanChange();
	ret->initWithString(value, callback);
	ret->autorelease();

	auto left = ui::Button::create(arrowRight);
	ret->addChild(left);
	left->setPosition(Vec2(-offset.x, offset.y));
	left->setFlipX(true);
	left->addTouchEventListener(CC_CALLBACK_2(MyMenuFontCanChange::touchEvent, ret));
	left->setTag(0);

	auto right = ui::Button::create(arrowRight);
	ret->addChild(right);
	right->setPosition(offset);
	right->addTouchEventListener(CC_CALLBACK_2(MyMenuFontCanChange::touchEvent, ret));
	right->setTag(1);

	return ret;
}

void MyMenuFontCanChange::touchEvent(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	int tag = ((ui::Button*)pSender)->getTag();
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		switch (tag)
		{
		case 0:	m_eDirection = e_left;	break;
		case 1:	m_eDirection = e_right;	break;
		}
		m_space = 0;
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		m_eDirection = e_none;
		break;
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		m_eDirection = e_none;
		break;
	default:
		break;
	}
}

void MyMenuFontCanChange::onEnter()
{
	MenuItemFont::onEnter();
	scheduleUpdate();
}

void MyMenuFontCanChange::update(float delta)
{
	MenuItemFont::update(delta);

	if (m_eDirection != e_none && ++m_space > 10)
	{
		m_space = 0;
		this->activate();
	}
}


//////////////////////////////////////////////////////////////////////////

MyMenuInputNumber* MyMenuInputNumber::MyCreate(const std::string& value, int iMin, int iMax, InputNumberOnMessage* target, const ccMenuCallback& callback)
{
	MyMenuInputNumber *ret = new (std::nothrow) MyMenuInputNumber(iMin, iMax, target);
	ret->initWithString(value, callback);
	ret->autorelease();
	return ret;
}
