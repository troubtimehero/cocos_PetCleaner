#pragma once
#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
USING_NS_CC;
#include "tool/InputNumber.h"

//////////////////////////////////////////////////////////////////////////
class MyMenuStaticLang : public MenuItemFont
{
public:
	enum eLang
	{
		langCn,
		langEn
	};
	MyMenuStaticLang(const std::string& valueEn, const std::string& valueCn);
	static MyMenuStaticLang * create(const std::string& valueEn, const std::string& valueCn);
	void updateLanguage(eLang lang);
	void update(float delta);
private:
	std::string		m_valueEn;
	std::string		m_valueCn;
};


//////////////////////////////////////////////////////////////////////////
// 暂时有问题，箭头不对齐
class MyMenuFontCanChange : public MenuItemFont
{
public:
	/** Creates a menu item from a string with a target/selector. */
	static MyMenuFontCanChange * create(const std::string& value, const ccMenuCallback& callback, const std::string& arrowLeft, Vec2 offset);

	virtual void onEnter();
	virtual void update(float delta);

	enum eDirection
	{
		e_none,
		e_left,
		e_right
	};
	eDirection getTouchingDirection(){ return m_eDirection; }

private:
	void touchEvent(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);

	int		m_space;
	eDirection	m_eDirection;
};

//////////////////////////////////////////////////////////////////////////
class MyMenuInputNumber : public MenuItemFont, public InputNumberGetting
{
	MyMenuInputNumber(int iMin, int iMax, InputNumberOnMessage* target) : InputNumberGetting(iMin, iMax, target) {}
public:
	static MyMenuInputNumber* MyCreate(const std::string& value, int iMin, int iMax, InputNumberOnMessage* target, const ccMenuCallback& callback);
};
