#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "tool/SceneMgr.h"

class Scene_Game : public SceneMgr
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	void onEnter();
	void onExit();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(Scene_Game);
};

#endif // __HELLOWORLD_SCENE_H__
