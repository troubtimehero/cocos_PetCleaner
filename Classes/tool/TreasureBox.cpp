#include "TreasureBox.h"


TreasureBox::TreasureBox()
{
}


TreasureBox::~TreasureBox()
{
}

void TreasureBox::init(const std::string &backName, const std::string &openName, int frames, int rows, int cols, float interval, Vec2 openOffset)
{
	Node::init();
	this->setCascadeColorEnabled(true);
	this->setCascadeOpacityEnabled(true);


	m_back = Sprite::create(backName);
	this->addChild(m_back);

	m_open = SpriteAnimation::createAni(openName, frames, rows, cols, interval);
	addChild(m_open);
	m_open->setPosition(openOffset);
}

void TreasureBox::openBox(bool bWin, float hideDelay)
{
	if (bWin)
	{
		auto act2 = Sequence::createWithTwoActions(ScaleTo::create(0.5f, 0), Hide::create());

		m_open->play(false);		this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.84f/*0.4f*/ + hideDelay + 1), act2));
	}
	else
	{
		m_open->play(false);
		this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), TintTo::create(0.5f, Color3B::GRAY)));
	}
}

void TreasureBox::flowCoin(int nCoin)
{
	//喷发金币特效
	auto coin = Sprite::create("image/tips/coin.png");
	this->addChild(coin);
	coin->setRotation(random(0.0f, 360.0f));
	coin->setOpacity(0);
	coin->runAction(Spawn::create(
		FadeIn::create(0.05f),
		Sequence::createWithTwoActions(JumpTo::create(1, Point(random(-300.0f, 300.0f), random(-400.0f, -200.0f)), random(300.0f, 500.0f), 1), RemoveSelf::create()),
		nullptr));
}
