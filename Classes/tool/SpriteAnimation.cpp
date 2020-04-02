#include "SpriteAnimation.h"

//------------------------------------------------------------------
//
// SpriteAnimation
//
//------------------------------------------------------------------
#define SPRITE_ANIMATION_ACTION_TAG		1

SpriteAnimation* SpriteAnimation::createAni(const std::string &picName, int frames, int rows, int cols, float interval, const std::string &aniCacheName)
{
	SpriteAnimation *pRet = new(std::nothrow) SpriteAnimation();
	if (pRet && pRet->init(picName, frames, rows, cols, interval, aniCacheName))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}


bool SpriteAnimation::init(const std::string &picName, int frames, int rows, int cols, float interval, const std::string &aniCacheName)
{
	Node::init();
	setCascadeColorEnabled(true);
	setCascadeOpacityEnabled(true);


	m_strAniCacheName = aniCacheName.empty() ? picName : aniCacheName;
	
	if (!AnimationCache::getInstance()->getAnimation(m_strAniCacheName))
	{
		auto texture = Director::getInstance()->getTextureCache()->addImage(picName);

		float w = texture->getContentSize().width / cols;
		float h = texture->getContentSize().height / rows;

		Vector<SpriteFrame*> m_frames(frames);
		for (int i = 0; i < frames; ++i)
		{
			int idxX = i % cols;
			int idxY = i / cols;
			auto f = SpriteFrame::createWithTexture(texture, Rect(w * idxX, h * idxY, w, h));
			m_frames.pushBack(f);
		}

		auto animation = Animation::createWithSpriteFrames(m_frames, interval);
		AnimationCache::getInstance()->addAnimation(animation, m_strAniCacheName);

		m_ani = Sprite::createWithSpriteFrame(m_frames.at(0));
	}

	m_ani = Sprite::createWithSpriteFrame(AnimationCache::getInstance()->getAnimation(m_strAniCacheName)->getFrames().at(0)->getSpriteFrame());
	addChild(m_ani);

	this->setContentSize(m_ani->getContentSize());

	return true;
}

void SpriteAnimation::play(bool bRepeat, int nDelay, float* arrDelay)
{
	auto animate = Animate::create(AnimationCache::getInstance()->getAnimation(m_strAniCacheName));

	m_ani->stopAllActions();
	Action* act = nullptr;
	if (bRepeat)
	{
		if (nDelay > 0 && arrDelay != nullptr)
		{
			Vector<FiniteTimeAction*> actArray;
			actArray.clear();
			for (int i = 0; i < nDelay; ++i)
			{
				actArray.pushBack(animate);
				actArray.pushBack(DelayTime::create(arrDelay[i]));
			}

			act = RepeatForever::create(Sequence::create(actArray));
		}
		else
		{
			act = RepeatForever::create(animate);
		}
	}
	else
		act = animate;
	act->setTag(SPRITE_ANIMATION_ACTION_TAG);
	m_ani->runAction(act);
}

void SpriteAnimation::stopAt(int index /*= -1*/)
{
	m_ani->stopAllActions();
	
	if (index < 0)
		index = AnimationCache::getInstance()->getAnimation(m_strAniCacheName)->getFrames().size() - 1;
		
	m_ani->setSpriteFrame(AnimationCache::getInstance()->getAnimation(m_strAniCacheName)->getFrames().at(index)->getSpriteFrame());
}

bool SpriteAnimation::isDone()
{
	auto act = m_ani->getActionByTag(SPRITE_ANIMATION_ACTION_TAG);
	if (!act || act->isDone())
		return true;
	return false;
}

Sprite* SpriteAnimation::getAni()
{
	return m_ani;
}

#undef SPRITE_ANIMATION_ACTION_TAG