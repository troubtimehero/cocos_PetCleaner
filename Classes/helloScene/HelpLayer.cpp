#include "HelpLayer.h"
#include "tool/MyTools.h"
#include "ui/CocosGUI.h"
// #include "GameData.h"
// #include "RoleManager.h"
#include "MachineDefined.h"


bool HelpLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto cover = LayerColor::create(Color4B(0, 255, 255, 255), GAME_WIDTH, GAME_HEIGHT);
	this->addChild(cover);
	DisableBelowTouch(cover);

	//help
	auto btnClose = ui::Button::create("CloseNormal.png", "CloseSelected.png");
	btnClose->setPosition(VisibleRect::rightTop() - btnClose->getContentSize() / 2);
	btnClose->addClickEventListener([this](Ref* r){
		this->runAction(Sequence::create(
			Spawn::create(
			ScaleTo::create(0.3f, 0),
			MoveTo::create(0.3f, -VisibleRect::center()),
			nullptr),
			RemoveSelf::create(),
			nullptr));

		this->_parent->getChildByName("HelpButton")->runAction(Sequence::create(
			DelayTime::create(0.4f),
			Show::create(),
			nullptr));
	});
	addChild(btnClose);


	auto tips = Label::createWithSystemFont(GBKToUTF(string("游戏玩法：\n选择多个文字或图片中唯一不同的\n\n购买方法：\n投入足够币数\n选择扭蛋窗口对应的号码点击即可")), "黑体", 60);
	tips->setPosition(VisibleRect::center());
	this->addChild(tips);

	return true;
}
