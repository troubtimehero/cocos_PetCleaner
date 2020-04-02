#ifndef __GLOBAL_DEFINED_H__
#define __GLOBAL_DEFINED_H__

#include "BuildVersion.h"
// #include "D:/Pro_share/myLibBase.h"

/************************************************************************/
/*                           游戏属性                                   */
/************************************************************************/
#define GAME_NAME	"PetCleaner"
#define SETTING_SELECT_GAME		//设置界面切换游戏

#ifdef USE_COM
// #define USE_KNOB	//使用旋钮
#endif // USE_COM
#define USE_KNOB_ADDR	5	//旋钮在IO板接收数据中的地址( 1-4P：0-3, MGR: 4-5 )

#define CAN_CHOOSE_WORK_TIME	//用户可以自由选择清洗与烘干时间
#define SCAN_MUSIC		//可切换背景音乐

/************************************************************************/
/*                           购买内容                                   */
/************************************************************************/
#define BUY_NONE	0
#define BUY_EGG		1
#define BUY_GIFT	2
#define VERSION_BUY_TYPE		BUY_EGG	//


/************************************************************************/
/*                           扭蛋机                                     */
/************************************************************************/
#define USE_BUY_EGG			//可以购买扭蛋
#define COUNT_WIN	8		//扭蛋窗口数
// #define SINGLE_EGG_OUT	//只使用一个退扭蛋位


#define CENT_P_COIN			100

/************************************************************************/
/*                           扭蛋机                                     */
/************************************************************************/
#define LYY_USE_BOXWAY_FOR_PET	//把乐摇摇货道定义为 清洗记录
#define LYY_PET_PRICE_COUNT	5	//价格分段数量：1元以下，10元以下，10-20元，20-30元，30以上
#define LYY_PET_WORK_COUNT	3	//工作模式分类数量： 洗、烘、洗烘


//////////////////////////////////////////////////////////////////////////

#define USER_DEF_GAME_INDEX		"GAME_INDEX"
#define USER_DEF_COM_PORT		"ComPort"
#define USER_DEF_COM_BOX		"ComBox"
#define USER_DEF_DISPLAY_STATS	"DISPLAY_STATS"
#define USER_DEF_PRICE_SEPARATE	"PRICE_SEPARATE"
#define MENU_BACKGROUND_COLOR	Color4B(200, 200, 200, 255)


#endif	//__GLOBAL_DEFINED_H__
