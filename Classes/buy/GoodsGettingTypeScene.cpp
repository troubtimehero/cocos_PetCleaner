#include "GoodsGettingTypeScene.h"
#include "tool/MyTools.h"
#include "ui/CocosGUI.h"
#include "MachineDefined.h"
#include "GameData.h"
#include "BuyScene.h"
#include "BuyByGame.h"
#include "HelloScene/HelloWorldScene.h"

Scene* GoodsGettingTypeScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GoodsGettingTypeScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GoodsGettingTypeScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	if (dataGet_GameCent() == 0)	//禁止游戏时，直接进入购买界面
	{
		this->addChild(BuyLayer::create());
		return true;	//-----------------------返回
	}


	//提示
	auto tips = Label::createWithSystemFont("", "Arral", 75);
	this->addChild(tips);
	tips->setPosition(GAME_WIDTH / 2, GAME_HEIGHT * 3 / 4);
	tips->setString(LanguageChoose("请选择获取方式", "Please Choose a Way"));



	//选择购买
	auto btnBuy = ui::Button::create("image2/buy/btn_buy.png");
	btnBuy->setPosition(Point(GAME_WIDTH / 2, GAME_HEIGHT / 2 ));
	btnBuy->addClickEventListener([this](Ref* r){
		this->addChild(BuyLayer::create());
		this->m_fTimeout = CONTROL_TIMEOUT;
	});
	this->addChild(btnBuy);

	auto nameBuy = createButtonTTF(btnBuy, "fonts/dai meng wa wa ti.ttf", 75, 3, Color4B::YELLOW, Color4B::RED, Color4B(74, 0, 0, 255));
	nameBuy->setString(LanguageChoose("购买", "Buy"));
	


	//选择游戏
	auto btnGame = ui::Button::create("image2/buy/btn_game.png");
	btnGame->setPosition(Point(GAME_WIDTH / 2, GAME_HEIGHT / 4));
	btnGame->addClickEventListener([this](Ref* r){
		this->addChild(BuyByGame::create());
		this->m_fTimeout = CONTROL_TIMEOUT;
	});
	this->addChild(btnGame);
	auto nameGame = createButtonTTF(btnGame, "fonts/dai meng wa wa ti.ttf", 75, 3, Color4B::YELLOW, Color4B::RED, Color4B(74, 0, 0, 255));
	nameGame->setString(LanguageChoose("游戏获取", "Game"));



	//返回
	auto back = createEmptyBtnBack();
	this->addChild(back);
	back->addClickEventListener([this](Ref* r){
		Director::getInstance()->replaceScene(HorizontalScreen(Scene_Game::createScene()));
	});


	m_fTimeout = CONTROL_TIMEOUT;

	Label* lTimeout = Label::createWithSystemFont(StringUtils::format("%d", static_cast<int>(m_fTimeout)), "黑体", 53);
	this->addChild(lTimeout);
	lTimeout->setPosition(back->getPosition() + Vec2(150, 0));
	lTimeout->setName("timeout");


	scheduleUpdate();

	return true;
}

void GoodsGettingTypeScene::update(float delta)
{
	if (!getChildByName(LAYER_NAME_WANT_GAME) && !getChildByName(LAYER_NAME_BUY))
	{
		m_fTimeout -= delta;

		Label* lTimeout = (Label*)this->getChildByName("timeout");
		lTimeout->setString(StringUtils::format("%d", static_cast<int>(m_fTimeout)));

		if (m_fTimeout <= 0)
		{
			Director::getInstance()->replaceScene(HorizontalScreen(Scene_Game::createScene()));
		}
	}
}
