#ifndef __BUILD_VERSION_H__
#define __BUILD_VERSION_H__

#define	VERSION_MAJOR_NUMBER		1	//���汾��(��1��ʼ,����Ч�����������д��޸ľͼ�1)
#define VERSION_MINOR_NUMBER		0	//�ΰ汾��(��0��ʼ,�����ܡ��㷨��С�޸ľͼ�1)
#define VERSION_REVISION_NUMBER		0	//�����汾��(��0��ʼ,�д����Ż���bug�޸��ͼ�1)
#define VERSION_BUILD_NUMBER		2	//����汾��(���ƺ꿪��,�ο�����ı���汾����)

#define USE_REMOVEDISK_UPDATE_EXE	//U���Զ�����


#if (VERSION_BUILD_NUMBER == 0)
// #define TEST_ALG
#define USE_COM


#elif	(VERSION_BUILD_NUMBER==1)
#define FULL_SCREEN				//ȫ����ʾ
#define LOAD_ENCODE_RESOUCE		//��ȡ���ܵ�ͼƬ��Ч
#define USE_COM
#define USE_VERIFY				//���빦��


#elif	(VERSION_BUILD_NUMBER==2)
#define FULL_SCREEN				//ȫ����ʾ
#define LOAD_ENCODE_RESOUCE		//��ȡ���ܵ�ͼƬ��Ч
#define FORBID_KEYBOARD			//���μ��̰���
#define USE_COM
#define USE_VERIFY				//���빦��

#endif // VERSION_BUILD_NUMBER == 0


#ifdef _DEBUG
#undef FULL_SCREEN
#endif // _DEBUG

#endif	//__BUILD_VERSION_H__
