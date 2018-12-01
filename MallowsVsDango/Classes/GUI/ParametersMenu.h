#pragma once
#include "CentralMenu.h"
#include "../Config/json.h"
#include "../Config/AudioController.h"
#include "../Config/Config.h"
#include "../Config/Settings/GameSettings.h"
#include "GUISettings.h"

typedef std::pair<AudioSlider*, cocos2d::ui::CheckBox*> SoundController;

class ParametersMenu : public CentralMenu {
public:
	static ParametersMenu* create(GameSettings* gameSettings, GUISettings* guiSettings);
	ParametersMenu(GameSettings* gameSettings, GUISettings* guiSettings);
	virtual bool init();
	void displayWithAnimation();
	virtual ~ParametersMenu();

protected:
	cocos2d::ui::Layout* blackMask;
	cocos2d::Vec2 lastObjectPosition;
	cocos2d::Size lastObjectSize;
	cocos2d::ui::CheckBox* checkboxLoop;
	std::vector<SoundController> soundControllers;
	std::vector<cocos2d::ui::CheckBox*> settingsCheckboxes;
	GameSettings* gameSettings;
	AudioController* audioController;
	GUISettings* guiSettings;

protected:
	virtual void addBottomButtons();
	virtual void rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	virtual void leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void addBlackMask(const cocos2d::Size &visibleSize);
	void addGlobalSettings( const cocos2d::Size &visibleSize);
	virtual void handleAlwaysShowGridButton(cocos2d::ui::Widget::TouchEventType type);
	virtual void handleMovingGridButton(cocos2d::ui::Widget::TouchEventType type);
	virtual void handleNeverShowGridButton(cocos2d::ui::Widget::TouchEventType type);
	SoundController createSoundController(const std::string& title, cocos2d::Size visibleSize, AudioController::SOUNDTYPE type);
	void addMusicLoopCheckBox(const cocos2d::Size &visibleSize, AudioController * audioController);
	void addTitle(const cocos2d::Size &visibleSize);
	cocos2d::ui::CheckBox* createNormalCheckBox(const cocos2d::Size& visibleSize, cocos2d::Vec2 position);
	cocos2d::ui::CheckBox* createCheckBoxWithLabel(std::string label, const cocos2d::Size& visibleSize, int posXBox);
	cocos2d::Label* ParametersMenu::createLabelForGridCheckBox(std::string title, double fontSize);
};