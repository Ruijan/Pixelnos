#pragma once
#include "ThrowableAttack.h"

class WaterBall : public ThrowableAttack{
public:
	WaterBall(Dango* ntarget, double ndamages, double nspeed);
	virtual ~WaterBall();
	static WaterBall* create(Dango* ntarget, double damages, double nspeed);
	static WaterBall* createWithFile(std::string file, Dango* ntarget, double damages, double nspeed);

	virtual bool affectEnemy(Dangobese* enemy);
};