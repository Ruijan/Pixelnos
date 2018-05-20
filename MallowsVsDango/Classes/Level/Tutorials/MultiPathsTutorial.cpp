#include "MultiPathsTutorial.h"
#include "../InterfaceGame.h"
#include "../../AppDelegate.h"

MultiPathsTutorial::MultiPathsTutorial(InterfaceGame* interfaceGame, Level* level) :
	interfaceGame(interfaceGame),
	level(level)
{
}

void MultiPathsTutorial::startDialogues() {
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL)["multi_paths"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
}

void MultiPathsTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->completeTutorial("multi_paths");
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}

bool MultiPathsTutorial::isDone()
{
	return ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->isGameTutorialComplete("multi_paths");
}

bool MultiPathsTutorial::areConditionsMet()
{
	auto config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL);

	return level->getLevelId() == config["multi_paths"]["level"].asInt() &&
		level->getWorldId() == config["multi_paths"]["world"].asInt() &&
		interfaceGame->getGameState() == InterfaceGame::GameState::TITLE;
}

MultiPathsTutorial::~MultiPathsTutorial()
{
}
