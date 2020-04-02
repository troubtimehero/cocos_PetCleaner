#pragma once
#include "MachineDefined.h"


const cocos2d::Size box_GetSize(int index)
{
#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
	return{ 0.7f, 0.8f };
#else
	if (index < 15)
	{
		return{ 0.9f, 0.5f };
	}
	else
	{
		return{ 0.5f, 0.33f };
	}
#endif
}

const cocos2d::Point box_GetPosition(int index)
{
	cocos2d::Point pt;

#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
	const float blx = GAME_WIDTH / 5.0f;
	const float bly = GAME_HEIGHT / 4.5f;
	pt.x = blx * (index % 4 + 1);
	pt.y = GAME_HEIGHT / 4.5f * (3.3f - index / 4);
#else
	const float blx = GAME_WIDTH / 5.0f;
	const float bly = GAME_HEIGHT / 6.0f;
	pt.x = blx * (index % 4 + 1);
	pt.y = GAME_HEIGHT / 6 * (4 - index / 4);
#endif

	return pt;
}

#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)
const POINT arrBoxPidBitOfData[COUNT_BOX] = {
	{ 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
	{ 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
	{ 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 }
};
#else
const POINT arrBoxPidBitOfData[COUNT_BOX] = {
	{ 0, 0 }, { 0, 1 }, { 0, 2 },
	{ 1, 0 }, { 1, 1 }, { 1, 2 },
	{ 2, 0 }, { 2, 1 }, { 2, 2 },
	{ 3, 0 }, { 3, 1 }, { 3, 2 },
	{ 4, 0 }, { 4, 1 }, { 4, 2 },
	{ 5, 0 }, { 5, 1 }, { 5, 2 }, { 5, 3 }, { 5, 4 },
	{ 6, 0 }, { 6, 1 }, { 6, 2 }, { 6, 3 }, { 6, 4 },
	{ 7, 0 }, { 7, 1 }, { 7, 2 }, { 7, 3 }, { 7, 4 }
};
#endif

POINT box_GetPidBit(int index)	//通过 顺序 获取 数据存储中的位置
{
	return arrBoxPidBitOfData[index];
}
int box_GetIndex(int pid, int bit)	//通过 数据存储中的位置 获取 顺序
{
	for (size_t i = 0; i < ARRAYSIZE(arrBoxPidBitOfData); i++)
	{
		if (arrBoxPidBitOfData[i].x == pid && arrBoxPidBitOfData[i].y == bit)
			return i;
	}
	return -1;
}
