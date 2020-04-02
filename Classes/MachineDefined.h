#ifndef __MACHINE_DEFINED_H__
#define __MACHINE_DEFINED_H__

#include "BuildVersion.h"
#include "GlobalDefined.h"
#include <cocos2d.h>
// #include "D:/Pro_share/myLibBase.h"

/************************************************************************/
/*                           �ں��                                     */
/************************************************************************/
//�����ɵ���Ʒ���� �ں�� 1�� ������ N
#define BOX_EACH_ONE			1
#define BOX_EACH_MUTL			4
#define BOX_CONTAIN_GOODS		BOX_EACH_MUTL

struct tagDevRelayInfo	//----------------------- [����] ��Ӧ { IO��Pλ, �����λ }
{
	int pid;
	int bit;
};
typedef tagDevRelayInfo tagComBtnIndex;

#define ROTATE_SCREEN		0	//������1�� 0��

#define GAME_WIDTH	1280
#define GAME_HEIGHT	800
#define SCREEN_WIDTH	GAME_WIDTH
#define SCREEN_HEIGHT	GAME_HEIGHT


#if (BOX_CONTAIN_GOODS == BOX_EACH_MUTL)

#define GAME_COUNT	1		//���ý�����л���Ϸ��

#define COUNT_BOX		12
#define COUNT_PRICE		12
// ��������������ת��
const tagDevRelayInfo def_devMotor[] = {
	{ 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
	{ 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
	{ 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 },
	{ 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 },
	{ 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 }
};
// ���룺���΢�����
const tagComBtnIndex def_btnMotor[] = {
	{ 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
	{ 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
	{ 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 },
	{ 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 },
	{ 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 }
};

#define IO_USED_COUNT		1

#else


#define GAME_COUNT	2		//���ý�����л���Ϸ��

#define COUNT_BOX		30
#define COUNT_PRICE		30
// ��������������ת��
const tagDevRelayInfo def_devRelayOut_V[] = {
	{ 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 }
};
const tagDevRelayInfo def_devRelayOut_H[] = {
	{ 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 }
};

#define IO_USED_COUNT		1

#endif

//�����ڵ�  ��������()����Ļλ�á���С
//��С
const cocos2d::Size box_GetSize(int index);

//λ��
const cocos2d::Point box_GetPosition(int index);

//����
POINT box_GetPidBit(int index);	//ͨ�� ˳�� ��ȡ ���ݴ洢�е�λ��
int box_GetIndex(int pid, int bit);	//ͨ�� ���ݴ洢�е�λ�� ��ȡ ˳��

// �۸�����
#define BUY_PRICE_MIN		1
#define	BUY_PRICE_MAX		20000
const int arrGameCentSetting[] = {
	0, 10, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1500, 2000, 3000, 4000, 5000
};



#endif	//__MACHINE_DEFINED_H__
