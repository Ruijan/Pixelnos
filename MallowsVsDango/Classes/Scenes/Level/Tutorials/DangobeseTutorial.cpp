#include "DangobeseTutorial.h"
#include "../Level.h"
#include "../Interface/LevelInterface.h"

DangobeseTutorial::DangobeseTutorial(TutorialSettings* settings, LevelInterface* levelInterface, Level* level) :
	DialogueTutorial(settings),
	level(level),
	levelInterface(levelInterface)
{
}

DangobeseTutorial::~DangobeseTutorial() {
	if (running) {
		levelInterface->removeChild(dialogues);
	}
}

bool DangobeseTutorial::isDone()
{
	return settings->isTutorialComplete("dangobese");
}

bool DangobeseTutorial::areConditionsMet()
{
	return level->getLevelId() == settings->getSettingsMap()["dangobese"]["level"].asInt() &&
		level->getWorldId() == settings->getSettingsMap()["dangobese"]["world"].asInt();
}


void DangobeseTutorial::startDialogues()
{
	running = true;
	level->pause();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["dangobese"]["dialogue"]);
	levelInterface->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	levelInterface->hideStartMenu();
	levelInterface->lockStartMenu();
}

void DangobeseTutorial::endTutorial()
{
	levelInterface->removeChild(dialogues);
	dialogues = nullptr;
	level->resume();
	settings->completeTutorial("dangobese");
	levelInterface->unlockStartMenu();
	levelInterface->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
	running = false;
}
