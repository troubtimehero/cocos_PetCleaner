#pragma once
#include "D:/Pro_share/DataMgr/sqlite/sqlite3.h"

class CSQLiteSave
{
public:
	CSQLiteSave(const char* tableName);
	~CSQLiteSave(void);

	int init();
	int insert(int key, int data);
	int get(int key, int& data);
	int set(int key, int data);
	const char* getErrorMsg(){return m_errMsg;}

private:
	int create();
	sqlite3 *m_pDB;//���ݿ�ָ��   
	char * m_errMsg;//������Ϣ   
	const char*	m_tableName;
};

