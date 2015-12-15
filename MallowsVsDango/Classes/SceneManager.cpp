#include "SceneManager.h"
#include "MainMenu.h"
#include "StoryMenu.h"
#include "AppDelegate.h"
#include "Config/Config.h"
#include "Config/json.h"
#include "Lib/FadeMusic.h"
USING_NS_CC;

SceneManager *SceneManager::manager;

SceneManager::SceneManager(){
	cacheScene[MENU] = MainMenu::create();
	cacheScene[MENU]->retain();
	cacheScene[GAME] = Game::create();
	cacheScene[GAME]->retain();
	cacheScene[LEVELS] = StoryMenu::create();
	cacheScene[LEVELS]->retain();

	currentscene = cacheScene[MENU];
	c_index = 0;
	Director::getInstance()->runWithScene(currentscene);

	std::string music = ((AppDelegate*)Application::getInstance())->getConfig()["sound_transition"][c_index][c_index].asString();
	auto action = ChangeMusic::create(music);
	Director::getInstance()->getActionManager()->addAction(action,currentscene,false);
}


SceneManager::~SceneManager(){
	for (Scene*& scene : cacheScene)
		scene->release();
}

void SceneManager::setScene(SceneManager::SceneType type){
	currentscene = cacheScene[type];

	TransitionFade* transition = TransitionFade::create(0.5f, currentscene);
	Director::getInstance()->replaceScene(transition);// , false));
	std::string music = ((AppDelegate*)Application::getInstance())->getConfig()["sound_transition"][c_index][(int)type].asString();
	log("musique : %s",music.c_str());
	if(music != "none"){
		auto action1 = FadeOutMusic::create(0.5f);
		auto action2 = ChangeMusic::create(music);
		Director::getInstance()->getActionManager()->addAction(Sequence::create(action1, action2, nullptr), currentscene, false);
	}
	c_index = (int)type;
}

Game* SceneManager::getGame(){
	return  ((Game*)cacheScene[GAME]);
}
