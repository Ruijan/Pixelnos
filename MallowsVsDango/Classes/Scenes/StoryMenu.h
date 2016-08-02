#ifndef STORYMENU_HPP
#define STORYMENU_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class StoryMenu : public cocos2d::Scene
{
	public:
	virtual bool init();
	CREATE_FUNC(StoryMenu);
	virtual void onEnterTransitionDidFinish();
	
	void selectLevelCallBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type, 
		int level_id, int world_id);
	void changeWorld(cocos2d::Ref* sender, cocos2d::ui::PageView::EventType type);
	void showCredit(Ref* sender);
	void initLevels(cocos2d::ui::Layout* page, int id_world);
	
};

#endif
