#include "AppDelegate.h"
#include "GlobalDefined.h"
#include "helloScene/HelloWorldScene.h"
#include "GameData.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "verify/VerifyScene.h"
#include "tool/MyTools.h"

CDataMgr*	g_pData = nullptr;
IoCtrlLayer* g_pIoCtrlLayer = nullptr;
USING_NS_CC;

#include <platform/CCFileUtils.h>

#if ( VERSION_BUILD_NUMBER == 2 )
#include "D:/Pro_share/libs/CopyRight.h"
#pragma comment(lib, "D:/Pro_share/libs/CopyRight.lib")
#endif


AppDelegate::AppDelegate()
{
	srand(time(0));
}

AppDelegate::~AppDelegate() 
{
	if (g_pData)
		delete g_pData;
	if (g_pIoCtrlLayer)
		delete g_pIoCtrlLayer;
	DebugInfoLayer::myRelease();
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
	FileUtils::getInstance()->setPassword("sydmkj-cyhol");	//自编加密算法密码
#ifdef LOAD_ENCODE_RESOUCE
	ResourcesDecode::getInstance()->setForceDir("C:\\");
	ResourcesDecode::getInstance()->loadZIP("image.zip", "cyhol");	//压缩包密码
	ResourcesDecode::getInstance()->loadZIP("image2.zip", "cyhol");	//压缩包密码
	ResourcesDecode::getInstance()->loadZIP("sound.zip", "cyhol");
// 	ResourcesDecode::getInstance()->loadZIP("voice.zip", "cyhol");
#endif // LOAD_ENCODE_RESOUCE
	
	return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {

#if ( VERSION_BUILD_NUMBER == 2 )
	CCopyRight cp;
	if (0x133 != cp.getCopyRight())
		return false;
#endif

	//设置用户路径
	char userPath[256] = { 0 };
	GetCurrentDirectory(255, userPath);
	FileUtils::getInstance()->setWritablePath(std::string(userPath).append("\\"));

	// initialize director
	auto director = Director::getInstance();
	auto glview = (GLViewImpl*)director->getOpenGLView();
	if(!glview) {
		glview = GLViewImpl::create(GAME_NAME);
		director->setOpenGLView(glview);
	}

	bool bDisplayStats = MyIniGetInt("SETTING", USER_DEF_DISPLAY_STATS, 0) != 0;
	director->setDisplayStats(bDisplayStats);

	director->setAnimationInterval(1.0f / 60);

	VisibleRect::SetVisibleRect(Rect(0, 0, GAME_WIDTH, GAME_HEIGHT));
	auto designSize = Size(SCREEN_WIDTH, SCREEN_HEIGHT);

	glview->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::EXACT_FIT);

#ifdef FULL_SCREEN
	::ShowCursor(true);
	glview->setFullscreen();
#else
	glview->setWindowed(designSize.width, designSize.height);
#endif


	register_all_packages();

	//数据加载
	g_pData = new CDataMgr(GAME_NAME, eDataKey::key_count);
	g_pData->init();
	dataInit();


	g_pIoCtrlLayer = new IoCtrlLayer();	// not autoRelease

    // create a scene. it's an autorelease object
	Scene* scene = nullptr;

	auto verifyScene = Scene_Verify::createScene();
	Scene_Verify* pVerify = (Scene_Verify*)verifyScene->getChildByName("verify");
	if (!pVerify->IsCanPlay())
	{
		scene = verifyScene;
	}
	else
	{
#define LOAD_START_SCENE	0
#if (LOAD_START_SCENE == 0)
		scene = Scene_Game::createScene();
#elif (LOAD_START_SCENE == 1)
		scene = Scene_Award::createScene();
#elif (LOAD_START_SCENE == 2)
		scene = Scene_Menu::createScene();
#endif // _DEBUG
	}

    // run
	director->runWithScene(HorizontalScreen(scene));

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be paused
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
