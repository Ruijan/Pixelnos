#include "SceneManager.h"
#include "MainMenu.h"
#include "StoryMenu.h"
USING_NS_CC;

SceneManager *SceneManager::manager;

SceneManager::SceneManager(){
	cacheScene[GAME] = Game::create();
	cacheScene[GAME]->retain();
	cacheScene[MENU] = MainMenu::create();
	cacheScene[MENU]->retain();
	cacheScene[LEVELS] = StoryMenu::create();
	cacheScene[LEVELS]->retain();

	currentscene = cacheScene[LEVELS];
	Director::getInstance()->runWithScene(currentscene);
}


SceneManager::~SceneManager(){
	for (Scene*& scene : cacheScene)
		scene->release();
}

void SceneManager::setScene(SceneManager::SceneType type){
	currentscene = cacheScene[type];
	TransitionFade* transition = TransitionFade::create(0.5f, currentscene);
	Director::getInstance()->replaceScene(transition);// , false));
}

Game* SceneManager::getGame(){
	return  ((Game*)cacheScene[GAME]);
}
