#ifndef __LAYERLANGUAGE_H__
#define __LAYERLANGUAGE_H__

class LayerLanguage
{
public:
	LayerLanguage(){}
	~LayerLanguage(){}
	static void SetLanguage(bool bEn);
	virtual void SwitchLanguage();
private:
	static bool		m_bEn;
};
#endif /* __LAYERLANGUAGE_H__ */
