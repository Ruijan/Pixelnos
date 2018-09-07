#include "SugarTutorial.h"
#include "../InterfaceGame.h"

SugarTutorial::SugarTutorial(TutorialSettings* settings, InterfaceGame* nInterfaceGame) :
	DialogueTutorial(settings),
	interfaceGame(nInterfaceGame)
{
}

SugarTutorial::~SugarTutorial() {
	if (running) {
		interfaceGame->removeChild(dialogues);
	}
}

bool SugarTutorial::isDone() {
	return settings->isTutorialComplete("sugar");
}

void SugarTutorial::startDialogues() {
	running = true;
	interfaceGame->hideStartMenu();
	interfaceGame->lockStartMenu();
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["sugar"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	shakeScaleElement(interfaceGame->getChildByName("label_information")->getChildByName("sugar"), true);
}

void SugarTutorial::update(float dt) {
	DialogueTutorial::update(dt);
	if (!isDone() && areConditionsMet()) {
		interfaceGame->hideStartMenu();
		interfaceGame->lockStartMenu();
	}
}

void SugarTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	settings->completeTutorial("sugar");
	interfaceGame->resetSugarLabel();
	interfaceGame->unlockStartMenu();
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
	running = true;
}

bool SugarTutorial::areConditionsMet() {
	return interfaceGame->getSugarQuantity() < 90;
}

