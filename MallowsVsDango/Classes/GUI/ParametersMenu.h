#pragma once
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "Config/json.h"
#include "../Scenes/MyGame.h"
#include "../Config/AudioController.h"
#include "../Config/Config.h"

class ParametersMenu : public cocos2d::ui::Layout{
public:
	static ParametersMenu* create(MyGame* game);
	virtual bool init(MyGame* game);
	
	void displayWithAnimation();
	virtual ~ParametersMenu();

protected:	
	cocos2d::ui::Layout* blackMask;
	MyGame* game;
	cocos2d::Vec2 lastObjectPosition;
	cocos2d::Size lastObjectSize;
	cocos2d::Size panelSize;

private:
	virtual void addRightButton(Json::Value buttons, const std::string& language);
	virtual void addLeftButton(Json::Value buttons, const std::string& language);
	void addBlackMask(const cocos2d::Size &visibleSize);
	void addGlobalSettings(Json::Value &buttons, std::string &language, const cocos2d::Size &visibleSize, Config * config);
	void addSoundController(const std::string& title, cocos2d::Size visibleSize, AudioController::SOUNDTYPE type);
	void addLoopCheckBox(Json::Value &buttons, std::string &language, const cocos2d::Size &visibleSize, AudioController * audioController);
	void addTitle(Json::Value &buttons, std::string &language, const cocos2d::Size &visibleSize);
	void addPanel(const cocos2d::Size &visibleSize);
	cocos2d::ui::CheckBox* createNormalCheckBox(const cocos2d::Size& visibleSize, cocos2d::Vec2 position);
	cocos2d::ui::CheckBox* createCheckBoxWithLabel(std::string label, const cocos2d::Size& visibleSize, int posXBox);
	cocos2d::Label* ParametersMenu::createLabelForGridCheckBox(std::string title, double fontSize);
};

cocos2d::Sprite* createButtonShadow(cocos2d::ui::Button* button);
cocos2d::ui::Button* createDefaultButton(const std::string& buttonTitle, const std::string& buttonImage, int panelWidth);
cocos2d::TargetedAction* createHideAction(cocos2d::Node* target);