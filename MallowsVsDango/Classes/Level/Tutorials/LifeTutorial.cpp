#include "LifeTutorial.h"
#include "../InterfaceGame.h"
#include "../../Config/Config.h"

LifeTutorial::LifeTutorial(Config* config, InterfaceGame * nInterfaceGame):
	DialogueTutorial(config),
	interfaceGame(nInterfaceGame)
{
}

bool LifeTutorial::isDone() {
	return config->isGameTutorialComplete("life");
}

LifeTutorial::~LifeTutorial()
{
}

void LifeTutorial::startDialogues() {
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(config->getConfigValues(Config::ConfigType::GAMETUTORIAL)["life"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	shakeScaleElement(interfaceGame->getChildByName("label_information")->getChildByName("life"), true);
}

void LifeTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	config->completeTutorial("life");
	interfaceGame->getChildByName("label_information")->getChildByName("life")->stopAllActions();
	interfaceGame->getChildByName("label_information")->getChildByName("life")->setRotation(0);
	interfaceGame->getChildByName("label_information")->getChildByName("life")->setScale(1.f);
	Tutorial::endTutorial();
}

bool LifeTutorial::areConditionsMet() {
	return interfaceGame->getLifeQuantity() < 3;
}
