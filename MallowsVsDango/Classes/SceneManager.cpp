#include "SceneManager.h"
#include "MainMenu.h"
#include "StoryMenu.h"
#include "Lib/Loader.h"
#include "AppDelegate.h"
#include "Config/Config.h"
#include "Config/json.h"
#include "Lib/FadeMusic.h"
#include "CreditScreen.h"
#include "LevelEditor.h"
USING_NS_CC;

SceneManager *SceneManager::manager;

SceneManager::SceneManager(){
	MainMenu* menu = MainMenu::create();
	MyGame* game = MyGame::create();
	StoryMenu* story_menu = StoryMenu::create();
	CreditScreen* credit_screen = CreditScreen::create();
	LevelEditor* editor = LevelEditor::create();
	cacheScene[MENU] = menu;
	cacheScene[MENU]->retain();
	cacheScene[GAME] = game;
	cacheScene[GAME]->retain();
	cacheScene[LEVELS] = story_menu;
	cacheScene[LEVELS]->retain();
	cacheScene[CREDIT] = credit_screen;
	cacheScene[CREDIT]->retain();
	cacheScene[EDITOR] = editor;
	cacheScene[EDITOR]->retain();

	currentscene = cacheScene[LEVELS];
	c_index = 0;
	Director::getInstance()->runWithScene(currentscene);
	if(((AppDelegate*)Application::getInstance())->getConfig()["play_sound"].asBool()){
		std::string music = ((AppDelegate*)Application::getInstance())->getConfig()["sound_transition"][c_index][c_index].asString();
		auto action1 = ChangeMusic::create(music);
		auto action2 = FadeInMusic::create(0.5f);
		Director::getInstance()->getActionManager()->addAction(Sequence::create(action1,action2,nullptr),currentscene,false);
	}

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

	if(music != "none" && ((AppDelegate*)Application::getInstance())->getConfig()["play_sound"].asBool()){
		auto action1 = FadeOutMusic::create(0.5f);
		auto action2 = ChangeMusic::create(music);
		auto action3 = FadeInMusic::create(0.5f);
		Director::getInstance()->getActionManager()->addAction(Sequence::create(action1, action2, action3, nullptr), currentscene, false);
	}
	c_index = (int)type;
}

MyGame* SceneManager::getGame(){
	return  ((MyGame*)cacheScene[GAME]);
}
