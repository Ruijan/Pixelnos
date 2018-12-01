#pragma once
#include "WaterBall.h"

class Level;

class WaterBombBall : public WaterBall {
public:
	WaterBombBall(Level* level, Dango* ntarget, double ndamages, double nspeed, double nrange);
	virtual ~WaterBombBall();
	static WaterBombBall* create(Level* level, Dango* ntarget, double damages, double nspeed, double range);

	virtual void applyDamagesToTarget();
	virtual bool affectEnemy(Dangobese* enemy);

protected:
	double range;
	Level* level;
};