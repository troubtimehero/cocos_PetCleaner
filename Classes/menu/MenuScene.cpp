#include "MenuScene.h"

USING_NS_CC;

#include "SettingLayer_Pet.h"
#include "SettingLayer.h"
#include "RecordLayer.h"
#include "ioCtrl/IoCtrlLayer.h"

Scene* Scene_Menu::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Scene_Menu::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Scene_Menu::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}


	auto settingPet = SettingLayer_Pet::create();
	auto setting = SettingLayer::create();
	auto record = RecordLayer::create();
	auto lm = LayerMultiplex::create(settingPet, setting, record, nullptr);
	this->addChild(lm, 0, 1);
//	lm->switchTo(0);

	return true;
}

#include "tool/CheckRemovable.h"
#include "tool/MyFile.h"
void Scene_Menu::onEnter()
{
	SceneMgr::onEnter();
	g_pIoCtrlLayer->setOpacity(0);
	g_pIoCtrlLayer->setTag(IoCtrlLayer::e_scene_other);

#ifdef USE_REMOVEDISK_UPDATE_EXE
	ReInitUSB_Disk_Letter();
	if (System_My_GetRemovableList()[0] != 0)
	{
		int disk = System_My_GetRemovableList()[1];
#if 1
		char curDir[128] = { 0 };
		GetCurrentDirectory(127, curDir);

		char tarDir[32] = { 0 };
		sprintf(tarDir, "%c:\\", disk);


		//判断U盘的AutoUpdate.exe是否存在
		string exePath = tarDir;
		exePath += "AutoUpdate.exe";
		if (!MyFileFind::isExist(exePath))
			return;


		//复制到C盘
		char tarCmdCopy[128] = { 0 };
		sprintf(tarCmdCopy, "copy \"%c:\\AutoUpdate.exe\" \"C:\\\"", disk);
		system(tarCmdCopy);


		//运行C盘的AutoUpdate.exe
		char tarCmdRun[128] = { 0 };
		sprintf(tarCmdRun, "\"C:\\AutoUpdate.exe\" \"%s\" \"%c\"", curDir, disk);


		SetCurrentDirectory("C:\\");
		WinExec(tarCmdRun, SW_SHOW);
		exit(0);
#endif
	}
#endif // USE_REMOVEDISK_QRCODE_UPDATE

}

void Scene_Menu::onExit()
{
	g_pIoCtrlLayer->setOpacity(255);
	SceneMgr::onExit();
}


