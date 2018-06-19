#pragma once
#include "../json.h"
#include "cocos2d.h"

class Settings {
public:
	Settings();
	void init(std::string settingsFilename);
	const Json::Value& getSettingsMap();

protected:
	void tryReadingSettingsFile();

protected:
	Json::Value settingsMap;
	std::string settingsFilename;
	Json::Reader* reader;
	cocos2d::FileUtils* fileUtils;
};