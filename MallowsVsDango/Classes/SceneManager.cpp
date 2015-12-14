#include "SceneManager.h"
#include "MainMenu.h"
#include "StoryMenu.h"
USING_NS_CC;

SceneManager *SceneManager::manager;

SceneManager::SceneManager(){
	cacheScene[MENU] = MainMenu::create();
	cacheScene[MENU]->retain();
	cacheScene[GAME] = MvDGame::create();
	cacheScene[GAME]->retain();
	cacheScene[LEVELS] = StoryMenu::create();
	cacheScene[LEVELS]->retain();

	currentscene = cacheScene[MENU];
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

MvDGame* SceneManager::getGame(){
	return  ((MvDGame*)cacheScene[GAME]);
}
