#include "SugarTutorial.h"
#include "../InterfaceGame.h"
#include "../../AppDelegate.h"

SugarTutorial::SugarTutorial(InterfaceGame* nInterfaceGame) :
	interfaceGame(nInterfaceGame)
{
}

bool SugarTutorial::isDone() {
	return ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->isGameTutorialComplete("sugar");
}

void SugarTutorial::startDialogues() {
	auto config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL);
	interfaceGame->hideStartMenu();
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(config["sugar"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	shakeScaleElement(interfaceGame->getChildByName("label_information")->getChildByName("sugar"), true);
}

void SugarTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->completeTutorial("sugar");
	interfaceGame->resetSugarLabel();
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}

bool SugarTutorial::areConditionsMet() {
	return interfaceGame->getSugarQuantity() < 90;
}

