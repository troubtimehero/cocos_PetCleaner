/************************************************************************/
/* 
���࣬��ͬ��ϷҪ��Ӧ��ͬ�����ݣ����Բ��ܹ���
�����ӿ���һ���ģ�ֻҪ֪��KEY��KEY�ĸ�������
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
	int set(int key, int data);	//������Ҳ���ظ�ֵ
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
