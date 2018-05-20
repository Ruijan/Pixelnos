#pragma once
#include "DialogueTutorial.h"

class InterfaceGame;
class SugarTutorial : public DialogueTutorial {
public:
	SugarTutorial(Config* config, InterfaceGame* interfaceGame);
	bool isDone();

protected:
	void startDialogues();
	virtual void endTutorial();
	virtual bool areConditionsMet();

protected:
	InterfaceGame* interfaceGame;
};