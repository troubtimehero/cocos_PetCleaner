#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "menu/SettingLayer.h"
#include "tool/MyTools.h"
#include "ioCtrl/IoCtrlLayer.h"
#include "debug/DebugInfoLayer.h"
#include "cocos/ui/CocosGUI.h"
#include "GameData.h"
#include "MainLayer.h"

USING_NS_CC;
MySound		g_sound = MySound();
Scene* Scene_Game::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
	auto world = scene->getPhysicsWorld();

#ifdef _DEBUG
// 	world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif // _DEBUG


    // 'layer' is an autorelease object
    auto layer = Scene_Game::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Scene_Game::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //////////////////////////////////////////////////////////////////////////
    
	Layer* selectLayer = MainLayer::create();
	this->addChild(selectLayer);

    return true;
}

void Scene_Game::onEnter()
{
	SceneMgr::onEnter();
	g_pIoCtrlLayer->setTag(IoCtrlLayer::e_scene_game);

	MySound::setVolume(g_pData->get(eDataKey::set_Volume) / 100.0f);
#ifndef SCAN_MUSIC
	MySound::playMusic("sound/background.mp3", true);
#endif // SCAN_MUSIC
}

void Scene_Game::onExit()
{
#ifndef SCAN_MUSIC
	MySound::stopMusic();
#endif
	SceneMgr::onExit();
	g_pIoCtrlLayer->setTag(IoCtrlLayer::e_scene_other);
}



void Scene_Game::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}

