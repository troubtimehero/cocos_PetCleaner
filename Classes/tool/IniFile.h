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
// 	unsigned long GetProfileSectionNames(CStringArray& strArray); // 返回section数量  
	int GetProfileInt(const char* lpszSectionName, const char* lpszKeyName, int def = 0);    
	std::string GetProfileString(const char* lpszSectionName, const char* lpszKeyName, const char* def = "");    
	BOOL DeleteSection(const char* lpszSectionName);
	BOOL DeleteKey(const char* lpszSectionName, const char* lpszKeyName);
private:    
	std::string  m_szFileName; // .//Config.ini, 如果该文件不存在，则exe第一次试图Write时将创建该文件  
	unsigned int m_unMaxSection; // 最多支持的section数(256)  
	unsigned int m_unSectionNameMaxSize; // section名称长度，这里设为32(Null-terminated)  
	void Init();    
};    
#endif  
