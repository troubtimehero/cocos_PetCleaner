#pragma once
#include <cocos2d.h>
USING_NS_CC;
#include "cocos/ui/CocosGUI.h"

Label* createFontTTF(std::string filepath, int size, int outlineSize, Color4B colorText, Color4B colorGlow, Color4B colorOutline, bool bEnableShadow, TextHAlignment align = TextHAlignment::CENTER);

void ChangeLetterColor(Label* l, int from, int to, Color3B col);

#define DEF_TAG_ONSALE_FRAME		100
Sprite* setBoxTitleFrame(ui::Button* btn, bool bFrame);	//要在 SetPosition() 和 SetScale() 之后