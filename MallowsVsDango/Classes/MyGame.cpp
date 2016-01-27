#include "MyGame.h"
#include "AppDelegate.h"



USING_NS_CC;

MyGame::MyGame() : Loader(), touch(true), paused(false), reloading(false), acceleration(1.0),id_level(0){}

Scene* MyGame::scene()
{
	// 'layer' is an autorelease object
	MyGame *layer = MyGame::create();
	// return the scene
	return layer;
}

MyGame::~MyGame()
{
}

// on "init" you need to initialize your instance
bool MyGame::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//////////////////////////////
	// 1. super init first
	if (!CCScene::init())
	{
		return false;
	}
	
	cLevel = nullptr;
	menu = nullptr;
	load();

	loadingScreen = LoadingScreen::create();
	addChild(loadingScreen,3);

	return true;
}

bool MyGame::initLevel(int level_id){
	reset();
	if(cLevel != nullptr){
		removeChild(cLevel,1);
	}
	if(menu != nullptr){
		menu->setVisible(false);
	}

	id_level = level_id;
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["levels"][level_id];
	for(unsigned int i(0); i < config["animations"].size(); ++i){
		addAnimation(config["animations"][i].asString());
	}
	for(unsigned int i(0); i < config["musics"].size(); ++i){
		addMusic(config["musics"][i].asString());
	}
	for(unsigned int i(0); i < config["effects"].size(); ++i){
		addEffect(config["effects"][i].asString());
	}
	for(unsigned int i(0); i < config["tileset"].size(); ++i){
		addTileset(config["tileset"].asString());
	}
	unload();
	//schedule(CC_SCHEDULE_SELECTOR(MyGame::update));
	schedule(CC_SCHEDULE_SELECTOR(MyGame::updateLoading));

	loadingScreen->setVisible(true);
	loadingScreen->start();
	return true;
}

void MyGame::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
}
void MyGame::onExitTransitionDidStart(){
	Scene::onExitTransitionDidStart();
	unload();
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->removeUnusedSpriteFrames();
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
			int new_level_id = cLevel->getLevelId() + 1;
			removeChild(cLevel,1);
			cLevel = Level::create(new_level_id);
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
	if(root["level"].asInt() < (int)cLevel->getLevelId()+1){
		root["level"] = cLevel->getLevelId()+1;
	}
	root["exp"] = experience;
	((AppDelegate*)Application::getInstance())->getConfigClass().setSave(root);
	((AppDelegate*)Application::getInstance())->getConfigClass().save();
	return true;
}

bool MyGame::load(){
	// load experience saved from .sav file
	experience = ((AppDelegate*)Application::getInstance())->getSave()["exp"].asInt();
	return true;
}

void MyGame::initAttributes(){
	// create new level and add it to the game
	cLevel = Level::create(id_level);
	addChild(cLevel,0);

	/* create new interface in case this one hasn't been created and add it to the game.
	 * If already created, reset it to initial parameters.
	 */
	if(menu == nullptr){
		menu = InterfaceGame::create(this);
		addChild(menu,2);
	}
	else{
		menu->reset();
	}

	// reset attributes
	acceleration = 1.0;
	paused = false;

	// call the function of the mother class
	Loader::initAttributes();

	unschedule(CC_SCHEDULE_SELECTOR(MyGame::updateLoading));
	scheduleUpdate();

	// set interface visible and listening to input (touch)
	loadingScreen->setLoadingPercent(100);
	FadeOut* fade = FadeOut::create(0.5f);
	Hide* hide = Hide::create();
	loadingScreen->runAction(Sequence::create(fade,hide,nullptr));
	loadingScreen->stop();
	menu->setVisible(true);
	menu->setListening(true);
}

void MyGame::updateLoading(float dt){
	/* PUTAIN DE COCOS2D DE MERDE. Si tu veux comprendre ici, contacte moi...
	 * c'est trop compliqué par écrit.
	 */
	Loader::loading(dt);
	loadingScreen->setLoadingPercent(getProgress()*100);
	// update loading bar to the exact percentage

}
