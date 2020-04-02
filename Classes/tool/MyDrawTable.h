#pragma once
#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
USING_NS_CC;


//////////////////////////////////////////////////////////////////////////
class MyDrawTable : public DrawNode
{
public:
	/** Creates a DrawNode with row, col, width, height and lineSize.
	*
	* @param row 行数
	* @param col 列数
	* @param width 整体宽度
	* @param height 整体高度
	* @param lineSize 线宽
	* @return An autoreleased DrawNode object.
	*/
	static MyDrawTable* MyCreate(int nRow, int nCol, float width, float height, float lineSize, Color4F color);
	
	bool init(int nRow, int nCol, float width, float height, float lineSize, Color4F color);

	void myAddLine(Point p1, Point p2, float lineSize, Color4F color);
};
