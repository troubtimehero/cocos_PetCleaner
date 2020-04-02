#include "GameData.h"
#include "tool/MyTools.h"

bool s_bCoinIsUpdate = true;

void updateDebugText()
{
	DebugInfoLayer::updateString(StringUtils::format("gameBonus:%d", g_pData->get(eDataKey::tmp_GameBonus)));
}

void dataInit()
{
#ifdef CAN_CHOOSE_WORK_TIME
	//当设置小于最小值时，把它设为出厂值
	const int dataOfMin[][3] = {
		{ eDataKey::set_PetWash_BaseTime, DEF_SET_PET_CLEAN_BASE_MIN, DEF_SET_PET_CLEAN_BASE_MAX },		//分钟 1
		{ eDataKey::set_PetWash_BasePrice, 1, 100 },	//角 0.01
		{ eDataKey::set_PetWash_IncPrice, 1, 20 },		//角 0.01
		{ eDataKey::set_PetDry_BaseTime, DEF_SET_PET_DRY_BASE_MIN, DEF_SET_PET_DRY_BASE_MAX },
		{ eDataKey::set_PetDry_BasePrice, 1, 200 },
		{ eDataKey::set_PetDry_IncPrice, 1, 10 }
	};
	bool bFactory = false;
	for (int i = 0; i<ARRAYSIZE(dataOfMin); ++i)
	{
		if (g_pData->get(dataOfMin[i][0]) < dataOfMin[i][1])
		{
			g_pData->set(dataOfMin[i][0], dataOfMin[i][2]);
			bFactory = true;	//只要有修改成出厂值
		}
	}
	if(bFactory)	//就把最大加时设置为最大
	{
		g_pData->set(eDataKey::set_PetWash_IncTime, 10);
		g_pData->set(eDataKey::set_PetDry_IncTime, 60);
	}
#else
	for (int i = eDataKey::set_PetTimeWash_S; i <= eDataKey::set_PetPriceAll_L; ++i)
	{
		if (g_pData->get(i) <= 0)
			g_pData->set(i, 1);
	}
#endif // CAN_CHOOSE_WORK_TIME


#ifdef _DEBUG
#ifdef LYY_USE_BOXWAY_FOR_PET
	for (int i = 0; i < 60; ++i)
	{
		g_pData->set(i + eDataKey::rec_clean_01, i + 1);
	}
#endif // LYY_USE_BOXWAY_FOR_PET
#endif // _DEBUG

	for (int i = eDataKey::set_BuyCent_01; i < eDataKey::set_BuyCent_01 + COUNT_PRICE; ++i)
	{
		if (g_pData->get(i) <= 0)
			g_pData->set(i, 1);
	}

// 	if (g_pData->get(eDataKey::set_GameCent) <= 0)
// 		g_pData->set(eDataKey::set_GameCent, 1);

	if (g_pData->get(eDataKey::set_Volume) <= 0)
		g_pData->set(eDataKey::set_Volume, 100);

	bool bEnableSale = MyIniGetInt("SETTING", "ENABLE_SALE", 0);
	if (!bEnableSale)
		g_pData->set(eDataKey::set_saleMode, 0);
}

int dataNextOfLimit(int key, bool bIncrease, int _min, int _max, bool bLoop)
{
	int data = g_pData->get(key);
	if (bIncrease)
	{
		if (++data > _max)
			data = bLoop ? _min : _max;
	}
	else
	{
		if (--data < _min)
			data = bLoop ? _max : _min;
	}
	g_pData->set(key, data);

	return data;
}

int dataNextFromArray(int key, bool bIncrease, const int* arr, int size)
{
	int data = g_pData->get(key);
	if (bIncrease)
	{
		if (data >= arr[size - 1])
		{
			return g_pData->set(key, arr[0]);
		}
		else
		{
			for (int i = 0; i < size-1; ++i)
			{
				if (data >= arr[i] && data < arr[i+1])
				{
					return g_pData->set(key, arr[(i + 1) % size]);
				}
			}
			return g_pData->set(key, arr[0]);
		}
	}
	else
	{
		if (data <= arr[0])
		{
			return g_pData->set(key, arr[size - 1]);
		}
		else
		{
			for (int i = size - 1; i > 0; --i)
			{
				if (data <= arr[i] && data > arr[i-1])
				{
					return g_pData->set(key, arr[(i - 1 + size) % size]);
				}
			}
			return g_pData->set(key, arr[size - 1]);
		}
	}
}

int dataGet_NextSetting(int key, bool bIncrease)
{
	switch (key)
	{
	case eDataKey::set_Language:
		return dataNextOfLimit(key, bIncrease, 0, 1);

	case eDataKey::set_saleMode:
		return dataNextOfLimit(key, bIncrease, 0, 1);

	case eDataKey::set_GameCent:
		return dataNextFromArray(key, bIncrease, arrGameCentSetting, ARRAYSIZE(arrGameCentSetting));
	}

	return -1;
}

void dataSet_AddCent(int nCent, bool bFromNet)
{
	g_pData->increase(eDataKey::tmp_RemainCent, nCent);

	if (bFromNet)
	{
		g_pData->increase(eDataKey::tmp_RemainOnlineCent, nCent);
		g_pData->increase(eDataKey::rec_CentFromNet, nCent);
		g_pData->increase(eDataKey::rec_LastCentFromNet, nCent);
	}
	else
	{
		g_pData->increase(eDataKey::tmp_RemainOfflineCent, nCent);
		g_pData->increase(eDataKey::rec_CentIn, nCent);
		g_pData->increase(eDataKey::rec_LastCentIn, nCent);
	}
	s_bCoinIsUpdate = true;
}

bool dataSet_ClearCent()
{
	//剩余压分清零
// 	if (g_pData->get(eDataKey::tmp_RemainFireScore) > 0)
// 	{
// 		g_pData->set(eDataKey::tmp_RemainFireScore, 0);
// 	}
	//剩余币数清零
	if (g_pData->get(eDataKey::tmp_RemainCent) > 0)
	{
		g_pData->set(eDataKey::tmp_RemainCent, 0);
		return true;
	}
	s_bCoinIsUpdate = true;
	return false;
}

void dataUse_OutTick(int pid, int nTick)
{
	//扣奖金池
	g_pData->decrease(eDataKey::tmp_GameBonus, nTick);
	//出扭蛋记录
	g_pData->increase(eDataKey::rec_TickOut+ pid, nTick);
	g_pData->increase(eDataKey::rec_LastTickOut + pid, nTick);
	updateDebugText();
}

void dataUse_OutEgg(int pid, int nEgg)
{
	//扣币
	int nBuyCent = nEgg * dataGet_BuyCent(pid);
	g_pData->decrease(eDataKey::tmp_RemainCent, nBuyCent);

	g_pData->increase(eDataKey::rec_CentFromGoods, nBuyCent);
	g_pData->increase(eDataKey::rec_LastCentFromGoods, nBuyCent);

	//出扭蛋记录
	g_pData->increase(eDataKey::rec_EggOut_01 + pid, nEgg);
	g_pData->increase(eDataKey::rec_LastEggOut_01 + pid, nEgg);

	g_pData->increase(eDataKey::rec_giftOut, nEgg);	//记录总数，用与上传到网络

	s_bCoinIsUpdate = true;
}

int dataGet_BuyCent(int pid)
{
	return g_pData->get(eDataKey::set_BuyCent_01 + pid);
}

int dataGet_RemainCent()
{
	return g_pData->get(eDataKey::tmp_RemainCent);
}

void dataSet_AddCentByGame(int nCoin)
{
	g_pData->increase(eDataKey::tmp_RemainCent, nCoin);
	s_bCoinIsUpdate = true;
}


bool& dataIs_CoinChange()
{
	return s_bCoinIsUpdate;
}

int dataGet_GameCent(int index)
{
	return g_pData->get(eDataKey::set_GameCent);
}

int dataGet_BonusScore()
{
	return g_pData->get(eDataKey::tmp_GameBonus);
}

static const int BONUS_PER_COIN_WIN = 10;	//每币分数(中奖)
static const int BONUS_PER_COIN_PLAY = 50;	//每币分数(玩游戏)
int dataGet_CanWinCent()
{
	return g_pData->get(eDataKey::tmp_GameBonus) / BONUS_PER_COIN_WIN;
}

void dataUse_PlayGame()
{
	int gameCent = g_pData->get(eDataKey::set_GameCent);
	g_pData->decrease(eDataKey::tmp_RemainCent, gameCent);

	g_pData->increase(eDataKey::rec_CentFromGoods, gameCent);
	g_pData->increase(eDataKey::rec_LastCentFromGoods, gameCent);

	g_pData->increase(eDataKey::tmp_GameBonus, gameCent * (gameCent >= 10 ? 0.9f : 1));
	s_bCoinIsUpdate = true;
	updateDebugText();
}

void dataUse_WinCent(int nCoin)
{
	g_pData->decrease(eDataKey::tmp_GameBonus, nCoin * BONUS_PER_COIN_WIN);
	updateDebugText();
}


//////////////////////////////////// 口红机、福袋机 //////////////////////////////////////

int dataGet_OnSale(int boxIndex)
{
	return g_pData->get(eDataKey::status_remainGoods_01 + boxIndex);
}

void dataSet_OnSale(int boxIndex, int count)
{
	if (g_pData->get(eDataKey::status_remainGoods_01 + boxIndex) != count)
		g_pData->set(eDataKey::status_remainGoods_01 + boxIndex, count);
}

int dataSet_DecreaseContain(int boxIndex, bool bRound)
{
	int cur = g_pData->get(eDataKey::status_remainGoods_01 + boxIndex);
	int next = cur - 1;
	if (next < 0)
	{
		next = bRound ? BOX_CONTAIN_GOODS : 0;
	}

	if (next != cur)
	{
		g_pData->set(eDataKey::status_remainGoods_01 + boxIndex, next);
	}
	return next;
}

void dataSet_RecordOpenDoor(int v, int h)
{
	//先把记录挪位，最新的记在第一个
	for (int i = 4; i >= 0; --i)
	{
		int idx = eDataKey::list_open1_pos + i * 2;
		g_pData->set(idx + 2, g_pData->get(idx));
		g_pData->set(idx + 3, g_pData->get(idx + 1));
	}

	time_t t = time(0);
	g_pData->set(eDataKey::list_open1_pos, v * 10 + h);
	g_pData->set(eDataKey::list_open1_pos+1, t);
}

int dataGet_CountOnSale()
{
	int n = 0;
	for (int i = 0; i < COUNT_BOX; ++i)
	{
		if (dataGet_OnSale(i) > 0)
			n++;
	}
	return n;
}
