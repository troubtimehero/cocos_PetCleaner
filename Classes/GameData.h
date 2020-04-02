/************************************************************************/
/* 
此类，不同游戏要对应不同的数据，所以不能共用
但，接口是一样的，只要知道KEY和KEY的个数即可
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
		//洗狗
#ifdef CAN_CHOOSE_WORK_TIME
		set_PetWash_BaseTime,
		set_PetWash_IncTime,
		set_PetWash_BasePrice,	//都用 0.1元（角）
		set_PetWash_IncPrice,

		set_PetDry_BaseTime,
		set_PetDry_IncTime,
		set_PetDry_BasePrice,	//都用 0.1元
		set_PetDry_IncPrice,

#else
		set_PetTimeWash_S,	//洗时间，分钟（minute）
		set_PetTimeWash_M,	
		set_PetTimeWash_L,

		set_PetTimeBlow_S,	//吹时间，分钟
		set_PetTimeBlow_M,
		set_PetTimeBlow_L,

		set_PetPriceWash_S,	//单洗价格，分（cent）
		set_PetPriceWash_M,
		set_PetPriceWash_L,

		set_PetPricBlowe_S,	//单吹价格
		set_PetPricBlowe_M,
		set_PetPricBlowe_L,

		set_PetPriceAll_S,	//洗+吹价格
		set_PetPriceAll_M,
		set_PetPriceAll_L,
#endif // CAN_CHOOSE_WORK_TIME

		//////////////////////////////////////////////////////////////////////////


		set_Language,	//语言
		set_saleMode,		//售货模式
		set_GameCent,	//游戏币数	//0:免费，1-10
		set_Volume,		//音量
		set_BuyCent_01,	//直接购买需要币数	//0:关闭，1-99	【注意】个数要与 COUNT_PRICE 对应
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


		rec_CentIn,			//总收入币数
		rec_CentFromNet,	//云上分
		rec_CentFromGoods,	//购买货品的收益，总收益中的一部分
		rec_TickOut,		//总退票数
		rec_EggOut_01,		//总扭蛋数,用作出礼品数记录
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


		rec_LastCentIn,			//本期收入币数
		rec_LastCentFromNet,	//云上分
		rec_LastCentFromGoods,
		rec_LastTickOut,		//本期退票数
		rec_LastEggOut_01,		//本期扭蛋数,用作出礼品数记录
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

		tmp_RemainOfflineCent,	//线下投币(元)
		tmp_RemainOnlineCent,	//线上上分(分)
		tmp_RemainCent,		//剩余币数 = 线上+线下
		tmp_GameBonus,		//游戏奖金(币数 X 10)------------------- 也是用 cent
		tmp_OutError,	//未出礼品数
// 		tmp_RemainFireScore,	//剩余游戏压分


		status_remainGoods_01,	//福袋机，每格剩余货品数量
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


		list_open1_pos,		//出奖记录列表
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
		report_centIn,	// 与 rec_CoinIn 比较
		report_giftOut,	// 与 rec_outGift 比较


		//清洗记录
#ifdef CAN_CHOOSE_WORK_TIME
		//-------------- 工作次数
		rec_clean_01,	//洗烘，价格1，//工作模式为主,价格为辅，要与 LeYaoYao::petCleanRecName 对应
		rec_clean_02,	//洗烘，价格2，
		rec_clean_03,	//洗烘，价格3，
		rec_clean_04,	//洗烘，价格4，
		rec_clean_05,	//洗烘，价格5，

		rec_clean_06,	//洗，价格1
		rec_clean_07,
		rec_clean_08,
		rec_clean_09,
		rec_clean_10,
		
		rec_clean_11,	//烘
		rec_clean_12,
		rec_clean_13,
		rec_clean_14,
		rec_clean_15,
		//-------------- 各模式总收益
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


		rec_lastClean_01,//洗烘，价格1，
		rec_lastClean_02,//洗烘，价格2，
		rec_lastClean_03,//洗烘，价格3，
		rec_lastClean_04,//洗烘，价格4，
		rec_lastClean_05,//洗烘，价格5，

		rec_lastClean_06,//洗，价格1
		rec_lastClean_07,
		rec_lastClean_08,
		rec_lastClean_09,
		rec_lastClean_10,

		rec_lastClean_11,//烘
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
		note_clean_WB_S,	//洗+吹、长
		note_clean_WB_M,
		note_clean_WB_L,

		note_clean_W_S,	//洗、长
		note_clean_W_M,
		note_clean_W_L,

		note_clean_B_S,	//吹、长
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
void dataSet_AddCent(int nCent, bool bFromNet);	//传入的是分（1币 = 100分）
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
int dataGet_CanWinCent();	//币数10倍
void dataUse_PlayGame();
void dataUse_WinCent(int nCoin);

// 口红机、福袋机
int dataGet_OnSale(int boxIndex);
void dataSet_OnSale(int boxIndex, int count, bool);
int dataSet_DecreaseContain(int boxIndex, bool bRound = false);	//设置界面调节当前数量时 bRound = true
void dataSet_RecordOpenDoor(int v, int h);
int dataGet_CountOnSale();