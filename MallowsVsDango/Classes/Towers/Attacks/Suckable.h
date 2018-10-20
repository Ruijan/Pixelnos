#pragma once

class Suckable {
public:
	Suckable();
	bool isForceApplied();
	void setForceApplied(bool applied);
	void update();
protected:
	bool force_applied;
};