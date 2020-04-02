#include "GameMusic.h"
#include "tool/MyTools.h"


CGameMusic::CGameMusic()
{
	m_pRcMusicIndex = nullptr;
}

CGameMusic::~CGameMusic()
{
	delete m_pRcMusicIndex;
}

void CGameMusic::init()
{
	m_lstFile.clear();
// 	m_lstFile.resize(10);
	ScanFile(".\\music", m_lstFile);

	if (m_lstFile.empty())
	{
		MessageBox("�Ҳ�����Ч�������ļ�", "��ʾ");
	}

	int* idx = new int[m_lstFile.size()];
	for (int i = 0; i<m_lstFile.size(); ++i)
	{
		idx[i] = i;
	}
	m_pRcMusicIndex = new RandomController<int, 1>(idx, m_lstFile.size());
	delete[] idx;
}

void CGameMusic::play()
{
	MySound::playMusic(m_lstFile.at(m_pRcMusicIndex->GetRandom()).c_str());
}

bool CGameMusic::isPlaying()
{
	return MySound::isMscPlaying();
}

void CGameMusic::stop()
{
	MySound::stopMusic();
}