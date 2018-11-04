#pragma once
#include "DialogueTutorial.h"

class LevelInterface;
class SugarTutorial : public DialogueTutorial {
public:
	SugarTutorial(TutorialSettings* settings, LevelInterface* levelInterface);
	virtual ~SugarTutorial();
	bool isDone();

protected:
	void startDialogues();
	virtual void endTutorial();
	virtual bool areConditionsMet();
	virtual void update(float dt);

protected:
	LevelInterface* levelInterface;
};