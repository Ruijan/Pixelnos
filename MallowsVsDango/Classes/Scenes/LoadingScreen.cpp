#include "LoadingScreen.h"
#include "ui/CocosGUI.h"
#include "../AppDelegate.h"


USING_NS_CC;


LoadingScreen* LoadingScreen::create() {
	LoadingScreen* screen = new LoadingScreen();
	if (screen->init()) {
		screen->autorelease();
		return screen;
	}

	CC_SAFE_DELETE(screen);
	return NULL;
}
LoadingScreen::LoadingScreen() {}

LoadingScreen::~LoadingScreen() {

}

// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
bool LoadingScreen::init() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();

	//////////////////////////////
	// 1. super init first
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 255)))
	{
		return false;
	}
	ui::LoadingBar* loadingBar = ui::LoadingBar::create("res/buttons/loaderProgress.png");
	loadingBar->setPercent(0);
	loadingBar->setScale(visibleSize.width / 2 / loadingBar->getContentSize().width);

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("res/dango/animations/dangobese1.plist", "res/dango/animations/dangobese1.png");

	Sprite* loadingBackground = Sprite::create("res/background/crissXcross.png");
	loadingBackground->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	loadingBackground->setScale(visibleSize.width / loadingBackground->getContentSize().width);

	SkeletonAnimation* skeleton = SkeletonAnimation::createWithJsonFile("res/dango/animations/dangobese.json",
		"res/dango/animations/dangobese.atlas", 0.5f * visibleSize.width / 1280);
	skeleton->setPosition(visibleSize.width / 2, visibleSize.height * 1 / 3);
	skeleton->setSkin("normal_1");


	Label* loading_label = Label::createWithTTF(
		((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON)["loading"][language].asString(),
		"fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 10.0));
	loading_label->setColor(Color3B::YELLOW);
	loading_label->setPosition(visibleSize.width / 2, visibleSize.height * 5 / 6);

	// set the direction of the loading bars progress
	loadingBar->setDirection(ui::LoadingBar::Direction::LEFT);
	loadingBar->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 6));

	Sprite* loadingBarBackground = Sprite::create("res/buttons/loaderBackground.png");
	loadingBarBackground->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 6));
	loadingBarBackground->setScale(visibleSize.width / 2 / loadingBarBackground->getContentSize().width);

	addChild(loadingBackground, 1, "background");
	addChild(loadingBarBackground, 1, "loadingBackground");
	addChild(loadingBar, 1, "loadingBar");
	addChild(skeleton, 1, "dangobese_sk");
	addChild(loading_label, 1, "loading_label");

	return true;
}

void LoadingScreen::start() {
	((SkeletonAnimation*)getChildByName("dangobese_sk"))->setAnimation(0, "jump_down", true);
}

void LoadingScreen::stop() {
	((SkeletonAnimation*)getChildByName("dangobese_sk"))->stopAllActions();
}
/*
 * Set Loading Percentage in % not in ratio.
 */
void LoadingScreen::setLoadingPercent(double percent) {
	((ui::LoadingBar*)getChildByName("loadingBar"))->setPercent(percent);
}

void LoadingScreen::switchLanguage() {

}