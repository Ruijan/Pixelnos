#include "Bullet.h"
#include "../Dangos/Dango.h"
#include <iostream>

USING_NS_CC;

Bullet::Bullet(Dango* ntarget, double ndamages, double nspeed, 
bool nrotate): target(ntarget), 
damages(ndamages), touched(false), speed(nspeed), rotate(nrotate){
	std::cerr << "bullet created" << std::endl;
}

Bullet::~Bullet(){
	
}

Bullet* Bullet::create(std::string image, Dango* ntarget, double damages, 
double nspeed, bool nrotate){
	Bullet* pSprite = new Bullet(ntarget, damages, nspeed, nrotate);

	if (pSprite->initWithFile(image))
	{
		pSprite->autorelease();
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void Bullet::update(float dt){
	if(target != nullptr && !touched){
		Vec2 direction = target->getPosition() - getPosition();
		double distance = sqrt(direction.x*direction.x + direction.y*direction.y);
		if(distance < 10){
			touched = true;
			target->takeDamages(damages);
		}
		else{
			direction.normalize();
			setPosition(getPosition() + direction * speed * dt);
			setRotation(getRotation() + 360 * dt);
		}
	}
	else{
		touched = true;
	}
}

bool Bullet::hasTouched(){
	return touched;
}

Dango* Bullet::getTarget(){
	return target;
}

void Bullet::setTarget(Dango* dango){
	target = dango;
}
