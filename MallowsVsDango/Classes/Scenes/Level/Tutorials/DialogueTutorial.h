#pragma once
#include "Tutorial.h"
#include "../Dialogue.h"
#include "cocos2d.h"

class DialogueTutorial : public Tutorial {
public:
	DialogueTutorial(TutorialSettings* settings);
	virtual ~DialogueTutorial();
	virtual void update(float dt);
	virtual bool isDone() = 0;
	virtual bool areConditionsMet() = 0;
	virtual void startDialogues() = 0;
protected:
	Dialogue* dialogues;
};