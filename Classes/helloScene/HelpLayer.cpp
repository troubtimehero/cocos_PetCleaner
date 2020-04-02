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


	auto tips = Label::createWithSystemFont(GBKToUTF(string("��Ϸ�淨��\nѡ�������ֻ�ͼƬ��Ψһ��ͬ��\n\n���򷽷���\nͶ���㹻����\nѡ��Ť�����ڶ�Ӧ�ĺ���������")), "����", 60);
	tips->setPosition(VisibleRect::center());
	this->addChild(tips);

	return true;
}
