#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "cocos2d.h"
#include "MyGame.h"
#include "Lib/Loader.h"
#include <array>

class MyGame;

class SceneManager
{
public:
	// Define type of scene
	enum SceneType
	{
		MENU = 0,
		GAME = 1,
		LEVELS = 2,
		LOADING = 3,
		CREDIT = 4
	};

	//Constructor & destructor
	SceneManager();
	~SceneManager();

	//Add a scene
	void setScene(SceneManager::SceneType type);
	MyGame* getGame();

	// Create a new Instance of SceneManager*
	static SceneManager* getInstance()
	{
		if (!manager)
			manager = new SceneManager();
		return manager;
	}
	
	
private:
	static SceneManager *manager;
	std::array<cocos2d::Scene*,5> cacheScene;
	cocos2d::Scene* currentscene;
	int c_index;
};

#endif
