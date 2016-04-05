#ifndef STORYMENU_HPP
#define STORYMENU_HPP

#include "cocos2d.h"

class StoryMenu : public cocos2d::Scene
{
	public:
	virtual bool init();
	CREATE_FUNC(StoryMenu);
	virtual void onEnterTransitionDidFinish();
	
	void selectLevelCallBack(Ref* sender, int level_id);
	void showCredit(Ref* sender);
	
	private:
	/*cocos2d::Menu* menu;*/
};

#endif
