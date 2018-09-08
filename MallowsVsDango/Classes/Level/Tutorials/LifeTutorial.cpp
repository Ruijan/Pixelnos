#include "LifeTutorial.h"
#include "../Interface/LevelInterface.h"

LifeTutorial::LifeTutorial(TutorialSettings* settings, LevelInterface * nInterfaceGame):
	DialogueTutorial(settings),
	levelInterface(nInterfaceGame)
{
}

bool LifeTutorial::isDone() {
	return settings->isTutorialComplete("life");
}

LifeTutorial::~LifeTutorial()
{
	if (running) {
		levelInterface->removeChild(dialogues);
	}
}

void LifeTutorial::startDialogues() {
	running = true;
	levelInterface->pauseLevel();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["life"]["dialogue"]);
	levelInterface->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	shakeScaleElement(levelInterface->getChildByName("label_information")->getChildByName("life"), true);
}

void LifeTutorial::endTutorial() {
	levelInterface->removeChild(dialogues);
	dialogues = nullptr;
	levelInterface->resumeLevel();
	settings->completeTutorial("life");
	levelInterface->getChildByName("label_information")->getChildByName("life")->stopAllActions();
	levelInterface->getChildByName("label_information")->getChildByName("life")->setRotation(0);
	levelInterface->getChildByName("label_information")->getChildByName("life")->setScale(1.f);
	Tutorial::endTutorial();
	running = false;
}

bool LifeTutorial::areConditionsMet() {
	return levelInterface->getLifeQuantity() < 3;
}
