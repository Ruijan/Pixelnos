#include "MyGame.h"
#include "AppDelegate.h"



USING_NS_CC;

MyGame::MyGame() : Loader(), paused(false), reloading(false), 
acceleration(1.0), id_level(0), cLevel(nullptr), menu(nullptr)
{}

MyGame::~MyGame()
{
}

// on "init" you need to initialize your instance
bool MyGame::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	// 1. super init first of the Mother Fucking Class
	if (!CCScene::init())
	{
		return false;
	}

	// we have to do it here and not in the constructor because the scene has to be
	// initialize before
	loadingScreen = LoadingScreen::create();
	addChild(loadingScreen,3);

	return true;
}

bool MyGame::initLevel(int level_id){
	// reset the game just in case.
	reset();
	// remove the level and the interface if it is initialized.
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

void MyGame::update(float delta){

	//update the scene, the interface and the level.
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
		save();
	}
	if (reloading){
		reload();
		reloading = false;
	}
	// If it has been asked to go to the next level then we delete the current level,
	// we reset the interface and the parameters and let's go !
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
	unsigned int experience = ((AppDelegate*)Application::getInstance())->getSave()["exp"].asInt();
	root["exp"] = experience + cLevel->getTotalExperience();
	((AppDelegate*)Application::getInstance())->getConfigClass().setSave(root);
	((AppDelegate*)Application::getInstance())->getConfigClass().save();
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

	// call the function of the mother class to initialize attributes
	Loader::initAttributes();

	// unschedule the update of the loading because it is done
	unschedule(CC_SCHEDULE_SELECTOR(MyGame::updateLoading));
	// Let's the game begin: we schedule the update of MyGame
	scheduleUpdate();

	// Before, set interface visible and listening to input (touch)
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
	 * Ok... On se calme: d'abord, cocos2d ne veut pas comprendre la virtualite
	 * lorsqu'il s'agit de scheduling. J'ai du donc creer une autre fonction qui
	 * fait appel a la fonction de la classe mere...stupide...
	 */
	Loader::loading(dt);
	loadingScreen->setLoadingPercent(getProgress()*100);
	// update loading bar to the exact percentage
}
