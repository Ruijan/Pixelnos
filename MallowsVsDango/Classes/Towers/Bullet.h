#ifndef BULLET_HPP
#define BULLET_HPP

#include "cocos2d.h"

class Dango;

class Bullet : public cocos2d::Sprite{
	public:
	Bullet(Dango* ntarget, double ndamages, double nspeed, bool nrotate);
	virtual ~Bullet();
	static Bullet* create(std::string image, Dango* ntarget, double damages, double nspeed, bool nrotate);
	
	void update(float dt);
	bool hasTouched();
	bool isDone();
	Dango* getTarget();
	void setTarget(Dango* dango);
	void setOwner(std::string nowner);
	void startAnimation();
	
	private:
	Dango* target;
	double damages;
	bool touched;
	bool hasToBeDeleted;
	bool rotate;
	double speed;
	std::string owner;
	cocos2d::Action* action;
};

#endif
