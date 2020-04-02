#ifndef __VISIBLERECT_H__
#define __VISIBLERECT_H__

#include "cocos2d.h"


class VisibleRect
{
public:
	static void SetVisibleRect(cocos2d::Rect winRect);

	static cocos2d::Vec2 left();
	static cocos2d::Vec2 right();
	static cocos2d::Vec2 top();
	static cocos2d::Vec2 bottom();
	static cocos2d::Vec2 center();
	static cocos2d::Vec2 leftTop();
	static cocos2d::Vec2 rightTop();
	static cocos2d::Vec2 leftBottom();
	static cocos2d::Vec2 rightBottom();
	static cocos2d::Size size();
private:
	static cocos2d::Rect s_visibleRect;
};

cocos2d::Scene* HorizontalScreen(cocos2d::Scene *s);


#endif /* __VISIBLERECT_H__ */
