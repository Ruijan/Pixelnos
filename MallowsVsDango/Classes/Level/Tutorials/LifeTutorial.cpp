#include "LifeTutorial.h"
#include "../InterfaceGame.h"
#include "../../AppDelegate.h"

LifeTutorial::LifeTutorial(InterfaceGame * nInterfaceGame):
	interfaceGame(nInterfaceGame)
{
}

bool LifeTutorial::isDone() {
	return ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->isGameTutorialComplete("life");
}

LifeTutorial::~LifeTutorial()
{
}

void LifeTutorial::startDialogues() {
	auto config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GAMETUTORIAL);
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(config["life"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	shakeScaleElement(interfaceGame->getChildByName("label_information")->getChildByName("life"), true);
}

void LifeTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->completeTutorial("life");
	interfaceGame->getChildByName("label_information")->getChildByName("life")->stopAllActions();
	interfaceGame->getChildByName("label_information")->getChildByName("life")->setRotation(0);
	interfaceGame->getChildByName("label_information")->getChildByName("life")->setScale(1.f);
	Tutorial::endTutorial();
}

bool LifeTutorial::areConditionsMet() {
	return interfaceGame->getLifeQuantity() < 3;
}
