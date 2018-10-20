#pragma once
#include "WaterBall.h"

class WaterBombBall : public WaterBall {
public:
	WaterBombBall(Dango* ntarget, double ndamages, double nspeed, double nrange);
	virtual ~WaterBombBall();
	static WaterBombBall* create(Dango* ntarget, double damages, double nspeed, double range);

	virtual void applyDamagesToTarget();
	virtual bool affectEnemy(Dangobese* enemy);

protected:
	double range;
};