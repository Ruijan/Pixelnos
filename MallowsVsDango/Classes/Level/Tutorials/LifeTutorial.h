#pragma once
#include "DialogueTutorial.h"

class InterfaceGame;
class LifeTutorial : public DialogueTutorial {
public:
	LifeTutorial(TutorialSettings* settings, InterfaceGame* interfaceGame);
	bool isDone();
	virtual ~LifeTutorial();


protected:
	void startDialogues();
	void endTutorial();
	bool areConditionsMet();

protected:
	InterfaceGame* interfaceGame;
};