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
	bool hasTouched();
	Dango* getTarget();
	void setTarget(Dango* dango);
	
	private:
	Dango* target;
	double damages;
	bool touched;
	
};

#endif
