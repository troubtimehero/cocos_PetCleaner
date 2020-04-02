#include "KnobLayerBase.h"

KnobLayerBase::KnobLayerBase(int itemCount, const bool* bEnables) : c_nItemCount(itemCount)
{
	m_knob.setSmooth(2);
#ifdef USE_KNOB
	for (int i = 0; i < c_nItemCount; ++i)
	{
		m_pIsEnables[i] = bEnables[i];
	}
#endif // USE_KNOB
	m_iCurIndex = 0;
}


KnobLayerBase::~KnobLayerBase()
{
}

void KnobLayerBase::onEnter()
{
	LayerColor::onEnter();
#ifdef USE_KNOB
	scheduleUpdate();
	updateItemIndex(0);
#endif // USE_KNOB
}

void KnobLayerBase::update(float delta)
{
	switch (m_knob.getKey())
	{
	case KnobSmooth::e_key_l:
		updateItemIndex(-1);
		break;

	case KnobSmooth::e_key_r:
		updateItemIndex(1);
		break;
	}
}

void KnobLayerBase::updateItemIndex(int iDirection)
{
#ifdef USE_KNOB
	m_pItems[m_iCurIndex]->setColor(Color3B::WHITE);
	m_pItems[m_iCurIndex]->setScale(1);

	do 
	{
		if (iDirection > 0)
			++m_iCurIndex %= c_nItemCount;
		else if (iDirection < 0)
			m_iCurIndex = (m_iCurIndex + c_nItemCount - 1) % c_nItemCount;
	} while (!m_pIsEnables[m_iCurIndex]);

	m_pItems[m_iCurIndex]->setColor(Color3B::YELLOW);
	m_pItems[m_iCurIndex]->setScale(1.15f);
#endif // USE_KNOB
}
