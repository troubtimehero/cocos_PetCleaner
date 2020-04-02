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

//ɾ��ָ��
#define SAFE_DELETE_POINT(p)	{ if (p){ delete p; p = nullptr; } }

//λ����
template<typename T>
void MySetBit(T& src, int pos, bool bit)
{
	if (bit)
		src |= (1 << pos);
	else
		src &= (~(1 << pos));
}

//�����²㴥��
void DisableBelowTouch(Node* n);
//��ֹ����
void EnableBelowTouch(Node* n);

//���� 1��ת�ɡ�Ԫ 0.01��
std::string CentToChar(unsigned int cent);
//������ת��С����exponent��С��λ����ʱֻ֧�� 0 - 3
std::string IntToFloat(unsigned int number, int exponent);

//���� 100��ת�ɡ���׼ʱ�� 1:40��
std::string NumberToTime(unsigned int second);

//��ʾҪ��Ͷ�ң�Ҫ�Ļ�����true
#ifdef _DEBUG
#define CONTROL_TIMEOUT	10
#else
#define CONTROL_TIMEOUT	180
#endif // _DEBUG
bool ShowInsertMoreCoin(Node* caller, string boxName, int nNeed, PayTarget* payTar, string QrCodeLayerName = "QrCode");

//��ͼ����
ui::Button* createEmptyBtnBack();

//Ϊ��ť����TTF����
Label* createButtonTTF(ui::Button* btn, std::string filepath, int size, int outlineSize, Color4B colorText, Color4B colorGlow, Color4B colorOutline);

//��ʾ��˵ Ready Go
void SayReadyGo(Node* caller, const char* childName = "ready go");

//random
float myRandom(float min, float max);

//��Դ·��
string myGetResourcePath();

//�����ļ�������ļ�
void ScanFile(const std::string& dir, std::vector<std::string>& vecFile);


//��ȡ�Զ�������
int MyIniGetInt(const char* lpszSectionName, const char* lpszKeyName, int def = 0);
std::string MyIniGetString(const char* lpszSectionName, const char* lpszKeyName, const char* def = "");

Rect ScaleRect(const Rect rc, float scaleX, float scaleY);

#endif