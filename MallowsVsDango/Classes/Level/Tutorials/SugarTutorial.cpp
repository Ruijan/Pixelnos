#include "SugarTutorial.h"
#include "../InterfaceGame.h"

SugarTutorial::SugarTutorial(TutorialSettings* settings, InterfaceGame* nInterfaceGame) :
	DialogueTutorial(settings),
	interfaceGame(nInterfaceGame)
{
}

bool SugarTutorial::isDone() {
	return settings->isTutorialComplete("sugar");
}

void SugarTutorial::startDialogues() {
	interfaceGame->hideStartMenu();
	interfaceGame->pauseLevel();
	dialogues = Dialogue::createFromConfig(settings->getSettingsMap()["sugar"]["dialogue"]);
	interfaceGame->addChild(dialogues, 1, "dialogue");
	dialogues->launch();
	shakeScaleElement(interfaceGame->getChildByName("label_information")->getChildByName("sugar"), true);
}

void SugarTutorial::endTutorial() {
	interfaceGame->removeChild(dialogues);
	dialogues = nullptr;
	interfaceGame->resumeLevel();
	settings->completeTutorial("sugar");
	interfaceGame->resetSugarLabel();
	interfaceGame->displayStartMenuIfInTitleState();
	Tutorial::endTutorial();
}

bool SugarTutorial::areConditionsMet() {
	return interfaceGame->getSugarQuantity() < 90;
}

