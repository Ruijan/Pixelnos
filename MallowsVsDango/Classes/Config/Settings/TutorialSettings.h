#pragma once
#include "Settings.h"
#include "cocos2d.h"
#include <iostream>

class TutorialSettings : public Settings {
public:
	void init(const std::string& configFilename, const std::string& saveFilename);
	void save();
	void startTutorial(const std::string& name);
	void completeTutorial(const std::string& name);

	bool isTutorialRunning(const std::string& name);
	bool isTutorialComplete(const std::string& name);

protected:
	void tryReadingTutoSaveFile();
	void resetTutorialsState();
	void createNewTutorialSave();

private:
	Json::Value tutorialStates;
	std::string saveFilename;
};