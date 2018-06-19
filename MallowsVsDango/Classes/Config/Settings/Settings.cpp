#include "Settings.h"
#include "../Exceptions/JsonContentException.h"

Settings::Settings() :
	reader(new Json::Reader()),
	fileUtils(cocos2d::FileUtils::getInstance())
{
}

void Settings::init(std::string settingsFilename)
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
