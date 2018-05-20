#pragma once

#include "DialogueTutorial.h"
#include "../Dialogue.h"
#include "../Level.h"
#include "../InterfaceGame.h"


class SaucerTutorial : public DialogueTutorial {
public:
	SaucerTutorial(InterfaceGame* interfaceGame, Level* level);
	void update(float dt);
	bool isDone();
protected:
	void startDialogues();
	void endTutorial();
	void showTower();
	bool isLastTowerCreatedASaucer();
	bool areConditionsMet();

protected:
	InterfaceGame* interfaceGame;
	Level* level;
};