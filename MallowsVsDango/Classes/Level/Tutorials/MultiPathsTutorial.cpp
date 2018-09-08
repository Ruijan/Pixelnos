#include "MultiPathsTutorial.h"
#include "../Level.h"
#include "../Interface/LevelInterface.h"

MultiPathsTutorial::MultiPathsTutorial(TutorialSettings* settings, LevelInterface* levelInterface, Level* level) :
	DialogueTutorial(settings),
	levelInterface(levelInterface),
	level(level)
{
}

void MultiPathsTutorial::startDialogues() {
	running = true;
	levelInterface->pauseLevel();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["multi_paths"]["dialogue"]);
	levelInterface->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	levelInterface->hideStartMenu();
	levelInterface->lockStartMenu();
}

void MultiPathsTutorial::endTutorial() {
	levelInterface->removeChild(dialogues);
	dialogues = nullptr;
	levelInterface->resumeLevel();
	settings->completeTutorial("multi_paths");
	levelInterface->unlockStartMenu();
	levelInterface->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
	running = false;
}

bool MultiPathsTutorial::isDone()
{
	return settings->isTutorialComplete("multi_paths");
}

bool MultiPathsTutorial::areConditionsMet()
{
	return level->getLevelId() == settings->getSettingsMap()["multi_paths"]["level"].asInt() &&
		level->getWorldId() == settings->getSettingsMap()["multi_paths"]["world"].asInt() &&
		levelInterface->getGameState() == LevelInterface::GameState::TITLE;
}

MultiPathsTutorial::~MultiPathsTutorial()
{
	if (running) {
		levelInterface->removeChild(dialogues);
	}
}
