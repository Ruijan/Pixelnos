#include "GUISettings.h"


void GUISettings::init(const std::string & settingsFilename, std::string language)
{
	Settings::init(settingsFilename);
	this->language = language;
	update();
}

GUISettings::GUISettings()
{
}

void GUISettings::update()
{
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
}

const cocos2d::Size & GUISettings::getVisibleSize()
{
	return visibleSize;
}

const Json::Value & GUISettings::getButtons()
{
	return settingsMap;
}

const std::string GUISettings::getLanguage()
{
	return language;
}

std::string GUISettings::getButton(const std::string & buttonKey)
{
	return settingsMap[buttonKey][language].asString();
}
