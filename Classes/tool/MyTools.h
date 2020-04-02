#ifndef _TOOLS_H_
#define  _TOOLS_H_

#include "cocos2d.h"
USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

int GBKToUTF8(std::string &gbkStr, const char* toCode, const char* formCode);

const char* GBKToUTF(std::string &gbkStr);
std::string LanguageChoose(const char* ch, const char* en, ...);

#endif

#include "VisibleRect.h"

#include "RandomController.h"

#include "MyAnimation.h"
#include "FramesAnimation.h"

#include "MySound.h"
#include "MyFont.h"

#include "LayerLanguage.h"

#include "PayByQRCode.h"

inline void debugOut(const char* sz)
{
	CCLOG(sz);
}

//删除指针
#define SAFE_DELETE_POINT(p)	{ if (p){ delete p; p = nullptr; } }

//位设置
template<typename T>
void MySetBit(T& src, int pos, bool bit)
{
	if (bit)
		src |= (1 << pos);
	else
		src &= (~(1 << pos));
}

//屏蔽下层触摸
void DisableBelowTouch(Node* n);
//终止屏蔽
void EnableBelowTouch(Node* n);

//【分 1】转成【元 0.01】
std::string CentToChar(unsigned int cent);
//把整数转成小数，exponent：小数位，暂时只支持 0 - 3
std::string IntToFloat(unsigned int number, int exponent);

//【秒 100】转成【标准时间 1:40】
std::string NumberToTime(unsigned int second);

//提示要再投币，要的话返回true
#ifdef _DEBUG
#define CONTROL_TIMEOUT	10
#else
#define CONTROL_TIMEOUT	180
#endif // _DEBUG
bool ShowInsertMoreCoin(Node* caller, string boxName, int nNeed, PayTarget* payTar, string QrCodeLayerName = "QrCode");

//无图返回
ui::Button* createEmptyBtnBack();

//为按钮创建TTF字体
Label* createButtonTTF(ui::Button* btn, std::string filepath, int size, int outlineSize, Color4B colorText, Color4B colorGlow, Color4B colorOutline);

//显示和说 Ready Go
void SayReadyGo(Node* caller, const char* childName = "ready go");

//random
float myRandom(float min, float max);

//资源路径
string myGetResourcePath();

//遍历文件夹里的文件
void ScanFile(const std::string& dir, std::vector<std::string>& vecFile);


//获取自定义设置
int MyIniGetInt(const char* lpszSectionName, const char* lpszKeyName, int def = 0);
std::string MyIniGetString(const char* lpszSectionName, const char* lpszKeyName, const char* def = "");

Rect ScaleRect(const Rect rc, float scaleX, float scaleY);

#endif