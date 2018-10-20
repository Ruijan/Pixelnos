#pragma once
#include "WaterBall.h"


class ChocoSpit : public WaterBall{
public:
	ChocoSpit(Dango* ntarget, double damages, double slow, double duration, double nspeed);
	virtual ~ChocoSpit();
	static ChocoSpit* create(Dango* ntarget, double damages, double slow, double slow_duration, double nspeed);
	static ChocoSpit* createWithFile(std::string file, Dango* ntarget, double damages, double slow, double slow_duration, double nspeed);

	virtual void applyDamagesToTarget();
	virtual bool affectEnemy(Dangobese* enemy);
protected:
	float slow_percent;
	float slow_duration;
};