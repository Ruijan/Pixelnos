#pragma once
#include "DialogueTutorial.h"
#include "../Level.h"

class LevelInterface;
class Config;

class SaucerTutorial : public DialogueTutorial {
public:
	SaucerTutorial(Config* config, LevelInterface* levelInterface, Level* level, GUISettings* guiSettings);
	virtual ~SaucerTutorial();
	void update(float dt);
	bool isDone();
protected:
	void startDialogues();
	void endTutorial();
	void showTower();
	bool isLastTowerCreatedASaucer();
	bool areConditionsMet();

protected:
	LevelInterface* levelInterface;
	Level* level;
	Config* config;
};