#include "Bullet.h"

Bullet::Bullet(Dango* ntarget, double ndamages): target(ntarget), 
damages(ndamages){
}

Bullet::~Bullet(){
	
}

Bullet* Bullet::create(std::string image, Dango* ntarget, double damages){
	Bullet* pSprite = new Bullet(ntarget, damages);

	if (pSprite->initWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(image)))
	{
		
		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void Bullet::update(float dt){
	Vec2 direction = position - target->getPosition();
	direction.normalize();
	setPosition(position + direction * 5 * dt);
}
