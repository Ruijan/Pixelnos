#include "Game.h"
#include "AppDelegate.h"
#include "Lib/FadeMusic.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;

MvDGame::MvDGame() : touch(true), paused(false), launched(false), reloading(false), acceleration(1.0){}

Scene* MvDGame::scene()
{
	// 'scene' is an autorelease object
	//CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	MvDGame *layer = MvDGame::create();

	// add layer as a child to scene
	//scene->addChild(layer);

	// return the scene
	return layer;
	//    return scene;
}

MvDGame::~MvDGame()
{
}

// on "init" you need to initialize your instance
bool MvDGame::init()
{
	//////////////////////////////
	// 1. super init first
	if (!CCScene::init())
	{
		return false;
	}
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("res/turret/animations/archer.plist", "res/turret/animations/archer.png");
	cache->addSpriteFramesWithFile("res/turret/animations/cutter.plist", "res/turret/animations/cutter.png");
	
	cLevel = nullptr;
	menu = nullptr;
	load();

	return true;
}

bool MvDGame::initLevel(int level_id){
	removeChild(cLevel,1);
	cLevel = Level::create(level_id);
	addChild(cLevel,0);
	if(menu == nullptr){
		menu = InterfaceGame::create(this);
		addChild(menu,2);
	}
	else{
		menu->reset();
	}
	acceleration = 1.0;
	return true;
}

void MvDGame::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
	scheduleUpdate();
	menu->setListening(true);
	if (!launched){
		launched = true;
	}
	else{
		//pause();
	}
}

void MvDGame::update(float delta){

void Game::onExitTransitionDidStart(){
	Scene::onExitTransitionDidStart();
}


	Scene::update(delta);
	menu->update(delta);
	cLevel->update(delta * acceleration);
	if (cLevel->hasLost() && !cLevel->isPaused()){
		menu->showLoose();
		cLevel->pause();
	}
	else if (cLevel->hasWon() && !cLevel->isPaused()){
		menu->showWin();
		cLevel->pause();
		experience += cLevel->getTotalExperience();
		save();
	}
	if (reloading){
		reload();
		reloading = false;
	}
	if(menu->getState() == InterfaceGame::State::NEXT_LEVEL){
		Json::Value levels = ((AppDelegate*)Application::getInstance())->getConfig()["levels"];
		if(levels.size() > cLevel->getLevelId() + 1){
			removeChild(cLevel,1);
			cLevel = Level::create(cLevel->getLevelId() + 1);
			menu->reset();
			acceleration = 1.0;
			addChild(cLevel,0);
		}
		else{
			SceneManager::getInstance()->setScene(SceneManager::LEVELS);
		}
	}
}

Level* MvDGame::getLevel(){
	return cLevel;
}

void MvDGame::reload(){
	cLevel->reset();
	menu->reset();
	acceleration = 1.0;
}

void MvDGame::pause(){
	Node::pause();
	for(auto child: getChildren()){
		child->pause();
	}
	paused = true;
}

void MvDGame::resume(){
	Node::resume();
	for(auto child: getChildren()){
		child->resume();
	}
	paused = false;
}

bool MvDGame::isPaused(){
	return paused;
}

bool MvDGame::isLaunched(){
	return launched;
}

void MvDGame::setReloading(bool nreloading){
	reloading = nreloading;
}

void MvDGame::increaseSpeed(){
	acceleration = 4.0;
}
void MvDGame::setNormalSpeed(){
	acceleration = 1.0;
}

bool MvDGame::isAccelerated(){
	return acceleration != 1.0;
}

bool MvDGame::save(){
	Json::Value root = ((AppDelegate*)Application::getInstance())->getSave();
	if(root["level"].asInt() < cLevel->getLevelId()+1){
		root["level"] = cLevel->getLevelId()+1;
	}
	root["exp"] = experience;
	((AppDelegate*)Application::getInstance())->getConfigClass().setSave(root);
	((AppDelegate*)Application::getInstance())->getConfigClass().save();
	return true;
}

bool MvDGame::load(){
	experience = ((AppDelegate*)Application::getInstance())->getSave()["exp"].asInt();
	return true;
}
