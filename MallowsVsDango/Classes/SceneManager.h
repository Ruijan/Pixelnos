#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "cocos2d.h"
#include "Scenes/MainMenu.h"
#include "Scenes/StoryMenu.h"
#include "Scenes/CreditScreen.h"
#include "Scenes/LevelEditor.h"
#include "Scenes/Skills.h"
#include "Scenes/MyGame.h"
#include "Scenes/Shop.h"
#include "Lib/Loader.h"
#include <array>
#include "Lib/Translationable.h"

class MyGame;

class SceneManager: public Translationable
{
public:
	// Define type of scene
	enum SceneType
	{
		MENU = 0,
		GAME = 1,
		LEVELS = 2,
		LOADING = 3,
		CREDIT = 4,
		EDITOR = 5,
		SKILLS = 6,
		SHOP = 7,
		PAUSE = 8,
		STOP = 9,
		START = 10
	};

	//Constructor & destructor
	SceneManager();
	~SceneManager();

	//Add a scene
	void setScene(SceneManager::SceneType type);
	MyGame* getGame();
	void switchLanguage();
	SceneManager::SceneType getCurrentSceneIndex();

	// Create a new Instance of SceneManager*
	static SceneManager* getInstance()
	{
		if (!manager)
			manager = new SceneManager();
		return manager;
	}
	
	
private:
	static SceneManager *manager;
	std::array<cocos2d::Scene*,8> cacheScene;
	cocos2d::Scene* currentscene;
	int c_index;
};

#endif
