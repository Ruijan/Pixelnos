#ifndef __GAME_H__
#define __GAME_H__

#include "cocos2d.h"
#include "Level/Level.h"
#include "InterfaceGame.h"



class Game : public cocos2d::Scene
{

private:
	bool touch;
	int nextLevel;
	Level* cLevel;
	InterfaceGame* menu;
	double acceleration;
	bool paused;
	bool launched;
	bool reloading;
	

protected:
	virtual void onEnterTransitionDidFinish();
	void reload();

public:
	Game();
	~Game();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* scene();

	virtual void update(float delta);


	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);
	void setReloading(bool nreloading);
	virtual void pause();
	virtual void resume();
	bool isPaused();
	bool isLaunched();
	void createNew();
	void increaseSpeed();
	void setNormalSpeed();
	bool isAccelerated();
	

	Level* getLevel();
	// implement the "static node()" method manually
	CREATE_FUNC(Game);



};

#endif // __Game_SCENE_H__
