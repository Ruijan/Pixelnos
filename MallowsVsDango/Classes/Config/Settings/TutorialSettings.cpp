#include "TutorialSettings.h"
#include "../Exceptions/JsonContentException.h"

void TutorialSettings::init(const std::string& configFilename, const std::string& saveFilename)
{
	Settings::init(configFilename);
	this->saveFilename = saveFilename;
	tryReadingTutoSaveFile();
	save();
}

void TutorialSettings::save()
{
	Json::StyledWriter writer;
	std::string outputSave = writer.write(tutorialStates);
	std::string path = fileUtils->getWritablePath() + saveFilename;

	bool succeed = fileUtils->writeStringToFile(outputSave, path);
	if (succeed) {
		cocos2d::log("Saved File in %s", path.c_str());
	}
	else {
		cocos2d::log("error saving file %s", path.c_str());
	}
}

void TutorialSettings::startTutorial(const std::string& name) {
	tutorialStates[name]["state"] = "running";
	save();
}

void TutorialSettings::completeTutorial(const std::string& name) {
	tutorialStates[name]["state"] = "complete";
	save();
}

bool TutorialSettings::isTutorialRunning(const std::string& name)
{
	return tutorialStates[name]["state"].asString() == "running";
}

bool TutorialSettings::isTutorialComplete(const std::string& name)
{
	return tutorialStates[name]["state"].asString() == "complete";
}

void TutorialSettings::tryReadingTutoSaveFile()
{
	bool parsingSaveSuccessful = reader->parse(fileUtils->getStringFromFile(cocos2d::FileUtils::getInstance()->getWritablePath() + saveFilename), tutorialStates, false);
	if (!parsingSaveSuccessful) {
		createNewTutorialSave();
	}
	else {
		resetTutorialsState();
	}
}

void TutorialSettings::resetTutorialsState()
{
	auto tutos = tutorialStates.getMemberNames();
	for (unsigned int j(0); j < tutorialStates.getMemberNames().size(); ++j) {
		if (tutorialStates[tutos[j]]["state"].asString() == "running") {
			tutorialStates[tutos[j]]["state"] = "uncompleted";
		}
	}
}

void TutorialSettings::createNewTutorialSave()
{
	std::vector<std::string> tuto_names = settingsMap.getMemberNames();
	for (unsigned int i(0); i < tuto_names.size(); ++i) {
		tutorialStates[tuto_names[i]]["state"] = settingsMap[tuto_names[i]]["state"];
	}
}