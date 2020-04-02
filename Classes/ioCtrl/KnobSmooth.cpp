#include "KnobSmooth.h"
#include "IoCtrlLayer.h"
#include "ComThread.h"

KnobSmooth::KnobSmooth()
{
	m_index = 0;
	c_smooth = 1;
}

KnobSmooth::~KnobSmooth()
{
}

KnobSmooth::eKey KnobSmooth::getKey()
{
	//ÐýÅ¥¿ØÖÆ
#ifdef USE_KNOB
	if (g_pIoCtrlLayer->isKeyDown(IoCtrlLayer::e_key_left))
	{
		if (--m_index < 0)
		{
			m_index = c_smooth - 1;
			return e_key_l;
		}
	}
	else if (g_pIoCtrlLayer->isKeyDown(IoCtrlLayer::e_key_right))
	{
		if (++m_index >= c_smooth)
		{
			m_index = 0;
			return e_key_r;
		}
	}
#endif // USE_KNOB

	return e_key_null;
}

int KnobSmooth::getKeyCount(eKey key)
{
#ifdef USE_KNOB
	switch (key)
	{
	case KnobSmooth::e_key_l:
		if (g_pCom->GetKnob(CComThread::e_knob_l) > 0)
			return g_pCom->GetKnob(CComThread::e_knob_l);
		else if (g_pIoCtrlLayer->isKeyDown(IoCtrlLayer::e_key_left))
			return 1;
		return 0;
	case KnobSmooth::e_key_r:
		if (g_pCom->GetKnob(CComThread::e_knob_r) > 0)
			return g_pCom->GetKnob(CComThread::e_knob_r);
		else if (g_pIoCtrlLayer->isKeyDown(IoCtrlLayer::e_key_right))
			return 1;
		return 0;
	}
#endif // USE_KNOB
	return 0;
}
