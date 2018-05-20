#pragma once

#include "DialogueTutorial.h"
#include "../Dialogue.h"
#include "../Level.h"
#include "../InterfaceGame.h"

class MultiPathsTutorial : public DialogueTutorial {
public:
	MultiPathsTutorial(InterfaceGame* interfaceGame, Level* level);
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
