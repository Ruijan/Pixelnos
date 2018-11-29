#ifndef SKILLS_HPP
#define SKILLS_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../Config/json.h"
#include "../Config/Config.h"

class Skills : public cocos2d::Scene
{
public:
	virtual bool init(Config* config);
	static Skills* create(Config* config);
	static Json::Value getSkillFromID(int id);
	static Json::Value getSavedSkillFromID(int id);
	virtual void update(float dt);
	void updateTutorial(float dt);

protected:
	void hideValidationPanel();
	void showValidationPanel();
	int getSavedSkillPosFromID(int id);
	void setTalentBtnBought(cocos2d::ui::Button* btn);
	void selectSkill(int id);
	void initTalents();
	virtual void onEnterTransitionDidFinish();

protected:
	bool tutorial_running;
	cocos2d::ui::Button* c_button;
	Json::Value c_talent;
	std::map<int, cocos2d::ui::Button*> talents_button;
	Config* configClass;

};
#endif