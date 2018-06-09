#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "cocos2d.h"
#include "Scenes/SceneFactory.h"
#include "Lib/Loader.h"
#include <array>
#include "Lib/Translationable.h"
#include "Scenes/MyGame.h"
#include "Config/Config.h"

class SceneManager
{
public:

	//Constructor & destructor
	SceneManager(Config* config);
	~SceneManager();

	void startGameWithLevel(int worldID, int levelID);

	//Add a scene
	void setScene(SceneFactory::SceneType type);

	MyGame* getGame();
	SceneFactory::SceneType getCurrentSceneIndex();

	static SceneManager * createInstance(Config * config);

	static SceneManager* getInstance();

protected:
	void replaceSceneWithCurrentScene(SceneFactory::SceneType type);
	void switchMusic(SceneFactory::SceneType type);
	void addSceneTransitionTrackingEvent(SceneFactory::SceneType type);
	
private:
	static SceneManager *manager;
	Config* configClass;
	cocos2d::Scene* currentscene;
	int c_index;
};

#endif
