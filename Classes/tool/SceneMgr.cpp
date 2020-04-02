#include "SceneMgr.h"
#include "MachineDefined.h"

USING_NS_CC;

#include "tool/MyTools.h"
#include "ioCtrl/IoCtrlLayer.h"

void SceneMgr::onEnter()
{
	Layer::onEnter();
	if (g_pIoCtrlLayer->getParent())
		g_pIoCtrlLayer->removeFromParent();
	this->addChild(g_pIoCtrlLayer, 10001);
	g_pIoCtrlLayer->setVisible(true);

#if (ROTATE_SCREEN == 1)
	this->setRotation(90);
	this->setAnchorPoint(Point::ZERO);
	this->setPosition(0, VisibleRect::size().width);
#endif // HORIZONTAL_SCREEN
}

void SceneMgr::onExit()
{
	Layer::onExit();
}

void SceneMgr::captureScreen()
{
	//��ȡ��Ļ�ߴ��С
	Size winSize = CCDirector::sharedDirector()->getWinSize();

	//����RenderTexture������ͼƬ��СΪ���ڴ�СwinSize
	RenderTexture* screen = RenderTexture::create(winSize.width, winSize.height);

	//��Ļ��ͼ
	screen->begin();            //��ʼץ��
	this->visit();				//������ǰ����Scene�������ӽڵ���Ϣ������screen��
	screen->end();              //����ץ��

	//�����ͼ
// 	screen->saveToFile("ScreenShot.png", Image::Format::PNG); //����ΪPNG��ʽ
	screen->saveToFile("ScreenShot.jpg", Image::Format::JPG); //����ΪJPG��ʽ
}

void SceneMgr::captureScreen2()
{
	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "snapshot\\ScreenShot %Y-%m-%d_%H-%M-%S.png", localtime(&t));

	utils::captureScreen(CC_CALLBACK_2(SceneMgr::afterCapture, this), tmp);
}

//��ͼ��ִ��afterCapture
void SceneMgr::afterCapture(bool succeed, const std::string& outputFile)
{
	if (succeed)
	{
		//��ʾ��ͼ
		Sprite* sp = Sprite::create(outputFile);
		sp->setPosition(Point::ZERO);
		sp->setAnchorPoint(Point::ZERO);
		this->addChild(sp);
		sp->runAction(Sequence::createWithTwoActions(ScaleTo::create(1, 0), RemoveSelf::create()));
	}
}