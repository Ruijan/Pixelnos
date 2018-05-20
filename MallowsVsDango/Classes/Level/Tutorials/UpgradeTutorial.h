#pragma once

#include "DialogueTutorial.h"
#include "../Dialogue.h"
#include "../Level.h"
#include "../InterfaceGame.h"

class UpgradeTutorial : public DialogueTutorial {
public:
	UpgradeTutorial(InterfaceGame* interfaceGame, Level* level);
	virtual void update(float dt);
	bool isDone();
	virtual ~UpgradeTutorial();

protected:
	void startDialogues();
	void showHand();
	void endTutorial();
	bool areConditionsMet();

protected:
	Tower* selectedTower;
	InterfaceGame* interfaceGame;
	Level* level;
};