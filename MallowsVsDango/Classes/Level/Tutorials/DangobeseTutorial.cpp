#include "DangobeseTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"

DangobeseTutorial::DangobeseTutorial(TutorialSettings* settings, InterfaceGame* interfaceGame, Level* level) :
	DialogueTutorial(settings),
	level(level),
	interfaceGame(interfaceGame)
{
}

DangobeseTutorial::~DangobeseTutorial() {
	if (running) {
		interfaceGame->removeChild(dialogues);
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
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
	interfaceGame->lockStartMenu();
}

void DangobeseTutorial::endTutorial()
{
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	level->resume();
	settings->completeTutorial("dangobese");
	interfaceGame->unlockStartMenu();
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
	running = false;
}
