//
//  QRSprite.hpp
//  CGame
//
//  Created by neo on 15/12/16.
//
//
#include "cocos2d.h"
#ifndef QRSprite_hpp
#define QRSprite_hpp
using namespace cocos2d;

//#if COCOS2D_VERSION <0x00030000


class QRSprite
	{
public:	
	static DrawNode* create(const char *code);
};
//tolua_end



#endif /* QRSprite_hpp */