#pragma once
#include <cocos2d.h>
USING_NS_CC;
#include "GlobalDefined.h"
#include "KnobSmooth.h"

class KnobLayerBase : public LayerColor
{
public:
	KnobLayerBase(int itemCount, const bool* pEnables);
	~KnobLayerBase();

	virtual void onEnter();
	virtual void update(float delta);
	virtual void updateItemIndex(int iDirection);
protected:
	static const int	c_maxItemCount = 128;
	KnobSmooth	m_knob;
	int			c_nItemCount;
	Node*		m_pItems[c_maxItemCount];
	bool		m_pIsEnables[c_maxItemCount];
	int			m_iCurIndex;
};

