#pragma once
#include "DialogueTutorial.h"
#include "../Dialogue.h"


class SugarTutorial : public DialogueTutorial {
public:
	SugarTutorial(InterfaceGame* interfaceGame);
	bool isDone();

protected:
	void startDialogues();
	virtual void endTutorial();
	virtual bool areConditionsMet();

protected:
	InterfaceGame* interfaceGame;
};