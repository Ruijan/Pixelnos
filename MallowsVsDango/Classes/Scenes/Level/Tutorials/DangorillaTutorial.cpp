#include "DangorillaTutorial.h"
#include "../Level.h"
#include "../Interface/LevelInterface.h"


DangorillaTutorial::DangorillaTutorial(TutorialSettings* settings, LevelInterface * levelInterface, Level* level, GUISettings* guiSettings) :
	DialogueTutorial(settings, guiSettings),
	level(level),
	levelInterface(levelInterface)
{
}

bool DangorillaTutorial::isDone()
{
	return settings->isTutorialComplete("dangorilla");
}

bool DangorillaTutorial::areConditionsMet()
{
	return level->getLevelId() == settings->getSettingsMap()["dangorilla"]["level"].asInt() &&
		level->getWorldId() == settings->getSettingsMap()["dangorilla"]["world"].asInt() &&
		level->getLastEnemy() != nullptr &&
		level->getLastEnemy()->getSpecConfig()["name"].asString() == "Dangorille";
}

DangorillaTutorial::~DangorillaTutorial()
{
	if (running) {
		levelInterface->removeChild(dialogues);
	}
}

void DangorillaTutorial::startDialogues()
{
	running = true;
	level->pause();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["dangorilla"]["dialogue"], guiSettings);
	levelInterface->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	levelInterface->hideStartMenu();
	levelInterface->lockStartMenu();
}

void DangorillaTutorial::endTutorial()
{
	levelInterface->removeChild(dialogues);
	dialogues = nullptr;
	level->resume();
	settings->completeTutorial("dangorilla");
	levelInterface->unlockStartMenu();
	levelInterface->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
	running = false;
}