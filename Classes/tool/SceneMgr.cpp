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
	//获取屏幕尺寸大小
	Size winSize = CCDirector::sharedDirector()->getWinSize();

	//创建RenderTexture，纹理图片大小为窗口大小winSize
	RenderTexture* screen = RenderTexture::create(winSize.width, winSize.height);

	//屏幕截图
	screen->begin();            //开始抓屏
	this->visit();				//遍历当前场景Scene的所有子节点信息，画入screen中
	screen->end();              //结束抓屏

	//保存截图
// 	screen->saveToFile("ScreenShot.png", Image::Format::PNG); //保存为PNG格式
	screen->saveToFile("ScreenShot.jpg", Image::Format::JPG); //保存为JPG格式
}

void SceneMgr::captureScreen2()
{
	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "snapshot\\ScreenShot %Y-%m-%d_%H-%M-%S.png", localtime(&t));

	utils::captureScreen(CC_CALLBACK_2(SceneMgr::afterCapture, this), tmp);
}

//截图后执行afterCapture
void SceneMgr::afterCapture(bool succeed, const std::string& outputFile)
{
	if (succeed)
	{
		//显示截图
		Sprite* sp = Sprite::create(outputFile);
		sp->setPosition(Point::ZERO);
		sp->setAnchorPoint(Point::ZERO);
		this->addChild(sp);
		sp->runAction(Sequence::createWithTwoActions(ScaleTo::create(1, 0), RemoveSelf::create()));
	}
}