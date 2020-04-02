/************************************************************************/
/* 
此类，不同游戏要对应不同的数据，所以不能共用
但，接口是一样的，只要知道KEY和KEY的个数即可
*/
/************************************************************************/

#pragma once
#ifdef DLLCLASS_EXPORT
#define EXT_CLASS _declspec(dllexport)
#else
#define EXT_CLASS _declspec(dllimport)
#endif

#pragma once
#include <string>

class EXT_CLASS CDataMgr
{
public:
	CDataMgr(const char* name, int cKeyCount);
	~CDataMgr();

	void init();
	int get(int key){ return m_data[key]; }
	std::string getstr(int key);
	int set(int key, int data);	//设置完也返回该值
	void increase(int key, unsigned int _add);
	void decrease(int key, unsigned int _dec);

public:
	void enableLog(bool bLog);

private:
//	virtual	bool check(){}
	void log(const char* txt);
	int*	m_data;
	int		m_keyCount;
	bool	m_bEnableLog;
};
