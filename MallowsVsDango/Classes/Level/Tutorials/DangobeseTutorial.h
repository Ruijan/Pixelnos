#pragma once
#include "DialogueTutorial.h"
#include "../Dialogue.h"
#include "../Level.h"

class InterfaceGame;
class DangobeseTutorial : public DialogueTutorial {
public:
	DangobeseTutorial(InterfaceGame* interfaceGame, Level* level);
	bool isDone();
	bool areConditionsMet();
	virtual ~DangobeseTutorial();

protected:
	void startDialogues();
	void endTutorial();

protected:
	InterfaceGame* interfaceGame;
	Level* level;
};
