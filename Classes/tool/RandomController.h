#pragma once

#include <string.h>

// 随机数控制器
#define size_of_array(arr)		sizeof(arr)/sizeof(arr[0])

template<typename T, int nArrayCount>
class RandomController
{
public:
	// data: 数据
	// num: 数目
	struct RandomDataInfo 
	{
		T	data;
		int	num;
	};
	// RandomDataInfo类型的数组来决定比例
	RandomController(const RandomDataInfo* arr, int size) : m_size(0)
	{
		M_ASSERT(arr && size > 0);

		int i, j, k;
		for(i=0; i<size; ++i)
		{
			m_size += arr[i].num;
		}
		for(i=0; i<nArrayCount; ++i)
		{
			m_array[i] = new T[m_size];
			T*	p = m_array[i];
			for(j=0; j<size; ++j)
			{
				for(k=0; k<arr[j].num; ++k)
				{
					*p++ = arr[j].data;
				}
			}
			p = NULL;
			m_rest[i] = m_size;
		}
	}

	//	arr: 可控随机数组，size: 数组大小
	RandomController(const T* arr, int size) : m_size(size)
	{
// 		M_ASSERT(arr && size > 0);
		//复制数组
		for(int i=0; i<nArrayCount; ++i)
		{
			m_array[i] = new T[size];
			memcpy_s(m_array[i], sizeof(T)* size, arr, sizeof(T)*size);
			m_rest[i] = size;
		}
	}
	virtual ~RandomController(void)
	{
		for(int i=0; i<nArrayCount; ++i)
		{
			if(m_array[i])
				delete m_array[i];
		}
	}
	// 获取数组中的随机数
	const T GetRandom(int idx = 0)
	{
		if(m_rest[idx] <= 0)
			m_rest[idx] = m_size;

		int index = rand() % m_rest[idx]-- ;

		if(index == m_rest[idx])
			return m_array[idx][index];

		T tmp = m_array[idx][index];
		m_array[idx][index] = m_array[idx][m_rest[idx]];
		m_array[idx][m_rest[idx]] = tmp;

		return tmp;
	}
	// 获取本轮剩余个数
	int GetCurResume(int idx = 0)
	{
		return m_rest[idx];
	}
	// 重置数组上限，此功能用于避免随机数重复
	void	Reset(int idx = -1)
	{
		if(idx >= 0)
			m_rest[idx] = m_size;
		else
		{
			for(int i=0; i<nArrayCount; ++i)
				m_rest[i] = m_size;
		}

	}

	int size(){return m_size;}
private:
	T*		m_array[nArrayCount];
	int	m_size;
	int	m_rest[nArrayCount];		//剩余数
};

