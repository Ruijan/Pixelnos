#include "DangobeseTutorial.h"
#include "../InterfaceGame.h"
#include "../../AppDelegate.h"
#include "../../GUI/StartMenu.h"

DangobeseTutorial::DangobeseTutorial(InterfaceGame * interfaceGame, Level * level) :
	level(level),
	interfaceGame(interfaceGame)
{
}

bool DangobeseTutorial::isDone()
{
	return ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->isGameTutorialComplete("dangobese");
}

bool DangobeseTutorial::areConditionsMet()
{
	auto config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL);

	return level->getLevelId() == config["dangobese"]["level"].asInt() &&
		level->getWorldId() == config["dangobese"]["world"].asInt();
}

DangobeseTutorial::~DangobeseTutorial()
{
}

void DangobeseTutorial::startDialogues()
{
	level->pause();
	dialogues = Dialogue::createFromConfig(
		((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL)["dangobese"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
}

void DangobeseTutorial::endTutorial()
{
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	level->resume();
	((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->completeTutorial("dangobese");
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}
