#include "SceneManager.h"
#include "Lib/Loader.h"
#include "AppDelegate.h"
#include "Config/Config.h"
#include "Config/json.h"
#include "Lib/FadeMusic.h"

USING_NS_CC;

SceneManager *SceneManager::manager;

SceneManager::SceneManager() {
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
	TrackingEvent c_event;
	c_event.from_scene = getStringFromSceneType(SceneManager::SceneType::START);
	c_event.to_scene = getStringFromSceneType(getCurrentSceneIndex());
	c_event.time = time(0);

	((AppDelegate*)Application::getInstance())->getConfigClass()->addTrackingEvent(c_event);
	Director::getInstance()->runWithScene(currentscene);
	if (((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GENERAL)["play_sound"].asBool()) {
		std::string music = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GENERAL)
			["sound_transition"][c_index][c_index].asString();
		auto action1 = ChangeMusic::create(music);
		auto action2 = FadeInMusic::create(0.5f);
		Director::getInstance()->getActionManager()->addAction(Sequence::create(action1, action2, nullptr), currentscene, false);
	}
}


SceneManager::~SceneManager() {
	for (Scene*& scene : cacheScene)
		scene->release();
}

void SceneManager::setScene(SceneManager::SceneType type) {
	TrackingEvent c_event;
	c_event.from_scene = getStringFromSceneType((SceneType)c_index);
	c_event.to_scene = getStringFromSceneType(type);
	c_event.time = time(0);

	((AppDelegate*)Application::getInstance())->getConfigClass()->addTrackingEvent(c_event);
	currentscene = cacheScene[type];
	TransitionFade* transition = TransitionFade::create(0.5f, currentscene);
	Director::getInstance()->replaceScene(transition);
	std::string music = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GENERAL)["sound_transition"][c_index][(int)type].asString();

	if (music != "none" && ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GENERAL)["play_sound"].asBool()) {
		auto action1 = FadeOutMusic::create(0.5f);
		auto action2 = ChangeMusic::create(music);
		auto action3 = FadeInMusic::create(0.5f);
		Director::getInstance()->getActionManager()->addAction(Sequence::create(action1, action2, action3, nullptr), currentscene, false);
	}
	c_index = (int)type;
}

MyGame* SceneManager::getGame() {
	return  ((MyGame*)cacheScene[GAME]);
}

void SceneManager::switchLanguage() {
	((MainMenu*)cacheScene[MENU])->switchLanguage();
}

SceneManager::SceneType SceneManager::getCurrentSceneIndex() {
	return (SceneManager::SceneType)c_index;
}

std::string SceneManager::getStringFromSceneType(SceneManager::SceneType type) {
	switch (type) {
	case SceneManager::SceneType::CREDIT:
		return "credit";
	case SceneManager::SceneType::LEVELS:
		return "levels";
	case SceneManager::SceneType::MENU:
		return "menu";
	case SceneManager::SceneType::GAME:
		return "game";
	case SceneManager::SceneType::SKILLS:
		return "skills";
	case SceneManager::SceneType::SHOP:
		return "shop";
	case SceneManager::SceneType::EDITOR:
		return "editor";
	case SceneManager::SceneType::LOADING:
		return "loading";
	case SceneManager::SceneType::PAUSE:
		return "pause";
	case SceneManager::SceneType::STOP:
		return "stop";
	case SceneManager::SceneType::START:
		return "start";
	default:
		return "error";
	}
}