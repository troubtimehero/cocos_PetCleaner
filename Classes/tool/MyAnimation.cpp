#include "MyAnimation.h"


MyAnimation::MyAnimation()
{
}


MyAnimation::~MyAnimation()
{
}

Animation* MyAnimation::create(std::string plist, float spf)
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist.c_str());

	auto ani = Animation::create();

	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(plist);


// 	ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
	Dictionary *dictionary = Dictionary::createWithContentsOfFileThreadSafe(fullPath.c_str());
	Dictionary *framesDict = static_cast<Dictionary*> (dictionary->objectForKey("frames"));

	Array *keys = framesDict->allKeys();
	if (keys->count() > 8)
	{
		for (int i = 0; i < 8; ++i)
		{
			keys->swap(i, i + 1);
		}
	}

	for (int i = 0; i < keys->count(); ++i)
	{
		String *spriteFileName = static_cast<String *> (keys->objectAtIndex(i));
		SpriteFrame* spr = SpriteFrameCache::getInstance()->spriteFrameByName(spriteFileName->getCString());
		if (!spr)
		{
			MessageBox("Í¼Æ¬Â·¾¶ÓÐ´í", spriteFileName->getCString());
		}
		ani->addSpriteFrame(spr);
	}
	

	ani->setDelayPerUnit(spf);
// 	ani->setRestoreOriginalFrame(true);

	return ani;
}
