#define DLLCLASS_EXPORT
#include "DataMgr.h"
#include "D:/Pro_share/libs/database.h"
#pragma comment(lib, "D:/Pro_share/libs/database.lib")
#include <cocos2d.h>

static CSQLiteSave* pDB = nullptr;
CDataMgr::CDataMgr(const char* name, int cKeyCount)
{
	m_keyCount = cKeyCount;
	pDB = new CSQLiteSave(name);
	m_data = new int[m_keyCount];

	for (int i = 0; i < m_keyCount; ++i)
	{
		m_data[i] = -1;
	}
}


CDataMgr::~CDataMgr()
{
	if(pDB)
	{
		delete pDB;
		pDB = nullptr;
	}
	if(m_data)
	{
		delete[] m_data;
	}
}

void CDataMgr::init()
{
	int res;
	pDB->init();
	for (int i = 0; i < m_keyCount; ++i)
	{
		res = pDB->insert(i, 0);	//若本身存在，则不会被修改
		if(m_bEnableLog && SQLITE_OK != res)
		{
			log(pDB->getErrorMsg());
		}
	}

	for (int i = 0; i < m_keyCount; ++i)
	{
		res = pDB->get(i, m_data[i]);
		if(m_bEnableLog && SQLITE_OK != res)
		{
			log(pDB->getErrorMsg());
		}
	}
}

int CDataMgr::set(int key, int data)
{
	m_data[key] = data;
	int res = pDB->set(key, m_data[key]);
	if(m_bEnableLog && SQLITE_OK != res)
	{
		log(pDB->getErrorMsg());
	}
	return data;
}

void CDataMgr::increase(int key, unsigned int _add)
{
	m_data[key] += _add;
	int res = pDB->set(key, m_data[key]);
	if(m_bEnableLog && SQLITE_OK != res)
	{
		log(pDB->getErrorMsg());
	}
}

void CDataMgr::decrease(int key, unsigned int _dec)
{
	m_data[key] -= _dec;
	if (m_data[key] < 0)
		m_data[key] = 0;
	int res = pDB->set(key, m_data[key]);
	if(m_bEnableLog && SQLITE_OK != res)
	{
		log(pDB->getErrorMsg());
	}
}

std::string CDataMgr::getstr(int key)
{
	char buf[32] = { 0 };
	return _itoa(get(key), buf, 10);
}

void CDataMgr::enableLog( bool bLog )
{
	m_bEnableLog = bLog;
}

void CDataMgr::log( const char* txt )
{
	CCLOG("database error");
}
