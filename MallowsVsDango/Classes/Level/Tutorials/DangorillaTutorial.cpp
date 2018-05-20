#include "DangorillaTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"
#include "../../Config/Config.h"


DangorillaTutorial::DangorillaTutorial(Config* config, InterfaceGame * interfaceGame, Level* level) :
	DialogueTutorial(config),
	level(level),
	interfaceGame(interfaceGame)
{
}

bool DangorillaTutorial::isDone()
{
	return config->isGameTutorialComplete("dangorilla");
}

bool DangorillaTutorial::areConditionsMet()
{
	return level->getLevelId() == config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["dangorilla"]["level"].asInt() &&
		level->getWorldId() == config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["dangorilla"]["world"].asInt() &&
		level->getLastEnemy() != nullptr &&
		level->getLastEnemy()->getSpecConfig()["name"].asString() == "Dangorille";
}

DangorillaTutorial::~DangorillaTutorial()
{
}

void DangorillaTutorial::startDialogues()
{
	level->pause();
	dialogues = Dialogue::createFromConfig(config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["dangorilla"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	interfaceGame->hideStartMenu();
}

void DangorillaTutorial::endTutorial()
{
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	level->resume();
	config->completeTutorial("dangorilla");
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}