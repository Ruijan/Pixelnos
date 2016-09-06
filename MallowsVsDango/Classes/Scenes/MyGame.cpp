#include "MyGame.h"
#include "../AppDelegate.h"



USING_NS_CC;

MyGame::MyGame() : Loader(), paused(false), reloading(false), 
acceleration(1.0), id_level(0), cLevel(nullptr), menu(nullptr){
}

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

bool MyGame::initLevel(int level_id, int world_id){
	// reset the game just in case.
	reset();
	// remove the level and the interface if it is initialized.
	if(cLevel != nullptr){
		removeChild(cLevel,1);
		cLevel = nullptr;
	}
	if(menu != nullptr){
		menu->setVisible(false);
	}

	id_level = level_id;
	id_world = world_id;
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["worlds"][id_world]["levels"][id_level];
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

void MyGame::update(float delta) {
	//update the scene, the interface and the level.
	Scene::update(delta);
	menu->update(delta);
	
	// If the player lost, we show the losing screen
	if (cLevel->hasLost() && !cLevel->isPaused()){
		menu->showLose();
		cLevel->pause();
		updateTracker(cLevel->getHolySugar(), "lost", time(0));
	}
	
	// In case of reloading
	if (reloading){
		reload();
		createNewTracker();
		reloading = false;
	}
	// If it has been asked to go to the next level then we delete the current level,
	// we reset the interface and the parameters and let's go !
	if (menu->getGameState() == InterfaceGame::GameState::TITLE) {
		cLevel->updateTowers(delta);
		cLevel->removeElements();
	}
	else if(menu->getGameState() == InterfaceGame::GameState::NEXT_LEVEL){
		Json::Value worlds = ((AppDelegate*)Application::getInstance())->getConfig()["worlds"][id_world];
		Json::Value levels = worlds["levels"];
		if(levels.size() > cLevel->getLevelId() + 1){
			int new_level_id = cLevel->getLevelId() + 1;
			removeChild(cLevel,1);
			cLevel = Level::create(new_level_id, id_world);
			menu->reset();
			menu->setListening(true);
			acceleration = 1.;0;
			addChild(cLevel,0);
			createNewTracker();
		}
		else{
			SceneManager::getInstance()->setScene(SceneManager::LEVELS);
		}
	}
	else if (menu->getGameState() == InterfaceGame::GameState::RUNNING) {
		cLevel->update(delta * acceleration);
		// update tracking event of the level
		if (cLevel->getHolySugar() != l_event.holy_sugar) {
			updateTracker(cLevel->getHolySugar(), "running", time(0));
		}

	}
	else if (menu->getGameState() == InterfaceGame::GameState::DONE && !cLevel->isPaused()) {
		menu->showWin();
		cLevel->pause();
		save();
		updateTracker(cLevel->getHolySugar(), "completed", time(0) - l_event.time);
	}
}

Level* MyGame::getLevel(){
	return cLevel;
}

InterfaceGame* MyGame::getMenu() {
	return menu;
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
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig();

	if(root["c_level"].asInt() < (int)cLevel->getLevelId()+1){
		root["c_level"] = cLevel->getLevelId()+1;
	}
	root["holy_sugar"] = root["holy_sugar"].asInt() + cLevel->getHolySugar();

	for (unsigned int i(0); i < root["towers"].getMemberNames().size(); ++i) {
		std::string tower_name = root["towers"].getMemberNames()[i];
		if (root["towers"][tower_name]["unlocked"].asBool()) {
			root["towers"][tower_name]["exp"] = root["towers"][tower_name]["exp"].asInt() + cLevel->getTowerXP(tower_name) + cLevel->getTotalExperience();
			while (root["towers"][tower_name]["max_level"].asInt() < (int)config["towers"][tower_name]["xp_level"].size() &&
				root["towers"][tower_name]["exp"].asInt() > config["towers"][tower_name]["xp_level"][root["towers"][tower_name]["max_level"].asInt() + 1].asInt()) {
				root["towers"][tower_name]["exp"] = root["towers"][tower_name]["exp"].asInt() -
					config["towers"][tower_name]["xp_level"][root["towers"][tower_name]["max_level"].asInt() + 1].asInt();
				root["towers"][tower_name]["max_level"] = root["towers"][tower_name]["max_level"].asInt() + 1;
			}
		}
	}
	((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(root);
	((AppDelegate*)Application::getInstance())->getConfigClass()->save();
	((AppDelegate*)Application::getInstance())->getConfigClass()->setProgressionNeedSave(true);
	return true;
}

void MyGame::initAttributes(){
	// create new level and add it to the game
	cLevel = Level::create(id_level, id_world);
	addChild(cLevel,0);
	if (menu == nullptr) {
		menu = InterfaceGame::create(this);
		addChild(menu, 2);
	}
	else {
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
	createNewTracker();
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

void MyGame::addActionToTracker(Json::Value action) {
	l_event.actions.append(action);
	l_event.duration = time(0) - l_event.time;
	((AppDelegate*)Application::getInstance())->getConfigClass()->updateCurrentLevelTrackingEvent(l_event);
}

void MyGame::createNewTracker() {
	l_event.level_id = id_level;
	l_event.world_id = id_world;
	l_event.holy_sugar = cLevel->getHolySugar();
	l_event.state = "starting";
	l_event.duration = 0;
	l_event.actions = Json::Value();
	l_event.time = time(0);
	((AppDelegate*)Application::getInstance())->getConfigClass()->addLevelTrackingEvent(l_event);
}

void MyGame::updateTracker(int holy_sugar, std::string state, int c_time) {
	l_event.holy_sugar = holy_sugar;
	l_event.duration = c_time - l_event.time;
	l_event.state = state;
	((AppDelegate*)Application::getInstance())->getConfigClass()->updateCurrentLevelTrackingEvent(l_event);
}