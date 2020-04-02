#pragma once
#include <cocos2d.h>
USING_NS_CC;

#include "QRCode/QRSprite.h"

//扫码、投币成功后，直接执行回调函数
class PayTarget
{
public:
	PayTarget(){}
	~PayTarget(){}

	virtual void callbackPayOK() = 0;
	virtual void callbackCancel(){}
};


class PayByQRCode : public LayerColor
{
public:
	static PayByQRCode* MyCreate(std::string boxName, int tarCent, float showTime, PayTarget* payTar);

	virtual bool init(std::string boxName, int needCent, float showTime, PayTarget* payTar);
	virtual void update(float delta);


public:
	void timerAutoBack(float delta);

private:
	Sprite*				m_payBackground;
	DrawNode*			m_qrCode;
	float			m_fRemainTime;

	int				m_nTarCoin;
	PayTarget*		m_payTarger;
};
