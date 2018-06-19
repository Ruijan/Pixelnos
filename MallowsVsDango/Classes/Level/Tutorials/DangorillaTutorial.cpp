#include "DangorillaTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"


DangorillaTutorial::DangorillaTutorial(TutorialSettings* settings, InterfaceGame * interfaceGame, Level* level) :
	DialogueTutorial(settings),
	level(level),
	interfaceGame(interfaceGame)
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
}

void DangorillaTutorial::startDialogues()
{
	level->pause();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["dangorilla"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
}

void DangorillaTutorial::endTutorial()
{
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	level->resume();
	settings->completeTutorial("dangorilla");
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}