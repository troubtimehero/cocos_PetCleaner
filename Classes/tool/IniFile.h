// IniFile.h  
#ifndef __INIFILE_H__  
#define __INIFILE_H__  
#include <cocos2d.h>
#include "tool/MyTools.h"

class CIniFile    
{    
public:    
	CIniFile();    
	CIniFile(const char* szFileName);    
	virtual ~CIniFile();    
public:    
	// Attributes      
	void SetFileName(const char* szFileName);    
public:    
	// Operations  
	BOOL SetProfileInt(const char* lpszSectionName, const char* lpszKeyName, int nKeyValue);    
	BOOL SetProfileString(const char* lpszSectionName, const char* lpszKeyName, const char* lpszKeyValue);
// 	unsigned long GetProfileSectionNames(CStringArray& strArray); // ����section����  
	int GetProfileInt(const char* lpszSectionName, const char* lpszKeyName, int def = 0);    
	std::string GetProfileString(const char* lpszSectionName, const char* lpszKeyName, const char* def = "");    
	BOOL DeleteSection(const char* lpszSectionName);
	BOOL DeleteKey(const char* lpszSectionName, const char* lpszKeyName);
private:    
	std::string  m_szFileName; // .//Config.ini, ������ļ������ڣ���exe��һ����ͼWriteʱ���������ļ�  
	unsigned int m_unMaxSection; // ���֧�ֵ�section��(256)  
	unsigned int m_unSectionNameMaxSize; // section���Ƴ��ȣ�������Ϊ32(Null-terminated)  
	void Init();    
};    
#endif  
