#ifndef SKILLS_HPP
#define SKILLS_HPP

#include "cocos2d.h"

class Skills : public cocos2d::Scene
{
public:
	virtual bool init();
	CREATE_FUNC(Skills);



private:
	void setSkill(int tier_id, int skill_id);
	void setDependancy(int tier_id, int skill_id);


};
#endif