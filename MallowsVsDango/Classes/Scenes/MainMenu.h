#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "cocos2d.h"
#include "Lib/Loader.h"
#include "ui/CocosGUI.h"
#include "../Lib/Translationable.h"



class MainMenu :public cocos2d::Scene, public Loader, public Translationable
{
public:
	virtual bool init();
	CREATE_FUNC(MainMenu);
	void menuContinueCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void update(float dt);
	virtual void switchLanguage();

protected:
	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();
	void initLanguageList();
};

#endif
