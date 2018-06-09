#ifndef STORYMENU_HPP
#define STORYMENU_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../Lib/Translationable.h"

class StoryMenu : public cocos2d::Scene, public Translationable
{
	public:
	virtual bool init();
	CREATE_FUNC(StoryMenu);
	virtual ~StoryMenu();
	virtual void switchLanguage();
	virtual void onEnterTransitionDidFinish();
	
	void selectLevelCallBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type, 
		int level_id, int world_id);
	void changeWorld(cocos2d::Ref* sender, cocos2d::ui::PageView::EventType type);
	void showCredit(Ref* sender);
	void initLevels(cocos2d::ui::Layout* page, int id_world);
	virtual void update(float dt);
	void updateTutorial(float dt);

private:
	bool tutorial_running;
	
};

#endif
