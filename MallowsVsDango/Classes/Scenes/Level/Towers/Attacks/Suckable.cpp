#include "Suckable.h"

Suckable::Suckable() : force_applied(false) {}

bool Suckable::isForceApplied() {
	return force_applied;
}

void Suckable::setForceApplied(bool applied) {
	force_applied = applied;
}

void Suckable::update() {
	force_applied = false;
}