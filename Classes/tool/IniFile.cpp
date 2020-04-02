#include "IniFile.h"  

void CIniFile::Init()    
{    
	m_unMaxSection = 512;    
	m_unSectionNameMaxSize = 33; // 32位UID串  
}    
CIniFile::CIniFile()    
{    
	Init();    
}    
CIniFile::CIniFile(const char* szFileName)    
{    
	// (1) 绝对路径，需检验路径是否存在  
	// (2) 以"./"开头，则需检验后续路径是否存在  
	// (3) 以"../"开头，则涉及相对路径的解析  
	Init();    
	// 相对路径  
	m_szFileName = StringUtils::format(".//%s", szFileName);    
}    
CIniFile::~CIniFile()      
{    
}    
void CIniFile::SetFileName(const char* szFileName)    
{    
	m_szFileName = StringUtils::format(".//%s", szFileName);    
}    
// DWORD CIniFile::GetProfileSectionNames(CStringArray &strArray)    
// {    
// 	int nAllSectionNamesMaxSize = m_unMaxSection*m_unSectionNameMaxSize+1;    
// 	char *pszSectionNames = new char[nAllSectionNamesMaxSize];    
// 	DWORD dwCopied = 0;    
// 	dwCopied = ::GetPrivateProfileSectionNames(pszSectionNames, nAllSectionNamesMaxSize, m_szFileName);    
// 	strArray.RemoveAll();    
// 	char *pSection = pszSectionNames;    
// 	do
// 	{    
// 		CString szSection(pSection);    
// 		if (szSection.GetLength() < 1)    
// 		{    
// 			delete[] pszSectionNames;    
// 			return dwCopied;    
// 		}    
// 		strArray.Add(szSection);    
// 		pSection = pSection + szSection.GetLength() + 1; // next section name  
// 	} while (pSection && pSection<pszSectionNames+nAllSectionNamesMaxSize);    
// 	delete[] pszSectionNames;    
// 	return dwCopied;    
// }    
std::string CIniFile::GetProfileString(const char* lpszSectionName, const char* lpszKeyName, const char* def)
{    
	unsigned long dwCopied = 0;    
	char buf[MAX_PATH] = { 0 };
	dwCopied = ::GetPrivateProfileString(lpszSectionName, lpszKeyName, def,
		buf, MAX_PATH, m_szFileName.c_str());    
	string str = buf;
	return str;    
}    
int CIniFile::GetProfileInt(const char* lpszSectionName, const char* lpszKeyName, int def)    
{    
	int nKeyValue = ::GetPrivateProfileInt(lpszSectionName, lpszKeyName, 0, m_szFileName.c_str());    
	return nKeyValue;    
}    
BOOL CIniFile::SetProfileString(const char* lpszSectionName, const char* lpszKeyName, const char* lpszKeyValue)
{    
	return ::WritePrivateProfileString(lpszSectionName, lpszKeyName, lpszKeyValue, m_szFileName.c_str());    
}    
BOOL CIniFile::SetProfileInt(const char* lpszSectionName, const char* lpszKeyName, int nKeyValue)
{    
	string szKeyValue;    
	szKeyValue = StringUtils::format("%d", nKeyValue);    
	return ::WritePrivateProfileString(lpszSectionName, lpszKeyName, szKeyValue.c_str(), m_szFileName.c_str());    
}    
BOOL CIniFile::DeleteSection(const char* lpszSectionName)
{    
	return ::WritePrivateProfileSection(lpszSectionName, NULL, m_szFileName.c_str());    
}    
BOOL CIniFile::DeleteKey(const char* lpszSectionName, const char* lpszKeyName)
{    
	return ::WritePrivateProfileString(lpszSectionName, lpszKeyName, NULL, m_szFileName.c_str());    
}    