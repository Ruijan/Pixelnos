#include "SugarTutorial.h"
#include "../Interface/LevelInterface.h"

SugarTutorial::SugarTutorial(TutorialSettings* settings, LevelInterface* nInterfaceGame, GUISettings* guiSettings) :
	DialogueTutorial(settings, guiSettings),
	levelInterface(nInterfaceGame)
{
}

SugarTutorial::~SugarTutorial() {
	if (running) {
		levelInterface->removeChild(dialogues);
	}
}

bool SugarTutorial::isDone() {
	return settings->isTutorialComplete("sugar");
}

void SugarTutorial::startDialogues() {
	running = true;
	levelInterface->hideStartMenu();
	levelInterface->lockStartMenu();
	levelInterface->pauseLevel();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["sugar"]["dialogue"], guiSettings);
	levelInterface->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	shakeScaleElement(levelInterface->getChildByName("label_information")->getChildByName("sugar"), true);
}

void SugarTutorial::update(float dt) {
	DialogueTutorial::update(dt);
	if (!isDone() && areConditionsMet()) {
		levelInterface->hideStartMenu();
		levelInterface->lockStartMenu();
	}
}

void SugarTutorial::endTutorial() {
	levelInterface->removeChild(dialogues);
	dialogues = nullptr;
	levelInterface->resumeLevel();
	settings->completeTutorial("sugar");
	levelInterface->resetSugarLabel();
	levelInterface->unlockStartMenu();
	levelInterface->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
	running = true;
}

bool SugarTutorial::areConditionsMet() {
	return levelInterface->getSugarQuantity() < 90;
}

