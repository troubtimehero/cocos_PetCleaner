#pragma once
#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
USING_NS_CC;


//////////////////////////////////////////////////////////////////////////
class LayerNeedPassword
{
public:
	LayerNeedPassword(){}
	~LayerNeedPassword(){}

	virtual void onPasswordOK(std::string password) = 0;
};


class LayerPassword : public LayerColor
{
public:
	static LayerPassword* MyCreate(LayerNeedPassword* caller, std::string password);
	virtual bool init(LayerNeedPassword* caller, std::string password);

private:
	void initPassword();
	void onBtnNumber(int num);
	void onBtnDel();

	Label*	m_input;
	LayerNeedPassword*	m_caller;
	std::string			m_password;
};

