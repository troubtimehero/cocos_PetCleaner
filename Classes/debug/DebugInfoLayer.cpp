#include "DebugInfoLayer.h"
#include "GameData.h"
#include "tool/MyTools.h"

Label* DebugInfoLayer::m_text = nullptr;
bool DebugInfoLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	if (nullptr == m_text)
	{
		m_text = Label::createWithSystemFont("xxx", "ºÚÌå", 30);
		m_text->retain();
	}
	m_text->setAnchorPoint(Vec2(0, 0.5));
	m_text->setPosition(Vec2(300, 20));
	this->addChild(m_text);

	return true;
}

void DebugInfoLayer::updateString(std::string str)
{
	if (m_text)
		m_text->setString(str);
}

void DebugInfoLayer::myRelease()
{
	if (m_text)
		m_text->autorelease();
}
