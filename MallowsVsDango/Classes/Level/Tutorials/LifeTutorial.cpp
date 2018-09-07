#include "LifeTutorial.h"
#include "../InterfaceGame.h"

LifeTutorial::LifeTutorial(TutorialSettings* settings, InterfaceGame * nInterfaceGame):
	DialogueTutorial(settings),
	interfaceGame(nInterfaceGame)
{
}

bool LifeTutorial::isDone() {
	return settings->isTutorialComplete("life");
}

LifeTutorial::~LifeTutorial()
{
	if (running) {
		interfaceGame->removeChild(dialogues);
	}
}

void LifeTutorial::startDialogues() {
	running = true;
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["life"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	shakeScaleElement(interfaceGame->getChildByName("label_information")->getChildByName("life"), true);
}

void LifeTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	settings->completeTutorial("life");
	interfaceGame->getChildByName("label_information")->getChildByName("life")->stopAllActions();
	interfaceGame->getChildByName("label_information")->getChildByName("life")->setRotation(0);
	interfaceGame->getChildByName("label_information")->getChildByName("life")->setScale(1.f);
	Tutorial::endTutorial();
	running = false;
}

bool LifeTutorial::areConditionsMet() {
	return interfaceGame->getLifeQuantity() < 3;
}
