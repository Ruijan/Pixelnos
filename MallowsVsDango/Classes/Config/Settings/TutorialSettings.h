#pragma once
#include "Settings.h"
#include "cocos2d.h"
#include <iostream>

class TutorialSettings : public Settings {
public:
	void init(std::string configFilename, std::string saveFilename);
	void save();
	void startTutorial(std::string name);
	void completeTutorial(std::string name);

	bool isTutorialRunning(std::string name);
	bool isTutorialComplete(std::string name);

protected:
	void tryReadingTutoSaveFile();
	void resetTutorialsState();
	void createNewTutorialSave();

private:
	Json::Value tutorialStates;
	std::string saveFilename;
};