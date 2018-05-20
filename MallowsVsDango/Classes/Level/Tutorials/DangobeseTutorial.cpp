#include "DangobeseTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"
#include "../../Config/Config.h"

DangobeseTutorial::DangobeseTutorial(Config* config, InterfaceGame * interfaceGame, Level * level) :
	DialogueTutorial(config),
	level(level),
	interfaceGame(interfaceGame)
{
}

bool DangobeseTutorial::isDone()
{
	return config->isGameTutorialComplete("dangobese");
}

bool DangobeseTutorial::areConditionsMet()
{
	return level->getLevelId() == config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["dangobese"]["level"].asInt() &&
		level->getWorldId() == config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["dangobese"]["world"].asInt();
}

DangobeseTutorial::~DangobeseTutorial()
{
}

void DangobeseTutorial::startDialogues()
{
	level->pause();
	dialogues = Dialogue::createFromConfig(config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["dangobese"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
}

void DangobeseTutorial::endTutorial()
{
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	level->resume();
	config->completeTutorial("dangobese");
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}
