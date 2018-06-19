#pragma once

#include "DialogueTutorial.h"
#include "../Level.h"
#include "../InterfaceGame.h"

class InterfaceGame;
class Level;
class MultiPathsTutorial : public DialogueTutorial {
public:
	MultiPathsTutorial(TutorialSettings* settings, InterfaceGame* interfaceGame, Level* level);
	bool isDone();
	bool areConditionsMet();
	virtual ~MultiPathsTutorial();


protected:
	void startDialogues();
	void endTutorial();

protected:
	InterfaceGame* interfaceGame;
	Level* level;
};
