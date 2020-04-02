#ifndef __BUILD_VERSION_H__
#define __BUILD_VERSION_H__

#define	VERSION_MAJOR_NUMBER		1	//主版本号(从1开始,界面效果、程序框架有大修改就加1)
#define VERSION_MINOR_NUMBER		0	//次版本号(从0开始,程序框架、算法有小修改就加1)
#define VERSION_REVISION_NUMBER		0	//修正版本号(从0开始,有代码优化、bug修复就加1)
#define VERSION_BUILD_NUMBER		2	//编译版本号(控制宏开关,参考下面的编译版本定义)

#define USE_REMOVEDISK_UPDATE_EXE	//U盘自动更新


#if (VERSION_BUILD_NUMBER == 0)
// #define TEST_ALG
#define USE_COM


#elif	(VERSION_BUILD_NUMBER==1)
#define FULL_SCREEN				//全屏显示
#define LOAD_ENCODE_RESOUCE		//读取加密的图片音效
#define USE_COM
#define USE_VERIFY				//打码功能


#elif	(VERSION_BUILD_NUMBER==2)
#define FULL_SCREEN				//全屏显示
#define LOAD_ENCODE_RESOUCE		//读取加密的图片音效
#define FORBID_KEYBOARD			//屏蔽键盘按键
#define USE_COM
#define USE_VERIFY				//打码功能

#endif // VERSION_BUILD_NUMBER == 0


#ifdef _DEBUG
#undef FULL_SCREEN
#endif // _DEBUG

#endif	//__BUILD_VERSION_H__
