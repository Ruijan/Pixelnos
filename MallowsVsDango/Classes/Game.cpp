#include "Game.h"
#include "AppDelegate.h"

USING_NS_CC;

Game::Game() : touch(true), paused(false), launched(false), reloading(false), acceleration(1.0){}

Scene* Game::scene()
{
	// 'scene' is an autorelease object
	//CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	Game *layer = Game::create();

	// add layer as a child to scene
	//scene->addChild(layer);

	// return the scene
	return layer;
	//    return scene;
}

Game::~Game()
{
}

// on "init" you need to initialize your instance
bool Game::init()
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

bool Game::initLevel(int level_id){
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

void Game::onEnterTransitionDidFinish(){
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


void Game::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
#else
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif
}

void Game::update(float delta){
	Scene::update(delta);
	menu->update(delta);
	cLevel->update(delta * acceleration);
	if (cLevel->hasLost()){
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

Level* Game::getLevel(){
	return cLevel;
}

void Game::reload(){
	cLevel->reset();
	menu->reset();
	acceleration = 1.0;
}

void Game::pause(){
	Node::pause();
	for(auto child: getChildren()){
		child->pause();
	}
	paused = true;
}

void Game::resume(){
	Node::resume();
	for(auto child: getChildren()){
		child->resume();
	}
	paused = false;
}

bool Game::isPaused(){
	return paused;
}

bool Game::isLaunched(){
	return launched;
}

void Game::setReloading(bool nreloading){
	reloading = nreloading;
}

void Game::increaseSpeed(){
	acceleration = 4.0;
}
void Game::setNormalSpeed(){
	acceleration = 1.0;
}

bool Game::isAccelerated(){
	return acceleration != 1.0;
}

bool Game::save(){
	Json::Value root = ((AppDelegate*)Application::getInstance())->getSave();
	if(root["level"].asInt() < cLevel->getLevelId()+1){
		root["level"] = cLevel->getLevelId()+1;
	}
	root["exp"] = experience;
	((AppDelegate*)Application::getInstance())->getConfigClass().setSave(root);
	((AppDelegate*)Application::getInstance())->getConfigClass().save();
	return true;
}

bool Game::load(){
	experience = ((AppDelegate*)Application::getInstance())->getSave()["exp"].asInt();
	return true;
}
