#include "FramesAnimation.h"

CFramesAnimation::CFramesAnimation()
{
	m_iCurFrame = m_iFrameCount = 0;
	m_bPlaying = false;
}

CFramesAnimation* CFramesAnimation::createWithFile( const char* file, int lines, int rows, int frames, float fps )
{
	CFramesAnimation *pAnimation = CFramesAnimation::create();
	
	if ( pAnimation )
	{
		pAnimation->initWithFile( file, lines, rows, frames, fps );
	}
	
	return pAnimation;
}

void CFramesAnimation::onEnterTransitionDidFinish()
{
	Sprite::onEnterTransitionDidFinish();

	scheduleUpdate();
}


bool CFramesAnimation::initWithFile(const char *file, int lines, int rows, int frames, float fps )
{
 	m_vRects.clear();

	Texture2D *texture = TextureCache::getInstance()->addImage(file);

	if ( texture == 0 )
	{
		MessageBox( "图片路径有错", file );
		return false;
	}

	if ( frames == 0 )
	{
		frames = lines*rows;
	}

	float framew = texture->getPixelsWide()/rows;
	float frameh = texture->getPixelsHigh()/lines;

	int picGirds = lines*rows;	//图片的总的格子数
	for ( int i=0; i<frames; ++i )
	{
		int spareGirds = i%picGirds;
		int line = spareGirds/rows;
		int row = spareGirds%rows;

		m_vRects.push_back( Rect(row*framew, line*frameh, framew, frameh) );	
	}
	

	m_iCurFrame = 0;
 	m_iFrameCount = m_vRects.size();
	m_fSinceLastFrame=-1.0f;
	m_fSpeed=1.0f/fps;
	m_bLoop = true;

	setTexture(texture);
	SetFrame( 0 );

	return true;
}

void CFramesAnimation::Play()
{
	m_bPlaying = true;
	m_iCurFrame = 0;	
	m_fSinceLastFrame = 0;
	SetFrame(0);
}

void CFramesAnimation::Stop()
{
	m_bPlaying = false;
}

void CFramesAnimation::update(float delta)
{
	if ( !m_bPlaying )
	{
		return;
	}

	m_fSinceLastFrame += delta;

	UINT curframe = m_iCurFrame;

	while(m_fSinceLastFrame >= m_fSpeed)
	{
		m_fSinceLastFrame -= m_fSpeed;

		curframe ++;

		if( curframe >= m_iFrameCount)
		{
			if ( !m_bLoop )
			{
				m_bPlaying = false;
				break;
			}			
			else
			{
				curframe = 0;
			}
		}		

		SetFrame( curframe );
	}
}

void CFramesAnimation::SetFrame( UINT frame )
{	
	if ( frame >= m_iFrameCount )
	{
		return;
	}

	if ( frame==m_iCurFrame && frame!=0 )
	{
		return;
	}

	m_iCurFrame = frame;

 	setTextureRect( m_vRects[m_iCurFrame] );
}

void CFramesAnimation::Show( bool bPlay )
{
	setVisible( true );
	if ( !m_bPlaying && bPlay )
	{
		Play();
	}
}

void CFramesAnimation::Hide()
{
	setVisible( false );
	if ( m_bPlaying )
	{
		Stop();
	}
}