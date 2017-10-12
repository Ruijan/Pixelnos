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
	void attackSpe(float dt);
	virtual void update(float dt);
	virtual void updateAnimation();
	virtual bool shouldAttackSpe();
	virtual void startAttackSpeAnimation();

	bool isAffectedByAttack(Attack* attack);

private:
	double attack_spe_duration;
	double attack_spe_timer;
	double attack_spe_reload_timer;
	double attack_spe_reload_time;
	bool attack_spe_done;
};

#endif
