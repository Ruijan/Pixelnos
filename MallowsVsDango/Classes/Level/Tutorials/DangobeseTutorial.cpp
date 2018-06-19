#include "DangobeseTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"

DangobeseTutorial::DangobeseTutorial(TutorialSettings* settings, InterfaceGame* interfaceGame, Level* level) :
	DialogueTutorial(settings),
	level(level),
	interfaceGame(interfaceGame)
{
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

DangobeseTutorial::~DangobeseTutorial()
{
}

void DangobeseTutorial::startDialogues()
{
	level->pause();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["dangobese"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
}

void DangobeseTutorial::endTutorial()
{
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	level->resume();
	settings->completeTutorial("dangobese");
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}
