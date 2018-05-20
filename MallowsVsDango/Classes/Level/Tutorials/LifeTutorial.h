#pragma once

#include "DialogueTutorial.h"
#include "../Dialogue.h"


class LifeTutorial : public DialogueTutorial {
public:
	LifeTutorial(InterfaceGame* interfaceGame);
	bool isDone();
	virtual ~LifeTutorial();


protected:
	void startDialogues();
	void endTutorial();
	bool areConditionsMet();

protected:
	InterfaceGame* interfaceGame;
};