#include "LoadingScreen.h"


LoadingScreen* LoadingScreen::create(GUISettings * settings) {
	LoadingScreen* screen = new LoadingScreen(settings);
	if (screen->init()) {
		screen->autorelease();
		return screen;
	}

	CC_SAFE_DELETE(screen);
	return NULL;
}
LoadingScreen::LoadingScreen(GUISettings * settings): settings(settings){}

LoadingScreen::~LoadingScreen() {

}

// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
bool LoadingScreen::init() {
	cocos2d::Size visibleSize = settings->getVisibleSize();

	//////////////////////////////
	// 1. super init first
	if (!LayerColor::initWithColor(cocos2d::Color4B(0, 0, 0, 255)))
	{
		return false;
	}
	loadingBar = cocos2d::ui::LoadingBar::create("res/buttons/loaderProgress.png");
	loadingBar->setPercent(0);
	loadingBar->setScale(visibleSize.width / 2 / loadingBar->getContentSize().width);

	cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("res/dango/animations/dangobese1.plist", "res/dango/animations/dangobese1.png");

	cocos2d::Sprite* loadingBackground = cocos2d::Sprite::create("res/background/crissXcross.png");
	loadingBackground->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2));
	loadingBackground->setScale(visibleSize.width / loadingBackground->getContentSize().width);

	dangobeseSkeleton = spine::SkeletonAnimation::createWithJsonFile("res/dango/animations/dangobese.json",
		"res/dango/animations/dangobese.atlas", 0.5f * visibleSize.width / 1280);
	dangobeseSkeleton->setPosition(visibleSize.width / 2, visibleSize.height * 1 / 3);
	dangobeseSkeleton->setSkin("normal_1");


	cocos2d::Label* loading_label = cocos2d::Label::createWithTTF(settings->getButton("loading"),
		"fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 10.0));
	loading_label->setColor(cocos2d::Color3B::YELLOW);
	loading_label->setPosition(visibleSize.width / 2, visibleSize.height * 5 / 6);

	// set the direction of the loading bars progress
	loadingBar->setDirection(cocos2d::ui::LoadingBar::Direction::LEFT);
	loadingBar->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 6));

	cocos2d::Sprite* loadingBarBackground = cocos2d::Sprite::create("res/buttons/loaderBackground.png");
	loadingBarBackground->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 6));
	loadingBarBackground->setScale(visibleSize.width / 2 / loadingBarBackground->getContentSize().width);

	addChild(loadingBackground, 1, "background");
	addChild(loadingBarBackground, 1, "loadingBackground");
	addChild(loadingBar, 1, "loadingBar");
	addChild(dangobeseSkeleton, 1, "dangobese_sk");
	addChild(loading_label, 1, "loading_label");

	return true;
}

void LoadingScreen::start() {
	dangobeseSkeleton->setAnimation(0, "jump_down", true);
}

void LoadingScreen::stop() {
	dangobeseSkeleton->stopAllActions();
}
/*
 * Set Loading Percentage in % not in ratio.
 */
void LoadingScreen::setLoadingPercent(double percent) {
	loadingBar->setPercent(percent);
}

void LoadingScreen::switchLanguage() {

}