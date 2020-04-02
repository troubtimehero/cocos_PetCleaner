#ifndef __MACHINE_DEFINED_H__
#define __MACHINE_DEFINED_H__

#include "BuildVersion.h"
#include "GlobalDefined.h"
#include <cocos2d.h>
// #include "D:/Pro_share/myLibBase.h"

/************************************************************************/
/*                           口红机                                     */
/************************************************************************/
//可容纳的物品数： 口红机 1， 福袋机 N
#define BOX_EACH_ONE			1
#define BOX_EACH_MUTL			4
#define BOX_CONTAIN_GOODS		BOX_EACH_MUTL

struct tagDevRelayInfo	//----------------------- [索引] 对应 { IO板P位, 灯输出位 }
{
	int pid;
	int bit;
};
typedef tagDevRelayInfo tagComBtnIndex;

#define ROTATE_SCREEN		0	//竖屏，1是 0否

#define GAME_WIDTH	1280
#define GAME_HEIGHT	800
#define SCREEN_WIDTH	GAME_WIDTH
#define SCREEN_HEIGHT	GAME_HEIGHT


#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)

#define GAME_COUNT	1		//设置界面可切换游戏数

#define COUNT_BOX		12
#define COUNT_PRICE		12
// 输出：开锁、电机转动
const tagDevRelayInfo def_devMotor[] = {
	{ 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
	{ 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
	{ 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 },
	{ 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 },
	{ 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 }
};
// 输入：电机微动检测
const tagComBtnIndex def_btnMotor[] = {
	{ 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
	{ 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
	{ 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 },
	{ 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 },
	{ 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 }
};

#define IO_USED_COUNT		1

#else


#define GAME_COUNT	2		//设置界面可切换游戏数

#define COUNT_BOX		30
#define COUNT_PRICE		30
// 输出：开锁、电机转动
const tagDevRelayInfo def_devRelayOut_V[] = {
	{ 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 }
};
const tagDevRelayInfo def_devRelayOut_H[] = {
	{ 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 }
};

#define IO_USED_COUNT		1

#endif

//各窗口的  横竖索引()、屏幕位置、大小
//大小
const cocos2d::Size box_GetSize(int index);

//位置
const cocos2d::Point box_GetPosition(int index);

//索引
POINT box_GetPidBit(int index);	//通过 顺序 获取 数据存储中的位置
int box_GetIndex(int pid, int bit);	//通过 数据存储中的位置 获取 顺序

// 价格设置
#define BUY_PRICE_MIN		1
#define	BUY_PRICE_MAX		20000
const int arrGameCentSetting[] = {
	0, 10, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1500, 2000, 3000, 4000, 5000
};



#endif	//__MACHINE_DEFINED_H__
