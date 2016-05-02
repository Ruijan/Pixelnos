#ifndef SKILLS_HPP
#define SKILLS_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Config/json.h"

class Skills : public cocos2d::Scene
{
public:
	virtual bool init();
	CREATE_FUNC(Skills);



private:
	void set_skill(int tier_id, int skill_id);
	void set_dependancy(int tier_id, int skill_id);
	void skill_update(int tier_id, int skill_id, cocos2d::ui::Button* skill);
	int selected_tier_id;
	int selected_skill_id;
	Json::Value config; //load data file
	Json::Value root; //load save file
};
#endif