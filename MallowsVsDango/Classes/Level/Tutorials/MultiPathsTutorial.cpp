#include "MultiPathsTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"
#include "../../Config/Config.h"

MultiPathsTutorial::MultiPathsTutorial(Config* config, InterfaceGame* interfaceGame, Level* level) :
	DialogueTutorial(config),
	interfaceGame(interfaceGame),
	level(level)
{
}

void MultiPathsTutorial::startDialogues() {
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["multi_paths"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
}

void MultiPathsTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	config->completeTutorial("multi_paths");
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}

bool MultiPathsTutorial::isDone()
{
	return config->isGameTutorialComplete("multi_paths");
}

bool MultiPathsTutorial::areConditionsMet()
{
	return level->getLevelId() == config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["multi_paths"]["level"].asInt() &&
		level->getWorldId() == config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["multi_paths"]["world"].asInt() &&
		interfaceGame->getGameState() == InterfaceGame::GameState::TITLE;
}

MultiPathsTutorial::~MultiPathsTutorial()
{
}
