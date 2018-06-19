#include "MultiPathsTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"

MultiPathsTutorial::MultiPathsTutorial(TutorialSettings* settings, InterfaceGame* interfaceGame, Level* level) :
	DialogueTutorial(settings),
	interfaceGame(interfaceGame),
	level(level)
{
}

void MultiPathsTutorial::startDialogues() {
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["multi_paths"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
}

void MultiPathsTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	settings->completeTutorial("multi_paths");
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}

bool MultiPathsTutorial::isDone()
{
	return settings->isTutorialComplete("multi_paths");
}

bool MultiPathsTutorial::areConditionsMet()
{
	return level->getLevelId() == settings->getSettingsMap()["multi_paths"]["level"].asInt() &&
		level->getWorldId() == settings->getSettingsMap()["multi_paths"]["world"].asInt() &&
		interfaceGame->getGameState() == InterfaceGame::GameState::TITLE;
}

MultiPathsTutorial::~MultiPathsTutorial()
{
}
