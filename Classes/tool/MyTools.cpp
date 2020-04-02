#include "MyTools.h"
#include "iconv.h"
//#pragma comment(lib, "libiconv.lib")
#include "GlobalDefined.h"
#include "GameData.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//字符转换，使cocos2d-x在win32平台支持中文显示
int GBKToUTF8(std::string &gbkStr, const char* toCode, const char* formCode)
{
	iconv_t iconvH;
	iconvH = iconv_open(formCode, toCode);
	if (iconvH == 0)
	{
		return -1;
	}

	const char* strChar = gbkStr.c_str();
	const char** pin = &strChar;

	size_t strLength = gbkStr.length();
	char* outbuf = (char*)malloc(strLength * 4);
	char* pBuff = outbuf;
	memset(outbuf, 0, strLength * 4);
	size_t outLength = strLength * 4;
	if (-1 == iconv(iconvH, pin, &strLength, &outbuf, &outLength))
	{
		iconv_close(iconvH);
		return -1;
	}

	gbkStr = pBuff;
	iconv_close(iconvH);
	return 0;
}
/**
**在封装一层，直接传入一个string，转换后还回对应的编码给你
*/
const char* GBKToUTF(std::string &gbkStr)
{
	GBKToUTF8(gbkStr, "gbk", "utf-8"); //后面两个参数就默认了,免得后面再传参麻烦

	return gbkStr.c_str();
}

#include <stdarg.h>
std::string LanguageChoose(const char* ch, const char* en, ...)
{
#define CC_MAX_STRING_LENGTH (1024*100)

	std::string ret;

	va_list ap;
	va_start(ap, en);

	char* buf = (char*)malloc(CC_MAX_STRING_LENGTH);
	if (buf != nullptr)
	{
		vsnprintf(buf, CC_MAX_STRING_LENGTH, IsChinese() ? ch : (en ? en : ch), ap);
		ret = buf;
		free(buf);
	}
	va_end(ap);

	return GBKToUTF(ret);
}

float myRandom(float min, float max)
{
	return min + (max - min) * cocos2d::rand_0_1();
}

std::string myGetResourcePath()
{
	string str = cocos2d::UserDefault::getInstance()->getXMLFilePath();
	return str.substr(0, str.rfind('/')+1);
}

void ScanFile(const std::string& dir, std::vector<std::string>& vecFile)
{
	//查找所有文件类型
	std::string path = dir + "\\*.*";
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(path.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		//过滤目录当前目录和父目录
		if (wfd.cFileName[0] == '.')
			continue;

		std::string subPath = dir + "\\" + wfd.cFileName;
		//扫描子目录
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ScanFile(subPath, vecFile);
		}
		//保存路径
		else
		{
			vecFile.push_back(subPath);
		}
	} while (FindNextFile(hFind, &wfd));

	FindClose(hFind);
}

void DisableBelowTouch(Node* n)
{
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [](Touch *, Event *){
		return true;
	};
	listener->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, n);
}

void EnableBelowTouch(Node* n)
{
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(n);
	n->setVisible(false);
}

std::string CentToChar(unsigned int cent)
{
	std::string str;
	str = StringUtils::format("%u.%02u", cent / CENT_P_COIN, cent % CENT_P_COIN);
// 	if (cent % 10 == 0)	//百分位为零
// 		str += "0";
	return str;
}

std::string IntToFloat(unsigned int number, int exponent)
{
	std::string str;
	switch (exponent)
	{
	case 1:
		str = StringUtils::format("%u.%u", number / 10, number % 10);
		break;
	case 2:
		str = StringUtils::format("%u.%02u", number / 100, number % 100);
		break;
	case 3:
		str = StringUtils::format("%u.%03u", number / 1000, number % 1000);
		break;
	default:
		str = StringUtils::format("%u", number);
	}

	return str;
}

std::string NumberToTime(unsigned int second)
{
	int h = second / 3600;
	second %= 3600;
	int m = second / 60;
	second %= 60;

	string str;
	if (h > 0)
		str = StringUtils::format("%u:%02u:%02u", h, m, second);
	else
		str = StringUtils::format("%02u:%02u", m, second);

	return str;
}

bool ShowInsertMoreCoin(Node* caller, string boxName, int nNeed, PayTarget* payTar, string QrCodeLayerName)
{
	//币不够，显示提示
	if (nNeed > 0)
	{
#if 0
		Label* tips = (Label*)caller->getChildByName(childName);
		if (!tips)
		{
			tips = createFontTTF("fonts/dai meng wa wa ti.ttf", 100, 15, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
			caller->addChild(tips);
			tips->setName(childName);
			tips->setPosition(VisibleRect::center());
			tips->setZOrder(100);
		}
		tips->setString(GBKToUTF(StringUtils::format("再投 %d 个币", nNeed)));
		tips->stopAllActions();
		tips->runAction(Sequence::create(Show::create(), ScaleTo::create(0.3f, 1.2f), ScaleTo::create(0.1f, 1), DelayTime::create(10), Hide::create(), nullptr));
#endif

#if 0
		g_pLeyaoyao->enableShowQRCode(boxName, nNeed, 60);
#endif
		PayByQRCode* qr = PayByQRCode::MyCreate(boxName, nNeed, CONTROL_TIMEOUT, payTar);
		caller->addChild(qr);
		qr->setName(QrCodeLayerName);
		qr->setZOrder(100001);

		return true;
	}
	return false;
}

ui::Button* createEmptyBtnBack()
{
	ui::Button* back = ui::Button::create("image/menu/button_empty.png");
	back->setTitleText(LanguageChoose("<返回", "<Back"));
	back->setTitleColor(Color3B::BLACK);
	back->setTitleFontSize(53);
	back->setPosition(back->getContentSize() / 2);
	return back;
}

Label* createButtonTTF(ui::Button* btn, std::string filepath, int size, int outlineSize, Color4B colorText, Color4B colorGlow, Color4B colorOutline)
{
	auto name = createFontTTF(filepath, size, outlineSize, colorText, colorGlow, colorOutline, false);
	name->setPosition(btn->getContentSize() / 2);
	btn->addChild(name);
	btn->setCascadeColorEnabled(true);
	btn->setCascadeOpacityEnabled(true);

	return name;
}

void SayReadyGo(Node* caller, const char* childName /*= "ready go"*/)
{
	auto ready = Sprite::create("image/tips/ready.png");
	caller->addChild(ready);
	ready->setPosition(VisibleRect::center());
	ready->setOpacity(0);
	ready->setScale(2);
	ready->runAction(Sequence::create(
		CallFunc::create(CC_CALLBACK_0(MySound::callbackPlayEffect, &g_sound, "sound/ready go.mp3")),
		Spawn::createWithTwoActions(ScaleTo::create(0.5f, 1), FadeIn::create(0.5f)),
		DelayTime::create(0.5f),
		RemoveSelf::create(),
		nullptr));

	auto go = Sprite::create("image/tips/go.png");
	caller->addChild(go);
	go->setName(childName);
	go->setPosition(VisibleRect::center());
	go->setOpacity(0);
	go->setScale(2);
	go->runAction(Sequence::create(
		DelayTime::create(1),
		Spawn::createWithTwoActions(ScaleTo::create(0.5f, 1), FadeIn::create(0.5f)),
		DelayTime::create(0.5f),
		RemoveSelf::create(),
		nullptr));
}

#include "tool/IniFile.h"
int MyIniGetInt(const char* lpszSectionName, const char* lpszKeyName, int def /*= 0*/)
{
#if 1
	CIniFile ini("setting.ini");
	return ini.GetProfileInt(lpszSectionName, lpszKeyName, def);
#else
	return UserDefault::getInstance()->getIntegerForKey(lpszKeyName, def);
#endif
}

std::string MyIniGetString(const char* lpszSectionName, const char* lpszKeyName, const char* def /*= ""*/)
{
#if 1
	CIniFile ini("setting.ini");
	return ini.GetProfileString(lpszSectionName, lpszKeyName, def);
#else
	return UserDefault::getInstance()->getStringForKey(lpszKeyName, def);
#endif
}

cocos2d::Rect ScaleRect(const Rect rc, float scaleX, float scaleY)
{
	float mx = rc.getMidX();
	float my = rc.getMidY();
	float w = rc.size.width;
	float h = rc.size.height;

	Rect rc2;
	rc2.setRect(mx - w*scaleX, my - h*scaleY, w*scaleX, h*scaleY);
	return rc2;
}

#endif 