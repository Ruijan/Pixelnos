#ifndef SKILLS_HPP
#define SKILLS_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../Config/json.h"
#include "../Lib/Translationable.h"

class Skills : public cocos2d::Scene, public Translationable
{
public:
	virtual bool init();
	CREATE_FUNC(Skills);
	virtual void switchLanguage();
	void selectSkill(int tier_id, int skill_id, cocos2d::ui::Button * skill);
	


private:
	virtual void onEnterTransitionDidFinish();
	void setSkill(int tier_id, int skill_id);
	void setDependency(int tier_id, int skill_id);
	void skillUpdate(int tier_id, int skill_id, cocos2d::ui::Button* skill);
	void showSkillValidationBuy();
	virtual void update(float dt);
	void updateTutorial(float dt);
	
	int selected_tier_id;
	int selected_skill_id;
	Json::Value config; //load data file
	Json::Value root; //load save file
	bool tutorial_running;
};
#endif