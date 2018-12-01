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
	void addLanguages(cocos2d::Size &visibleSize, std::string &language);
	void startSequenceOfAnimation(cocos2d::Size &visibleSize);
	void addUsername(cocos2d::Size &visibleSize, GUISettings * settings);
	void addStartButton(GUISettings * settings, cocos2d::Size &visibleSize);
	void addDangoText(cocos2d::Size &visibleSize);
	void addMallowsText(cocos2d::Size &visibleSize);
	void addVersus(cocos2d::Size &visibleSize);
	void addBattleBackground(cocos2d::Size &visibleSize);
	void addFlash(cocos2d::Size &visibleSize);
	void addBackground(cocos2d::Size &visibleSize);
	void addLoadingBackground(cocos2d::Size &visibleSize);
	void addBackgroungLogo(cocos2d::Size &visibleSize);
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

	cocos2d::Sprite* backgroundLogo;
	cocos2d::Sprite* background;
	cocos2d::Sprite* flash;
	cocos2d::Sprite* battleBackground;
	cocos2d::Sprite* vs;
	cocos2d::Sprite* mallowstxt;
	cocos2d::Sprite* dangotxt;
	cocos2d::ui::Button* button;
	cocos2d::ui::Layout* layout_username;
	cocos2d::ui::Button* mask;
};

#endif
