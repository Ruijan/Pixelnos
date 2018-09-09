#include "Settings.h"
#include "../Exceptions/JsonContentException.h"

Settings * Settings::create(const std::string & settingsFilename)
{
	Settings* settings = new Settings();
	settings->init(settingsFilename);
	return settings;
}

Settings::Settings() :
	reader(new Json::Reader()),
	fileUtils(cocos2d::FileUtils::getInstance())
{
}

void Settings::init(const std::string& settingsFilename)
{
	this->settingsFilename = settingsFilename;
	tryReadingSettingsFile();
}

const Json::Value & Settings::getSettingsMap()
{
	return settingsMap;
}

void Settings::tryReadingSettingsFile()
{
	bool parsingConfigSuccessful = reader->parse(fileUtils->getStringFromFile(settingsFilename), settingsMap, false);
	if (!parsingConfigSuccessful) {
		throw new JsonContentException(reader->getFormattedErrorMessages());
	}
}
