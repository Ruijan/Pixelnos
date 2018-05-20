#pragma once
#include "DialogueTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"

class InterfaceGame;
class Level;
class SaucerTutorial : public DialogueTutorial {
public:
	SaucerTutorial(Config* config, InterfaceGame* interfaceGame, Level* level);
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