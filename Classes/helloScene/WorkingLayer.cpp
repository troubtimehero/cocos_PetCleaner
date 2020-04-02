#include "WorkingLayer.h"
#include "tool/MyTools.h"
#include "ui/CocosGUI.h"
#include "MachineDefined.h"
#include "GameData.h"
#include "ioCtrl/IoCtrlLayer.h"

#define WORKING_COLD_TIME	180	//����ʱ�䣨�룩����3����
#ifdef _DEBUG
#define WORKING_WET_TIME	1
#else
#define WORKING_WET_TIME	30
#endif // _DEBUG


bool g_bWorking = false;

bool WorkingLayer::init()
{
	if (!LayerColor::initWithColor(Color4B::WHITE, GAME_WIDTH, GAME_HEIGHT))
	{
		return false;
	}


	Label* tips = Label::createWithSystemFont(LanguageChoose("���������������ţ������Զ���ֹ", "Do Not Open The Door During Work,\notherwise, it will be terminated"), "����", 53);
	tips->setTextColor(Color4B::RED);
	tips->setPosition(GAME_WIDTH / 2, GAME_HEIGHT * 6 / 7);
	this->addChild(tips);


	Label* prepare = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::WHITE, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
	this->addChild(prepare);
	prepare->setString(LanguageChoose("����׼��...", "Preparing..."));
	prepare->setPosition(VisibleRect::center());
	prepare->runAction(Sequence::createWithTwoActions(DelayTime::create(2.5f), RemoveSelf::create()));
	prepare->setName("prepare");



	m_lWashing = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::GREEN, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
	this->addChild(m_lWashing);
	m_lWashing->setPosition(VisibleRect::center());
	m_lWashing->setVisible(false);
	m_lWashing->setString(LanguageChoose("���ڳ�ϴ...", "Washing..."));

	auto washRemain = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::GREEN, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
	m_lWashing->addChild(washRemain);
	washRemain->setPosition(Point(0, -GAME_HEIGHT / 4.5f) + m_lWashing->getContentSize() / 2);
	washRemain->setTag(1);



	m_lBlowing = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::RED, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
	this->addChild(m_lBlowing);
	m_lBlowing->setPosition(VisibleRect::center());
	m_lBlowing->setVisible(false);
	m_lBlowing->setString(LanguageChoose("���ں��...", "Drying..."));

	auto blowRemain = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::RED, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
	m_lBlowing->addChild(blowRemain);
	blowRemain->setPosition(Point(0, -GAME_HEIGHT / 4.5f) + m_lBlowing->getContentSize() / 2);
	blowRemain->setTag(1);



	//stop
	auto btnStop = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	btnStop->setPosition(VisibleRect::bottom() + Vec2(0, 30));
	btnStop->setTitleFontSize(38);
	btnStop->setTitleText(LanguageChoose("��ֹ", "Stop"));
	btnStop->addClickEventListener([this](Ref*){
		this->wantStop();
	});
	addChild(btnStop);


	return true;
}

void WorkingLayer::Start(int nWashTime, int nBlowTime, bool bHot)
{
#ifdef _DEBUG
	nWashTime = 10;
	nBlowTime = 30;
#endif // _DEBUG

	m_lotionTime = 20;//ϴ�ྫʱ����Ҫ����ʹ����������ڣ���װ���ķ���һ�㣬�þ��˻��ˮ��ķŶ�һ��

	m_washTime = nWashTime - WORKING_WET_TIME - m_lotionTime;
	m_blowTime = nBlowTime;
	m_bHotWind = bHot;

	static bool sbFastTest = (MyIniGetInt("SETTING", "FAST_TEST", 0) > 0);
	if (sbFastTest)
		m_washTime = nWashTime;


	m_step = e_stepReady;
	m_stepTime = 0;

	m_fFinTipsTime = 5;

	schedule(schedule_selector(WorkingLayer::timerRemain), 1, CC_REPEAT_FOREVER, 3);//3����ʾ������׼����
	scheduleUpdate();

	g_pCom->CloseAllDev();
	lightNormalOn(true);
}

void WorkingLayer::update(float delta)
{
	if (m_step < e_stepFinTips && !g_pIoCtrlLayer->isKeyKeep(IoCtrlLayer::e_key_fire))
	{
		Stop(true);
	}
}

void WorkingLayer::timerRemain(float delta)
{
	//��ϴʣ��ʱ�䣬���״̬�ж��õ������Ե�������
	switch (m_step)
	{
	case WorkingLayer::e_stepWet:
	case WorkingLayer::e_stepLotion:
	case WorkingLayer::e_stepClean:
		{
			Label* l = (Label*)m_lWashing->getChildByTag(1);
			if (--m_washTime >= 0)
				l->setString(NumberToTime(m_washTime));
			else
				l->setString("");
		}
		break;
	}

	//
	switch (m_step)
	{
	case WorkingLayer::e_stepReady:	//׼��
		if (!getChildByName("prepare"))
		{
			if (m_washTime > 0)
			{
				m_step = e_stepWet;
				m_stepTime = WORKING_WET_TIME;	//������һ�� 30���ʪ
				enableWork(e_workWash, true);	//��ϴ�� ��
			}
			else
			{
				m_step = e_stepWind;
				enableWork(e_workBlow, true);	//���� ��
			}
		}
		break;

	case e_stepWet:	//��ʪ��
		m_stepTime -= delta;
		if (m_stepTime <= 0) //��ʪ��
		{
			m_step = e_stepLotion;
			m_stepTime = 5;		//������һ�� ������ʹ���������ϴ�ྫ X ��
			enableWork(e_workLotion, true);		//ϴ�ྫ ��
		}
		break;

	case e_stepLotion:	//��ϴ�ྫ��
		m_stepTime -= delta;
		if (m_stepTime <= 0) //��ϴ�ྫ����
		{
			enableWork(e_workLotion, false);	//ϴ�ྫ ��
			m_step = e_stepClean;
		}
		break;

	case e_stepClean:
		do 
		{
			if (m_washTime <= 0)
			{
				enableWork(e_workWash, false);	//ϴ�巧 ��
				
				if (m_blowTime > 0)
				{
					m_stepTime = 10;	//ϴ��֮�䣬���10��
					m_step = e_stepWait;

					Label* prepare = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::WHITE, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
					this->addChild(prepare);
					prepare->setString(LanguageChoose("׼ �� �� �� ...", "Dry Preparing..."));
					prepare->setPosition(VisibleRect::center());
					prepare->runAction(Sequence::createWithTwoActions(DelayTime::create(m_stepTime-0.5f), RemoveSelf::create()));
				}
				else
				{
					m_step = e_stepStop;
				}
			}
		} while (false);
		break;

	case e_stepWait:
		m_stepTime -= delta;
		if (m_stepTime <= 0) //��ϴ�ྫ����
		{
			m_step = e_stepWind;
			enableWork(e_workBlow, true);	//���� ��
			enableWork(e_workHot, true);	//���� ��
		}
		break;

	case e_stepWind:
		do
		{
			m_blowTime--;
			Label* l = (Label*)m_lBlowing->getChildByTag(1);
			l->setString(NumberToTime(m_blowTime));
			if (m_blowTime == WORKING_COLD_TIME)	//ֹͣ���ȣ���һ��ʱ�併��
			{
				enableWork(e_workHot, false);	//���� ��
			}

			if (m_blowTime <= 0)
			{
				m_step = e_stepStop;
				enableWork(e_workBlow, false);	//���� ��
			}
		} while (false);
		break;

	case e_stepStop:
		do
		{
			Label* fin = createFontTTF("fonts/dai meng wa wa ti.ttf", 80, 5, Color4B::YELLOW, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
			this->addChild(fin);
			fin->setString(LanguageChoose("���\n\n�뿪�Ŵ������뿪", "Finish\n\nPlease open the door"));
			fin->setPosition(VisibleRect::center());
			fin->runAction(Sequence::createWithTwoActions(DelayTime::create(2.5f), RemoveSelf::create()));

			this->runAction(Sequence::createWithTwoActions(DelayTime::create(3), RemoveSelf::create()));

			MySound::playEffect("sound/a_win.mp3");
		} while (false);

		m_step = e_stepFinTips;

		break;

	case e_stepFinTips:	//ֻ��Ҫ��˸��������������ʾ
		m_fFinTipsTime -= delta;
		lightNormalOn(static_cast<int>(m_fFinTipsTime * 2) % 2 != 0);
		if (m_fFinTipsTime <= 0)
		{
			lightNormalOn(true);
			m_step = e_stepNull;
		}
		break;

	case e_stepNull:
		break;
	}
}

void WorkingLayer::enableWork(eWork w, bool bEnable)
{
	switch (w)
	{
	case WorkingLayer::e_workLotion:
		g_pCom->SetDevOut(DEV_WORKING_LOTION.pid, DEV_WORKING_LOTION.bit, bEnable);
		break;

	case WorkingLayer::e_workWash:
		m_lWashing->setVisible(bEnable);
		g_pCom->SetDevOut(DEV_WORKING_WASH.pid, DEV_WORKING_WASH.bit, bEnable);
		break;

	case WorkingLayer::e_workBlow:
		m_lBlowing->setVisible(bEnable);
		g_pCom->SetDevOut(DEV_WORKING_BLOW.pid, DEV_WORKING_BLOW.bit, bEnable);
		g_pCom->SetDevOut(DEV_LIGHT_PURPLE.pid, DEV_LIGHT_PURPLE.bit, bEnable);

	case WorkingLayer::e_workHot:
		g_pCom->SetDevOut(DEV_WORKING_HOT.pid, DEV_WORKING_HOT.bit, bEnable);
		break;
	}
}

void WorkingLayer::wantStop()
{
	LayerColor* l = LayerColor::create(Color4B(0, 0, 0, 200));
	this->addChild(l);
	l->setName("are you sure");
	DisableBelowTouch(l);

	Label* tips = Label::createWithSystemFont(LanguageChoose("��ֹ�󽫲��ɼ���", "No Longer Work After Stop"), "����", 83);
	l->addChild(tips);
	tips->setTextColor(Color4B::YELLOW);
	tips->setPosition(GAME_WIDTH/2, GAME_HEIGHT * 2 / 3);

	ui::Button* yes = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	yes->setPosition(Point(GAME_WIDTH/3, GAME_HEIGHT/3));
	yes->setTitleFontSize(38);
	yes->setTitleText(LanguageChoose("ȷ��", "OK"));
	yes->addClickEventListener([this](Ref*){
		this->Stop(false);
	});
	l->addChild(yes);

	ui::Button* no = ui::Button::create("image/menu/button_normal.png", "image/menu/button_pressed.png");
	no->setPosition(Point(GAME_WIDTH *2 / 3, GAME_HEIGHT/3));
	no->setTitleFontSize(38);
	no->setTitleText(LanguageChoose("ȡ��", "Cancel"));
	no->addClickEventListener([this](Ref*){
		Node* n = this->getChildByName("are you sure");
		if (n)
			n->removeFromParent();
	});
	l->addChild(no);
}

void WorkingLayer::Stop(bool bOpenDoor)
{
	this->m_step = e_stepFinTips;
	g_pCom->CloseAllDev();

	this->removeAllChildren();


	Label* tips = createFontTTF("fonts/dai meng wa wa ti.ttf", 70, 5, Color4B::GREEN, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
	if (bOpenDoor)
		tips->setString(LanguageChoose("��    ��\n\n�ѽ�����ֹ", "Terminated Urgently\n\nBy Door Opening"));
	else
		tips->setString(LanguageChoose("�Ѱ�ȫ��ֹ\n\n�ɷ����뿪", "Has Been Stop\n\nCan Leave Safely"));
	tips->setPosition(VisibleRect::center());
	this->addChild(tips);


	if (!bOpenDoor)
	{
		tips->setVisible(false);
		tips->runAction(Sequence::createWithTwoActions(DelayTime::create(5), Show::create()));

		Label* tips2 = createFontTTF("fonts/dai meng wa wa ti.ttf", 70, 5, Color4B::GREEN, Color4B::BLACK, Color4B(74, 0, 0, 255), true);
		this->addChild(tips2);
		tips2->setString(LanguageChoose("����ֹͣ����", "Stop Working"));
		tips2->setPosition(VisibleRect::center());
		tips2->runAction(Sequence::createWithTwoActions(DelayTime::create(3.5f), RemoveSelf::create()));
	}
	

	this->runAction(Sequence::createWithTwoActions(DelayTime::create(bOpenDoor ? 10 : 15), RemoveSelf::create()));
}


//////////////////////////////////////////////////////////////////////////

void lightNormalOn(bool bOn)
{
	g_pCom->SetDevOut(DEV_LIGHT_NORMAL.pid, DEV_LIGHT_NORMAL.bit, bOn);
}
