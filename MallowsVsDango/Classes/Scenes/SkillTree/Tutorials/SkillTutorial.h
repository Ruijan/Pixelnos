#pragma once
#include "cocos2d.h"
#include "../../../Config/Settings/TutorialSettings.h"
#include "../../Level/Dialogue.h"
#include "../../../GUI/GUISettings.h"

class Skills;

class SkillTutorial {
public:
	SkillTutorial(TutorialSettings* settings, Skills* skillTree, GUISettings* guiSettings);
	virtual void update(float dt);
	virtual bool isDone();
	void showHowToBuySkill();
	cocos2d::Sprite* createHand(cocos2d::Size &visibleSize);
	virtual bool areConditionsMet();
	virtual void startDialogues();
	void addBlackMask();
	virtual ~SkillTutorial();

protected:
	cocos2d::CallFunc * createShowSkillFunction(cocos2d::Sprite * hand);
	void endTutorial();

protected:
	bool running;
	TutorialSettings* settings;
	Dialogue* dialogues;
	Skills* skillTree;
	bool showingHand;
	GUISettings* guiSettings;
};