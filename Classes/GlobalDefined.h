#ifndef __GLOBAL_DEFINED_H__
#define __GLOBAL_DEFINED_H__

#include "BuildVersion.h"
// #include "D:/Pro_share/myLibBase.h"

/************************************************************************/
/*                           ��Ϸ����                                   */
/************************************************************************/
#define GAME_NAME	"PetCleaner"
#define SETTING_SELECT_GAME		//���ý����л���Ϸ

#ifdef USE_COM
// #define USE_KNOB	//ʹ����ť
#endif // USE_COM
#define USE_KNOB_ADDR	5	//��ť��IO����������еĵ�ַ( 1-4P��0-3, MGR: 4-5 )

#define CAN_CHOOSE_WORK_TIME	//�û���������ѡ����ϴ����ʱ��
#define SCAN_MUSIC		//���л���������

/************************************************************************/
/*                           ��������                                   */
/************************************************************************/
#define BUY_NONE	0
#define BUY_EGG		1
#define BUY_GIFT	2
#define VERSION_BUY_TYPE		BUY_EGG	//


/************************************************************************/
/*                           Ť����                                     */
/************************************************************************/
#define USE_BUY_EGG			//���Թ���Ť��
#define COUNT_WIN	8		//Ť��������
// #define SINGLE_EGG_OUT	//ֻʹ��һ����Ť��λ


#define CENT_P_COIN			100

/************************************************************************/
/*                           Ť����                                     */
/************************************************************************/
#define LYY_USE_BOXWAY_FOR_PET	//����ҡҡ��������Ϊ ��ϴ��¼
#define LYY_PET_PRICE_COUNT	5	//�۸�ֶ�������1Ԫ���£�10Ԫ���£�10-20Ԫ��20-30Ԫ��30����
#define LYY_PET_WORK_COUNT	3	//����ģʽ���������� ϴ���桢ϴ��


//////////////////////////////////////////////////////////////////////////

#define USER_DEF_GAME_INDEX		"GAME_INDEX"
#define USER_DEF_COM_PORT		"ComPort"
#define USER_DEF_COM_BOX		"ComBox"
#define USER_DEF_DISPLAY_STATS	"DISPLAY_STATS"
#define USER_DEF_PRICE_SEPARATE	"PRICE_SEPARATE"
#define MENU_BACKGROUND_COLOR	Color4B(200, 200, 200, 255)


#endif	//__GLOBAL_DEFINED_H__
