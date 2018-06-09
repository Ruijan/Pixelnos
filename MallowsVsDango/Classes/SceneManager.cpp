#include "SceneManager.h"
#include "Lib/Loader.h"
#include "AppDelegate.h"
#include "Config/Config.h"
#include "Config/json.h"
#include "Lib/FadeMusic.h"

USING_NS_CC;

SceneManager *SceneManager::manager;

SceneManager* SceneManager::getInstance()
{
	if (!manager)
		manager = new SceneManager();
	return manager;
}

SceneManager::SceneManager() {
	currentscene = SceneFactory::createScene(SceneFactory::SceneType::MENU);
	c_index = 0;
	TrackingEvent c_event;
	c_event.from_scene = SceneFactory::getStringFromSceneType(SceneFactory::SceneType::START);
	c_event.to_scene = SceneFactory::getStringFromSceneType(getCurrentSceneIndex());
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


SceneManager::~SceneManager() {}

void SceneManager::startGameWithLevel(int worldID, int levelID) {
	MyGame* game = MyGame::create();
	game->initLevel(levelID, worldID);
	currentscene = game;
	replaceSceneWithCurrentScene(SceneFactory::GAME);
}

void SceneManager::setScene(SceneFactory::SceneType type) {
	addSceneTransitionTrackingEvent(type);
	currentscene = SceneFactory::createScene(type);
	replaceSceneWithCurrentScene(type);
}

void SceneManager::replaceSceneWithCurrentScene(SceneFactory::SceneType type)
{
	TransitionFade* transition = TransitionFade::create(0.5f, currentscene);
	Director::getInstance()->replaceScene(transition);
	switchMusic(type);
	c_index = (int)type;
}

void SceneManager::switchMusic(SceneFactory::SceneType type)
{
	std::string music = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GENERAL)["sound_transition"][c_index][(int)type].asString();
	if (music != "none" && ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::GENERAL)["play_sound"].asBool()) {
		auto action1 = FadeOutMusic::create(0.5f);
		auto action2 = ChangeMusic::create(music);
		auto action3 = FadeInMusic::create(0.5f);
		Director::getInstance()->getActionManager()->addAction(Sequence::create(action1, action2, action3, nullptr), currentscene, false);
	}
}

void SceneManager::addSceneTransitionTrackingEvent(SceneFactory::SceneType type)
{
	TrackingEvent c_event;
	c_event.from_scene = SceneFactory::getStringFromSceneType((SceneFactory::SceneType)c_index);
	c_event.to_scene = SceneFactory::getStringFromSceneType(type);
	c_event.time = time(0);
	((AppDelegate*)Application::getInstance())->getConfigClass()->addTrackingEvent(c_event);
}

MyGame* SceneManager::getGame() {
	return  ((MyGame*)currentscene);
}

SceneFactory::SceneType SceneManager::getCurrentSceneIndex() {
	return (SceneFactory::SceneType)c_index;
}