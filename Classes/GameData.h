/************************************************************************/
/* 
���࣬��ͬ��ϷҪ��Ӧ��ͬ�����ݣ����Բ��ܹ���
�����ӿ���һ���ģ�ֻҪ֪��KEY��KEY�ĸ�������
*/
/************************************************************************/

#pragma once
#include "debug/DebugInfoLayer.h"
#include <string>
#include "MachineDefined.h"
#include "DataMgr/DataMgr.h"


#define DEF_SET_PET_CLEAN_BASE_MIN	5
#define DEF_SET_PET_CLEAN_BASE_MAX	10
#define DEF_SET_PET_DRY_BASE_MIN	30
#define DEF_SET_PET_DRY_BASE_MAX	50


namespace eDataKey
{
	enum e_GAME_DATA_KEY
	{
		//ϴ��
#ifdef CAN_CHOOSE_WORK_TIME
		set_PetWash_BaseTime,
		set_PetWash_IncTime,
		set_PetWash_BasePrice,	//���� 0.1Ԫ���ǣ�
		set_PetWash_IncPrice,

		set_PetDry_BaseTime,
		set_PetDry_IncTime,
		set_PetDry_BasePrice,	//���� 0.1Ԫ
		set_PetDry_IncPrice,

#else
		set_PetTimeWash_S,	//ϴʱ�䣬���ӣ�minute��
		set_PetTimeWash_M,	
		set_PetTimeWash_L,

		set_PetTimeBlow_S,	//��ʱ�䣬����
		set_PetTimeBlow_M,
		set_PetTimeBlow_L,

		set_PetPriceWash_S,	//��ϴ�۸񣬷֣�cent��
		set_PetPriceWash_M,
		set_PetPriceWash_L,

		set_PetPricBlowe_S,	//�����۸�
		set_PetPricBlowe_M,
		set_PetPricBlowe_L,

		set_PetPriceAll_S,	//ϴ+���۸�
		set_PetPriceAll_M,
		set_PetPriceAll_L,
#endif // CAN_CHOOSE_WORK_TIME

		//////////////////////////////////////////////////////////////////////////


		set_Language,	//����
		set_saleMode,		//�ۻ�ģʽ
		set_GameCent,	//��Ϸ����	//0:��ѣ�1-10
		set_Volume,		//����
		set_BuyCent_01,	//ֱ�ӹ�����Ҫ����	//0:�رգ�1-99	��ע�⡿����Ҫ�� COUNT_PRICE ��Ӧ
		set_BuyCoin_02,
		set_BuyCoin_03,
		set_BuyCoin_04,
		set_BuyCoin_05,
		set_BuyCoin_06,
		set_BuyCoin_07,
		set_BuyCoin_08,
		set_BuyCoin_09,
		set_BuyCoin_10,
		set_BuyCoin_11,
		set_BuyCoin_12,
		set_BuyCoin_13,
		set_BuyCoin_14,
		set_BuyCoin_15,
		set_BuyCoin_16,
		set_BuyCoin_17,
		set_BuyCoin_18,
		set_BuyCoin_19,
		set_BuyCoin_20,
		set_BuyCoin_21,
		set_BuyCoin_22,
		set_BuyCoin_23,
		set_BuyCoin_24,
		set_BuyCoin_25,
		set_BuyCoin_26,
		set_BuyCoin_27,
		set_BuyCoin_28,
		set_BuyCoin_29,
		set_BuyCoin_30,


		rec_CentIn,			//���������
		rec_CentFromNet,	//���Ϸ�
		rec_CentFromGoods,	//�����Ʒ�����棬�������е�һ����
		rec_TickOut,		//����Ʊ��
		rec_EggOut_01,		//��Ť����,��������Ʒ����¼
		rec_EggOut_02,
		rec_EggOut_03,
		rec_EggOut_04,
		rec_EggOut_05,
		rec_EggOut_06,
		rec_EggOut_07,
		rec_EggOut_08,
		rec_EggOut_09,
		rec_EggOut_10,
		rec_EggOut_11,
		rec_EggOut_12,
		rec_EggOut_13,
		rec_EggOut_14,
		rec_EggOut_15,
		rec_EggOut_16,
		rec_EggOut_17,
		rec_EggOut_18,
		rec_EggOut_19,
		rec_EggOut_20,
		rec_EggOut_21,
		rec_EggOut_22,
		rec_EggOut_23,
		rec_EggOut_24,
		rec_EggOut_25,
		rec_EggOut_26,
		rec_EggOut_27,
		rec_EggOut_28,
		rec_EggOut_29,
		rec_EggOut_30,


		rec_LastCentIn,			//�����������
		rec_LastCentFromNet,	//���Ϸ�
		rec_LastCentFromGoods,
		rec_LastTickOut,		//������Ʊ��
		rec_LastEggOut_01,		//����Ť����,��������Ʒ����¼
		rec_LastEggOut_02,
		rec_LastEggOut_03,
		rec_LastEggOut_04,
		rec_LastEggOut_05,
		rec_LastEggOut_06,
		rec_LastEggOut_07,
		rec_LastEggOut_08,
		rec_LastEggOut_09,
		rec_LastEggOut_10,
		rec_LastEggOut_11,
		rec_LastEggOut_12,
		rec_LastEggOut_13,
		rec_LastEggOut_14,
		rec_LastEggOut_15,
		rec_LastEggOut_16,
		rec_LastEggOut_17,
		rec_LastEggOut_18,
		rec_LastEggOut_19,
		rec_LastEggOut_20,
		rec_LastEggOut_21,
		rec_LastEggOut_22,
		rec_LastEggOut_23,
		rec_LastEggOut_24,
		rec_LastEggOut_25,
		rec_LastEggOut_26,
		rec_LastEggOut_27,
		rec_LastEggOut_28,
		rec_LastEggOut_29,
		rec_LastEggOut_30,

		tmp_RemainOfflineCent,	//����Ͷ��(Ԫ)
		tmp_RemainOnlineCent,	//�����Ϸ�(��)
		tmp_RemainCent,		//ʣ����� = ����+����
		tmp_GameBonus,		//��Ϸ����(���� X 10)------------------- Ҳ���� cent
		tmp_OutError,	//δ����Ʒ��
// 		tmp_RemainFireScore,	//ʣ����Ϸѹ��


		status_remainGoods_01,	//��������ÿ��ʣ���Ʒ����
		status_remainGoods_02,
		status_remainGoods_03,
		status_remainGoods_04,
		status_remainGoods_05,
		status_remainGoods_06,
		status_remainGoods_07,
		status_remainGoods_08,
		status_remainGoods_09,
		status_remainGoods_10,
		status_remainGoods_11,
		status_remainGoods_12,
		status_remainGoods_13,
		status_remainGoods_14,
		status_remainGoods_15,
		status_remainGoods_16,
		status_remainGoods_17,
		status_remainGoods_18,
		status_remainGoods_19,
		status_remainGoods_20,
		status_remainGoods_21,
		status_remainGoods_22,
		status_remainGoods_23,
		status_remainGoods_24,
		status_remainGoods_25,
		status_remainGoods_26,
		status_remainGoods_27,
		status_remainGoods_28,
		status_remainGoods_29,
		status_remainGoods_30,


		list_open1_pos,		//������¼�б�
		list_open1_time,
		list_open2_pos,
		list_open2_time,
		list_open3_pos,
		list_open3_time,
		list_open4_pos,
		list_open4_time,
		list_open5_pos,
		list_open5_time,
		list_open6_pos,
		list_open6_time,


		rec_giftOut,
		report_centIn,	// �� rec_CoinIn �Ƚ�
		report_giftOut,	// �� rec_outGift �Ƚ�


		//��ϴ��¼
#ifdef CAN_CHOOSE_WORK_TIME
		//-------------- ��������
		rec_clean_01,	//ϴ�棬�۸�1��//����ģʽΪ��,�۸�Ϊ����Ҫ�� LeYaoYao::petCleanRecName ��Ӧ
		rec_clean_02,	//ϴ�棬�۸�2��
		rec_clean_03,	//ϴ�棬�۸�3��
		rec_clean_04,	//ϴ�棬�۸�4��
		rec_clean_05,	//ϴ�棬�۸�5��

		rec_clean_06,	//ϴ���۸�1
		rec_clean_07,
		rec_clean_08,
		rec_clean_09,
		rec_clean_10,
		
		rec_clean_11,	//��
		rec_clean_12,
		rec_clean_13,
		rec_clean_14,
		rec_clean_15,
		//-------------- ��ģʽ������
		rec_cleanPay_01,
		rec_cleanPay_02,
		rec_cleanPay_03,
		rec_cleanPay_04,
		rec_cleanPay_05,
		
		rec_cleanPay_06,
		rec_cleanPay_07,
		rec_cleanPay_08,
		rec_cleanPay_09,
		rec_cleanPay_10,
		
		rec_cleanPay_11,
		rec_cleanPay_12,
		rec_cleanPay_13,
		rec_cleanPay_14,
		rec_cleanPay_15,


		rec_lastClean_01,//ϴ�棬�۸�1��
		rec_lastClean_02,//ϴ�棬�۸�2��
		rec_lastClean_03,//ϴ�棬�۸�3��
		rec_lastClean_04,//ϴ�棬�۸�4��
		rec_lastClean_05,//ϴ�棬�۸�5��

		rec_lastClean_06,//ϴ���۸�1
		rec_lastClean_07,
		rec_lastClean_08,
		rec_lastClean_09,
		rec_lastClean_10,

		rec_lastClean_11,//��
		rec_lastClean_12,
		rec_lastClean_13,
		rec_lastClean_14,
		rec_lastClean_15,

		rec_lastCleanPay_01,
		rec_lastCleanPay_02,
		rec_lastCleanPay_03,
		rec_lastCleanPay_04,
		rec_lastCleanPay_05,

		rec_lastCleanPay_06,
		rec_lastCleanPay_07,
		rec_lastCleanPay_08,
		rec_lastCleanPay_09,
		rec_lastCleanPay_10,

		rec_lastCleanPay_11,
		rec_lastCleanPay_12,
		rec_lastCleanPay_13,
		rec_lastCleanPay_14,
		rec_lastCleanPay_15,

#else
		note_clean_WB_S,	//ϴ+������
		note_clean_WB_M,
		note_clean_WB_L,

		note_clean_W_S,	//ϴ����
		note_clean_W_M,
		note_clean_W_L,

		note_clean_B_S,	//������
		note_clean_B_M,
		note_clean_B_L,
#endif // CAN_CHOOSE_WORK_TIME

		key_count
	};
};

extern CDataMgr*	g_pData;
#define	IsChinese()	 (g_pData->get(eDataKey::set_Language) == 0)

void dataInit();
int dataNextOfLimit(int key, bool bIncrease, int _min, int _max, bool bLoop = true);
int dataNextFromArray(int key, bool bIncrease, const int* arr, int size);
int dataGet_NextSetting(int key, bool bIncrease = true);
void dataSet_AddCent(int nCent, bool bFromNet);	//������Ƿ֣�1�� = 100�֣�
bool dataSet_ClearCent();
int dataGet_BuyCent(int pid);
int dataGet_RemainCent();
void dataSet_AddCentByGame(int nCoin);
void dataUse_OutTick(int pid, int nTick);
void dataUse_OutEgg(int pid, int nEgg);
int dataGet_RemainFireScore();

bool& dataIs_CoinChange();

int dataGet_GameCent(int index = -1);
int dataGet_BonusScore();
int dataGet_CanWinCent();	//����10��
void dataUse_PlayGame();
void dataUse_WinCent(int nCoin);

// �ں����������
int dataGet_OnSale(int boxIndex);
void dataSet_OnSale(int boxIndex, int count, bool);
int dataSet_DecreaseContain(int boxIndex, bool bRound = false);	//���ý�����ڵ�ǰ����ʱ bRound = true
void dataSet_RecordOpenDoor(int v, int h);
int dataGet_CountOnSale();