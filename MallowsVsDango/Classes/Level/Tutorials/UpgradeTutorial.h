#pragma once
#include "DialogueTutorial.h"


class InterfaceGame;
class Level;
class Tower;
class UpgradeTutorial : public DialogueTutorial {
public:
	UpgradeTutorial(Config* config, InterfaceGame* interfaceGame, Level* level);
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