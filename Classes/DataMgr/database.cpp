// DataMgr.cpp : 定义 DLL 应用程序的导出函数。
//

#include "database.h"
#include <string>
using namespace std;


CSQLiteSave::CSQLiteSave(const char* tableName)
{
	m_pDB = nullptr;
	m_errMsg = nullptr;
	m_tableName = tableName;
}


CSQLiteSave::~CSQLiteSave(void)
{
	//关闭数据库   
	sqlite3_close(m_pDB);   
}

int CSQLiteSave::init()
{
	string sqlstr;//SQL指令   
	int result;//sqlite3_exec返回值   

	//打开一个数据库，如果该数据库不存在，则创建一个数据库文件   
	result = sqlite3_open(m_tableName, &m_pDB);
	if( result != SQLITE_OK )
		return result;

	result = create();
	if( result != SQLITE_OK )   
		return result;

	return SQLITE_OK;
}

//callback
int callbackIsExisted( void * para, int n_column, char ** column_value, char ** column_name ) 
{ 
	bool *isExisted_=(bool*)para; 
	*isExisted_=(**column_value)!='0'; 
	return 0; 
} 

int CSQLiteSave::create()
{
	bool bExisted;
	char szSql[128] = {0};
	sprintf(szSql, "select count(type) from sqlite_master where type='table' and name='%s'", m_tableName);
	sqlite3_exec( m_pDB, szSql, callbackIsExisted, &bExisted, &m_errMsg );

	if(bExisted)
		return SQLITE_OK;

	int result;
	//创建表，设置ID为主键，且自动增加
	sprintf(szSql, "create table %s ( ID integer primary key autoincrement, KEY integer, DATA integer ) ", m_tableName);
	result=sqlite3_exec( m_pDB, szSql, NULL, NULL, &m_errMsg );

	return result;
}

int CSQLiteSave::insert( int key, int data )
{
	bool isExisted_; 
	char szSql[128] = {0};
	sprintf(szSql, "select count(*) from %s where KEY = %d", m_tableName, key);
	sqlite3_exec( m_pDB, szSql, callbackIsExisted, &isExisted_, &m_errMsg );
	if(isExisted_)
		return SQLITE_OK;

	//插入数据   
	sprintf(szSql, "insert into %s ( KEY, DATA ) values ( %d, %d ) ", m_tableName, key, data);
	int result = sqlite3_exec( m_pDB, szSql , NULL, NULL, &m_errMsg );
	return result;
}

int loadRecord( void * para, int n_column, char ** column_value, char ** column_name ) 
{
	sscanf(column_value[0], "%d", (int*)para);
	return 0;
}
int CSQLiteSave::get( int key, int& data )
{
	char szSql[128] = {0};
	sprintf(szSql, "select DATA from %s where KEY=%d", m_tableName, key);
	int result = sqlite3_exec( m_pDB, szSql, loadRecord, &data, &m_errMsg );
	return result;
}

int CSQLiteSave::set( int key, int data )
{
	char szSql[128] = {0};
	sprintf(szSql, "update %s set DATA=%d where KEY=%d", m_tableName, data, key);
	int result = sqlite3_exec( m_pDB, szSql, nullptr, nullptr, &m_errMsg );
	return result;
}
