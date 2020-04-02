#include "MyFont.h"
#include "MyTools.h"

Label* createFontTTF(std::string filepath, int size, int outlineSize, Color4B colorText, Color4B colorGlow, Color4B colorOutline, bool bEnableShadow, TextHAlignment align /*= TextHAlignment::CENTER*/)
{
	TTFConfig ttfConfig(filepath, size, GlyphCollection::DYNAMIC, nullptr, true);

	auto label = Label::createWithTTF(ttfConfig, "", align);
	label->setTextColor(colorText);
	label->enableGlow(colorGlow);
	if (outlineSize > 0)
		label->enableOutline(colorOutline, outlineSize);
	if (bEnableShadow)
		label->enableShadow();

	return label;
}

void ChangeLetterColor(Label* l, int from, int to, Color3B col)
{
	int len = l->getStringLength();
	for (int i = from; i <= to; ++i)
	{
		if (i >= len)
			break;

		if (l->getLetter(i))
			l->getLetter(i)->setColor(col);
	}
}

Sprite* setBoxTitleFrame(ui::Button* btn, bool bFrame)
{
	if (bFrame)
	{
		btn->setTitleFontName("fonts/W9_337623.ttf");
	}

	string str = btn->getTitleText();
	int idx = str.find('\n');
	if (idx > 0)
	{
		str.replace(idx, 1, "\n");
	}
	else
	{
		str.append("\n");
	}
	btn->setTitleText(str);
	btn->setTitleFontSize(68);

	Label* l = btn->getTitleLabel();
#if 1
	btn->setTitleColor(Color3B(255, 255, 41));
	ChangeLetterColor(l, 2, 20, Color3B::BLACK);
#else
	btn->setTitleColor(Color3B(255, 0, 255));
	ChangeLetterColor(l, 0, 1, Color3B(255, 255, 41));
#endif

#if 0
#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
	ChangeLetterColor(l, 3, 3, Color3B::GRAY);

	Vec2 off = Vec2(0, -btn->getContentSize().height * 0.59f);
	l->setPosition(l->getPosition() + off);

	if (bFrame)
	{
		static int s_boxAnchorPointY = MyIniGetInt("SETTING", "Box_AnchorPoint_Y");

		Sprite* frame = Sprite::create("image2/buy/textFrame.png");
		btn->getParent()->addChild(frame);
		frame->setAnchorPoint(Vec2(0.5f, s_boxAnchorPointY / 100.0f));

		float scale = btn->getScaleY();
		frame->setPosition(btn->getPosition() + off * scale);
		frame->setScale(btn->getScaleX(), btn->getScaleY());

		frame->setZOrder(1);
		btn->setZOrder(2);
		btn->setCascadeColorEnabled(true);
		btn->setCascadeOpacityEnabled(true);

		return frame;
	}
#endif
#endif

	return nullptr;
}
