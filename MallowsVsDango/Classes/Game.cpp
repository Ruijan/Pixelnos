#include "Game.h"

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

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	cLevel = Level::create(Size(14, 12));
	menu = InterfaceGame::create(this);

	addChild(cLevel,0);
	addChild(menu,2);
	return true;
}

void Game::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
	scheduleUpdate();
	if (!launched){
		launched = true;
	}
	else{
		pause();
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
	else if (cLevel->hasWon()){
		menu->showWin();
		cLevel->pause();
	}
	if (reloading){
		reload();
		reloading = false;
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

void Game::createNew(){
	reload();
	launched = false;
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
