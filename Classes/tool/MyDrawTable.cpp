#include "MyDrawTable.h"
#include "tool/MyTools.h"


//////////////////////////////////////////////////////////////////////////

MyDrawTable* MyDrawTable::MyCreate(int nRow, int nCol, float width, float height, float lineSize, Color4F color)
{
	MyDrawTable *btn = new (std::nothrow) MyDrawTable();
	if (btn && btn->init(nRow, nCol, width, height, lineSize, color))
	{
		btn->autorelease();
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
}

bool MyDrawTable::init(int nRow, int nCol, float width, float height, float lineSize, Color4F color)
{
	DrawNode::init();

	float unitWidth = width / nCol;
	float unitHeight = height / nRow;
	float hls = lineSize / 2;
	for (int i = 0; i <= nRow; ++i)
	{
		this->drawSolidRect(Vec2(-hls, unitHeight * i - hls), Vec2(width + hls, unitHeight*i + hls), color);
	}
	for (int i = 0; i <= nCol; ++i)
	{
		this->drawSolidRect(Vec2(unitWidth*i - hls, -hls), Vec2(unitWidth*i + hls, height + hls), color);
	}

	this->setContentSize(Size(width + lineSize, height + lineSize));

	return true;
}

void MyDrawTable::myAddLine(Point p1, Point p2, float lineSize, Color4F color)
{
	float hls = lineSize / 2;
	this->drawSolidRect(p1 + Vec2(-hls, -hls), p2 + Vec2(hls, hls), color);
}
