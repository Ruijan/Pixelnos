#pragma once
#include "CentralMenu.h"
#include "../Config/json.h"
#include "../Scenes/MyGame.h"
#include "../Config/AudioController.h"
#include "../Config/Config.h"
#include "../Config/Settings.h"

typedef std::pair<AudioSlider*, cocos2d::ui::CheckBox*> SoundController;

class ParametersMenu : public CentralMenu{
public:
	static ParametersMenu* create(MyGame* game, Config* config);
	ParametersMenu(MyGame * game, Config * config);
	virtual bool init();
	void displayWithAnimation();
	virtual ~ParametersMenu();

protected:
	cocos2d::ui::Layout* blackMask;
	MyGame* game;
	cocos2d::Vec2 lastObjectPosition;
	cocos2d::Size lastObjectSize;
	cocos2d::ui::CheckBox* checkboxLoop;
	std::vector<SoundController> soundControllers;
	std::vector<cocos2d::ui::CheckBox*> settingsCheckboxes;
	Config* config;
	AudioController* audioController;

protected:
	virtual void addBottomButtons(Json::Value &buttons);
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void addBlackMask(const cocos2d::Size &visibleSize);
	void addGlobalSettings(Json::Value &buttons, const cocos2d::Size &visibleSize);
	SoundController createSoundController(const std::string& title, cocos2d::Size visibleSize, AudioController::SOUNDTYPE type);
	void addMusicLoopCheckBox(Json::Value &buttons, const cocos2d::Size &visibleSize, AudioController * audioController);
	void addTitle(Json::Value &buttons, const cocos2d::Size &visibleSize);
	cocos2d::ui::CheckBox* createNormalCheckBox(const cocos2d::Size& visibleSize, cocos2d::Vec2 position);
	cocos2d::ui::CheckBox* createCheckBoxWithLabel(std::string label, const cocos2d::Size& visibleSize, int posXBox);
	cocos2d::Label* ParametersMenu::createLabelForGridCheckBox(std::string title, double fontSize);
};