#pragma once
#include "DialogueTutorial.h"

class InterfaceGame;
class SugarTutorial : public DialogueTutorial {
public:
	SugarTutorial(TutorialSettings* settings, InterfaceGame* interfaceGame);
	virtual ~SugarTutorial();
	bool isDone();

protected:
	void startDialogues();
	virtual void endTutorial();
	virtual bool areConditionsMet();
	virtual void update(float dt);

protected:
	InterfaceGame* interfaceGame;
};