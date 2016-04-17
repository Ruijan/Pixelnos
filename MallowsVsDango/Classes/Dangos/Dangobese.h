#ifndef DANGOBESE_HPP
#define DANGOBESE_HPP

#include "cocos2d.h"
#include "../Config/json.h"
#include "Dango.h"


class Cell;
class Attack;

class Dangobese : public Dango{

public:
	Dangobese(std::vector<Cell*> npath, int nlevel);
	virtual ~Dangobese();
	static Dangobese* create(std::vector<Cell*> npath, int nlevel);

	static Json::Value getConfig();
	Json::Value getSpecConfig();
	virtual void update(float dt);
	virtual bool shouldAttack();
	virtual void attack(float dt);
	virtual void runAnimation(cocos2d::Animation* anim);

	bool isAffectedByAttack(Attack* attack);

private:
	double attack_duration;
	double attack_timer;
};

#endif
