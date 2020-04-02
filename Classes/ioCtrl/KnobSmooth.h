#pragma once
#ifndef __KNOB_SMOOTH_H__
#define __KNOB_SMOOTH_H__

class KnobSmooth
{
public:
	KnobSmooth();
	~KnobSmooth();

	enum eKey
	{
		e_key_null,
		e_key_l,
		e_key_r
	};
	void setSmooth(int smooth){ c_smooth = smooth; } //´óÓÚ0
	eKey getKey();
	int getKeyCount(eKey key);

private:
	int		c_smooth;
	int		m_index;
};

#endif //__KNOB_SMOOTH_H__