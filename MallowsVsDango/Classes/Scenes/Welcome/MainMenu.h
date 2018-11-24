#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "../AdvancedScene.h"
#include "../../Lib/Loader.h"
#include "ui/CocosGUI.h"
#include "../../Lib/Translationable.h"
#include "../../Config/Config.h"


class MainMenu :public AdvancedScene, public Loader, public Translationable, public cocos2d::ui::EditBoxDelegate
{
public:
	
	static MainMenu* create(Config* config, GUISettings* settings);
	virtual bool init(Config* config, GUISettings* settings);
	void menuContinueCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void update(float dt);
	virtual void switchLanguage();


protected:
	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();
	void initLanguageList();
	virtual void editBoxEditingDidEndWithAction(cocos2d::ui::EditBox *editBox);
	virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox *editBox);
	virtual void editBoxTextChanged(cocos2d::ui::EditBox *editBox, std::string &text);
	virtual void editBoxReturn(cocos2d::ui::EditBox *editBox);

protected:
	Config * configClass;
};

#endif
