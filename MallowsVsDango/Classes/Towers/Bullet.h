#ifndef BULLET_HPP
#define BULLET_HPP

#include "cocos2d.h"

class Dango;

class Bullet : public cocos2d::Sprite{
	public:
	Bullet(Dango* ntarget, double ndamages);
	virtual ~Bullet();
	static Bullet* create(std::string image, Dango* ntarget, double damages);
	
	void update(float dt);
	
	private:
	Dango* target;
	double damages;
	
};
