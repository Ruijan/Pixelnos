#pragma once
#include "Attack.h"


class Slash : public Attack {
public:
	Slash(Dango* ntarget, double ndamages);
	virtual ~Slash();
	static Slash* create(Dango* ntarget, double damages);

	virtual bool affectEnemy(Dangobese* enemy);

protected:
	void applyDamagesOnTarget();
	void updateTowerXP();
};