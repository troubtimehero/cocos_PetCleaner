#pragma once
#include "cocos2d.h"
USING_NS_CC;

class SpritePolygon : public Sprite
{
public:
	static SpritePolygon* MyCreate(const char* filename, PhysicsMaterial pm, float epsilon);

protected:
	bool initShape(const char* filename, PhysicsMaterial pm, float epsilon);
};
