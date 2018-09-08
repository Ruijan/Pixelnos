#ifndef __GAME_H__
#define __GAME_H__

#include "cocos2d.h"
#include "Level/Level.h"
#include "../Level/Interface/LevelInterface.h"
#include "Lib/Loader.h"
#include "LoadingScreen.h"
#include "../Lib/Translationable.h"
#include "../Level/Tutorials/Tutorial.h"


struct LevelTrackingEvent {
	int level_id_bdd;
	int level_id;
	int world_id;
	std::string state;
	int holy_sugar;
	int duration;
	time_t time;
	Json::Value actions;
};

/** @class MyGame
 * @brief
 * Cocos2d Scene that handle a level, the interface and the different possible
 * actions from a game: load images, save progress, add experience and so on.
 */
class MyGame : public cocos2d::Scene, public Loader, public Translationable
{

private:
	int id_level;
	int id_world;
	Level* cLevel;
	LevelInterface* menu;
	LoadingScreen* loadingScreen;
	double acceleration; // not used right now.
	bool paused;
	bool reloading;
	int experience;
	LevelTrackingEvent l_event;
	std::vector<Tutorial*> tutorials;

protected:
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();
	void reload();
	void initAttributes();
	void loadTutorials();
	void updateLoading(float dt);

public:
	MyGame();
	virtual ~MyGame();

	virtual bool init();
	virtual bool initLevel(int level_id, int world_id);

	void removeTutorials();

	virtual void update(float delta);
	void switchLevel();
	void setReloading(bool nreloading);

	virtual void pause();
	virtual void resume();
	bool isPaused();
	bool isAccelerated();

	void increaseSpeed();
	void setNormalSpeed();
	float getAcceleration();

	bool save();
	
	Level* getLevel();
	LevelInterface* getMenu();

	void addActionToTracker(Json::Value action);
	void createNewTracker();
	void updateTracker(std::string state);
	virtual void switchLanguage();
	void unlockTowers();

	CREATE_FUNC(MyGame);
};

#endif // __Game_SCENE_H__
