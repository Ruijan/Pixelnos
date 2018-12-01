#pragma once
#include "cocos2d.h"
#include "../Config/json.h"
#include "../Config/Settings/Settings.h"

class GUISettings: public Settings {

public: 
	GUISettings();
	void init(const std::string& settingsFilename, const std::string language);
	void update();
	const cocos2d::Size& getVisibleSize();
	const Json::Value& getButtons();
	const std::string getLanguage();
	std::string getButton(const std::string&);

protected:
	cocos2d::Size visibleSize;
	std::string language;
};