#include "MyGame.h"
#include "AppDelegate.h"

USING_NS_CC;

MyGame::MyGame() : touch(true), paused(false), launched(false), reloading(false), acceleration(1.0){}

Scene* MyGame::scene()
{
	// 'scene' is an autorelease object
	//CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	MyGame *layer = MyGame::create();

	// add layer as a child to scene
	//scene->addChild(layer);

	// return the scene
	return layer;
	//    return scene;
}

MyGame::~MyGame()
{
}

// on "init" you need to initialize your instance
bool MyGame::init()
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

bool MyGame::initLevel(int level_id){
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

void MyGame::onEnterTransitionDidFinish(){
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


void MyGame::menuCloseCallback(Ref* pSender)
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

void MyGame::update(float delta){
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

Level* MyGame::getLevel(){
	return cLevel;
}

void MyGame::reload(){
	cLevel->reset();
	menu->reset();
	acceleration = 1.0;
}

void MyGame::pause(){
	Node::pause();
	for(auto child: getChildren()){
		child->pause();
	}
	paused = true;
}

void MyGame::resume(){
	Node::resume();
	for(auto child: getChildren()){
		child->resume();
	}
	paused = false;
}

bool MyGame::isPaused(){
	return paused;
}

bool MyGame::isLaunched(){
	return launched;
}

void MyGame::setReloading(bool nreloading){
	reloading = nreloading;
}

void MyGame::increaseSpeed(){
	acceleration = 4.0;
}
void MyGame::setNormalSpeed(){
	acceleration = 1.0;
}

bool MyGame::isAccelerated(){
	return acceleration != 1.0;
}

bool MyGame::save(){
	Json::Value root = ((AppDelegate*)Application::getInstance())->getSave();
	if(root["level"].asInt() < cLevel->getLevelId()+1){
		root["level"] = cLevel->getLevelId()+1;
	}
	root["exp"] = experience;
	((AppDelegate*)Application::getInstance())->getConfigClass().setSave(root);
	((AppDelegate*)Application::getInstance())->getConfigClass().save();
	return true;
}

bool MyGame::load(){
	experience = ((AppDelegate*)Application::getInstance())->getSave()["exp"].asInt();
	return true;
}
