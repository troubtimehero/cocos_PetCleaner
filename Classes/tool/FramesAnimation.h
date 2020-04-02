/*
* 序列帧动画
*/

#ifndef _FRAMESANIMATION_H_
#define _FRAMESANIMATION_H_

#include "cocos2d.h"
using namespace cocos2d;
#include <vector>
using namespace std;

/*
** CFramesAnimation class
*/
class CFramesAnimation : public Sprite
{
public:
	CFramesAnimation();

	CREATE_FUNC(CFramesAnimation);

	void		onEnterTransitionDidFinish();

	static CFramesAnimation* createWithFile( const char* file, int lines, int rows, int frames=0, float fps=4 );

	bool		initWithFile(const char* file, int lines, int rows, int frames=0, float fps=4 );
	void		Play();
	void		Stop();
	bool		IsPlaying() const { return m_bPlaying; }

	void		SetSpeed(float fps) { m_fSpeed=1.0f/fps; }
	float		GetSpeed() const { return 1.0f/m_fSpeed; }
	
	void		SetFrame( UINT frame );
	UINT		GetFrame() const { return m_iCurFrame; }
	
	UINT		GetFrames() const { return m_iFrameCount; }

	void		SetLoop( bool bLoop ) { m_bLoop = bLoop; }
	bool		GetLoop() const  { return m_bLoop; }

	void		Show( bool bPlay = true );
	void		Hide();

private:
	void		update(float delta);


private:
	vector<Rect>	m_vRects;
	UINT			m_iCurFrame;	//当前第几帧
	UINT			m_iFrameCount;	//总帧数
	bool			m_bPlaying;		//是否在播放
	float			m_fSpeed;
	float			m_fSinceLastFrame;
	bool			m_bLoop;
};

class CTouchTip : public CFramesAnimation
{
public:
	void Init()
	{
		init();
		CFramesAnimation::initWithFile( "image/touchtip.png", 2, 3, 6, 8 );
		Hide();
	}
	void Show( bool bPlay = true )
	{
		CFramesAnimation::Show( bPlay );
		this->runAction( Sequence::create( 
			DelayTime::create(3),
			CallFunc::create(this,callfunc_selector(CTouchTip::Hide)), 
			NULL) );
	}
	void Hide()
	{
		this->stopAllActions();
		CFramesAnimation::Hide();
	}
};

class CTouchShow : public CFramesAnimation
{
public:
	void Init()
	{
		init();
		CFramesAnimation::initWithFile( "image/tips/touchshow.png", 1, 5, 5, 10 );
		Hide();
	}
	void Show()
	{
		this->runAction( Sequence::create( 
			DelayTime::create(0.2f),
			Show::create(),			
			CallFunc::create(this,callfunc_selector(CFramesAnimation::Play)),
			NULL) );
	}
	void Hide()
	{
		this->stopAllActions();
		CFramesAnimation::Hide();
	}	
};

#endif