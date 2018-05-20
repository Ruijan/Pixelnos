#include "SugarTutorial.h"
#include "../InterfaceGame.h"
#include "../../Config/Config.h"

SugarTutorial::SugarTutorial(Config* config, InterfaceGame* nInterfaceGame) :
	DialogueTutorial(config),
	interfaceGame(nInterfaceGame)
{
}

bool SugarTutorial::isDone() {
	return config->isGameTutorialComplete("sugar");
}

void SugarTutorial::startDialogues() {
	interfaceGame->hideStartMenu();
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["sugar"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	shakeScaleElement(interfaceGame->getChildByName("label_information")->getChildByName("sugar"), true);
}

void SugarTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	config->completeTutorial("sugar");
	interfaceGame->resetSugarLabel();
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}

bool SugarTutorial::areConditionsMet() {
	return interfaceGame->getSugarQuantity() < 90;
}

