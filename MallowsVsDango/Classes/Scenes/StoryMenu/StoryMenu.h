#ifndef STORYMENU_HPP
#define STORYMENU_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../AdvancedScene.h"
#include "../../Lib/Translationable.h"
#include "../../Config/Config.h"

class StoryMenu : public AdvancedScene, public Translationable
{
	public:
	static StoryMenu* create(Config* config, GUISettings* settings);
	virtual bool init(Config * config, GUISettings* settings);
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
	Config* configClass;
	
};

#endif
