#ifndef __GAME_H__
#define __GAME_H__

#include "cocos2d.h"
#include "Level/Level.h"
#include "../Level/InterfaceGame.h"
#include "Lib/Loader.h"
#include "LoadingScreen.h"

struct LevelTrackingEvent {
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
class MyGame : public cocos2d::Scene, public Loader
{

private:
	int id_level;
	int id_world;
	Level* cLevel;
	InterfaceGame* menu;
	LoadingScreen* loadingScreen;
	double acceleration; // not used right now.
	bool paused;
	bool reloading;
	int experience;
	LevelTrackingEvent l_event;

protected:
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();
	void reload();

	/**
	* @brief Inititialize the class by loading a level the configuration file.
	* Loads images and music, initialize the interface (or reset it).
	* @return If everything has been initialized correctly, return true.
	*/
	void initAttributes();

	/**
	* @brief Call the function Loader::loading() because the compiler in windows
	* mess up with daughter class and mother class for scheduling function call.
	* @param time step in seconds.
	* @warning Do not call from another function. Can only be call from a schedule
	* event.
	*/
	void updateLoading(float dt);

public:
	/**
	 * @brief Constructor of the class, initialize attribute to 0 or nullptr
	 */
	MyGame();
	/**
	 * @brief Destructor of the class, nothing to destroy in particular, nothing has been allocated
	 */
	virtual ~MyGame();

	/**
	 * @brief Inititialize the class by loading the parameters of the game: experience 
	 * collected until now.
	 * @return If everything has been initialized correctly, return true.
	 */	
	virtual bool init();
	/**
	 * @brief Inititialize the class by loading a level the configuration file.
	 * Loads images and music and reset the interface if needed.
	 * @param ID of the level as can be found in the file config.json
	 * @return If everything has been initialized correctly, return true.
	 */
	virtual bool initLevel(int level_id, int world_id);
	/**
	* @brief Update the game. Call the update of the interface and the update of
	* the level. Handle the end of a level and save parameters at the end.
	* @param Time step in second.
	*/
	virtual void update(float delta);


	// a selector callback
	void setReloading(bool nreloading);

	/**
	* @brief Pause the game (all the children too)
	*/
	virtual void pause();
	/**
	* @brief Resume the game (all the children too)
	*/
	virtual void resume();
	/**
	* @brief If the game is in pause mode return true.
	* @return If the game is in pause mode return true else return false.
	*/
	bool isPaused();
	/**
	* @brief Increment the speed to 4 time the normal speed. Increase the dt. 
	* Does not work on animation yet.
	*/
	void increaseSpeed();
	/**
	* @brief Decrease the speed to one time dt.
	*/
	void setNormalSpeed();
	/**
	* @brief Check if the game is accelerated
	* @return Check if the game is accelerated
	*/
	bool isAccelerated();

	/**
	* @brief Save the new experience and the progression of the game.
	* @return If everything is okay, return true
	*/
	bool save();
	
	/**
	* @brief Get a pointer to the current level. Useful for get information
	* about it.
	* @return Pointer of the current level.
	*/
	Level* getLevel();

	/**
	* @brief Get a pointer to the current interface. Useful for get information
	* about it.
	* @return Pointer of the current interface.
	*/
	InterfaceGame* getMenu();

	void addActionToTracker(Json::Value action);
	void createNewTracker();
	void updateTracker(int holy_sugar, std::string state, int duration);

	// implement the "static node()" method manually
	CREATE_FUNC(MyGame);
};

#endif // __Game_SCENE_H__
