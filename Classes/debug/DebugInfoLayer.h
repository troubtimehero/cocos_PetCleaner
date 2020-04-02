#pragma once
#include "cocos2d.h"
USING_NS_CC;

class DebugInfoLayer : public Layer
{
public:
	bool init();
	CREATE_FUNC(DebugInfoLayer);

	static void updateString(std::string str);
	static void myRelease();
	static Label* getTextLabel(){ return m_text; }
private:
	static Label*		m_text;
};

