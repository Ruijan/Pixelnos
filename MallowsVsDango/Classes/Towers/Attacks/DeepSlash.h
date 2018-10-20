#pragma once
#include "Slash.h"

class DeepSlash : public Slash {
public:
	DeepSlash(Dango* ntarget, double ndamages, double duration, double percent);
	virtual ~DeepSlash();
	static DeepSlash* create(Dango* ntarget, double damages, double duration, double percent);

	virtual bool affectEnemy(Dangobese* enemy);

protected:
	virtual void applyDamagesOnTarget();

protected:
	DeepWound * effect;
};