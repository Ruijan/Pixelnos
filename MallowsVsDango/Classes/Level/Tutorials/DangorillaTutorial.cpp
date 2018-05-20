#include "DangorillaTutorial.h"
#include "../InterfaceGame.h"
#include "../../AppDelegate.h"


DangorillaTutorial::DangorillaTutorial(InterfaceGame * interfaceGame, Level* level):
	level(level), 
	interfaceGame(interfaceGame)
{
}

bool DangorillaTutorial::isDone()
{
	return ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->isGameTutorialComplete("dangorilla");
}

bool DangorillaTutorial::areConditionsMet()
{
	auto config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL);
	return level->getLevelId() == config["dangorilla"]["level"].asInt() &&
		level->getWorldId() == config["dangorilla"]["world"].asInt() &&
		level->getLastEnemy() != nullptr &&
		level->getLastEnemy()->getSpecConfig()["name"].asString() == "Dangorille";
}

DangorillaTutorial::~DangorillaTutorial()
{
}

void DangorillaTutorial::startDialogues()
{
	level->pause();
	dialogues = Dialogue::createFromConfig(
		((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL)["dangorilla"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
}

void DangorillaTutorial::endTutorial()
{
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	level->resume();
	((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->completeTutorial("dangorilla");
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}
