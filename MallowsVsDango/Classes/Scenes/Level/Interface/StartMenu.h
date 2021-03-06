#pragma once
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "../../../Config/json.h"
#include "../../../GUI/GUISettings.h"

class LevelInterface;

class StartMenu : public cocos2d::ui::Layout {
public:
	StartMenu();
	static StartMenu* create(LevelInterface* levelInterface, int levelId);
	void displayWithAnimation();
	virtual void hide();
	virtual ~StartMenu();
	virtual void reset(int levelId);
	void lock(bool locked);

protected:
	LevelInterface* levelInterface;
	bool hidden;
	bool locked;
	GUISettings* settings;

protected:
	void startButtonCallback(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	virtual bool init(LevelInterface* levelInterface, int levelId);
	virtual void addTitleLabel(int levelId);
	virtual void addAdviceLabel();
	virtual void addStartButton();
};