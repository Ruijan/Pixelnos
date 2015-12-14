#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "cocos2d.h"
#include "Game.h"
#include <array>

class MvDGame;

class SceneManager
{
public:
	// Define type of scene
	enum SceneType
	{
		MENU = 0,
		GAME = 1,
		LEVELS = 2
	};

	//Constructor & destructor
	SceneManager();
	~SceneManager();

	//Add a scene
	void setScene(SceneManager::SceneType type);
	MvDGame* getGame();

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
};

#endif
