#include "ShowTouchPoint.h"

USING_NS_CC;

#include "tool/MyTools.h"

void ShowTouchPoint(cocos2d::Node* tar, cocos2d::Point pos, float scale, float duration)
{

	ParticleSystemQuad* par = ParticleSystemQuad::create("Particles/ExplodingRing.plist");
	tar->addChild(par);
	par->setPosition(pos);
	par->setScale(scale);
	par->setDuration(duration);
	par->start(); par->runAction(Sequence::createWithTwoActions(DelayTime::create(1), RemoveSelf::create()));
}
