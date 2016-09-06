#include "SceneManager.h"
#include "Lib/Loader.h"
#include "AppDelegate.h"
#include "Config/Config.h"
#include "Config/json.h"
#include "Lib/FadeMusic.h"

USING_NS_CC;

SceneManager *SceneManager::manager;

SceneManager::SceneManager(){
	MainMenu* menu = MainMenu::create();
	MyGame* game = MyGame::create();
	StoryMenu* story_menu = StoryMenu::create();
	CreditScreen* credit_screen = CreditScreen::create();
	LevelEditor* editor = LevelEditor::create();
	Skills* skills = Skills::create();
	Shop* shop = Shop::create();

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
	cacheScene[SKILLS] = skills;
	cacheScene[SKILLS]->retain();
	cacheScene[SHOP] = shop;
	cacheScene[SHOP]->retain();

	currentscene = cacheScene[MENU];
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
	TrackingEvent c_event;
	c_event.from_scene = Config::getStringFromSceneType((SceneType)c_index);
	c_event.to_scene = Config::getStringFromSceneType(type);
	c_event.time = time(0);

	((AppDelegate*)Application::getInstance())->getConfigClass()->addTrackingEvent(c_event);
	currentscene = cacheScene[type];
	TransitionFade* transition = TransitionFade::create(0.5f, currentscene);
	Director::getInstance()->replaceScene(transition);
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
