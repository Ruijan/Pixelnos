#include "LoadingScreen.h"
#include "ui/CocosGUI.h"

USING_NS_CC;


LoadingScreen* LoadingScreen::create(){
	LoadingScreen* screen = new LoadingScreen();
	if (screen->init()){
		screen->autorelease();
		return screen;
	}

	CC_SAFE_DELETE(screen);
	return NULL;
}
LoadingScreen::LoadingScreen(){}

LoadingScreen::~LoadingScreen(){

}

// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
bool LoadingScreen::init(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//////////////////////////////
	// 1. super init first
	if (!LayerColor::initWithColor(Color4B(0,0,0,255)))
	{
		return false;
	}
	ui::LoadingBar* loadingBar = ui::LoadingBar::create("res/buttons/loaderProgress.png");
	loadingBar->setPercent(0);

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("res/dango/animations/dangobese1.plist", "res/dango/animations/dangobese1.png");

	Sprite* loadingBackground = Sprite::create("res/background/crissXcross.png");
	loadingBackground->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2));
	loadingBackground->setScale(visibleSize.width / loadingBackground->getContentSize().width);

	Sprite* image = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("dangobese1_j_000.png"));
	image->setPosition(visibleSize.width/2,visibleSize.height/2);

	Label* loading_label = Label::createWithTTF("Loading", "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 10.0));
	loading_label->setColor(Color3B::YELLOW);
	loading_label->setPosition(visibleSize.width/2,visibleSize.height/2 + image->getContentSize().height*3/4);

	// set the direction of the loading bars progress
	loadingBar->setDirection(ui::LoadingBar::Direction::LEFT);
	loadingBar->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2 - image->getContentSize().height*3/4));

	Sprite* loadingBarBackground = Sprite::create("res/buttons/loaderBackground.png");
	loadingBarBackground->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2 - image->getContentSize().height*3/4));

	addChild(loadingBackground,1,"background");
	addChild(loadingBarBackground,1,"loadingBackground");
	addChild(loadingBar,1,"loadingBar");
	addChild(image,1,"dangobese");
	addChild(loading_label,1,"loading_label");

	return true;
}

void LoadingScreen::start(){
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("res/dango/animations/dangobese1.plist", "res/dango/animations/dangobese1.png");
	cocos2d::Vector<SpriteFrame*> animFrames;
	char str[100] = { 0 };
	for (int i = 0; i < 24; ++i){
		sprintf(str, "dangobese1_j_%03d.png", i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 1.0f / 24.0f);
	getChildByName("dangobese")->runAction(RepeatForever::create(Animate::create(animation)));
}

void LoadingScreen::stop(){
	getChildByName("dangobese")->stopAllActions();
}
/*
 * Set Loading Percentage in % not in ratio.
 */
void LoadingScreen::setLoadingPercent(double percent){
	((ui::LoadingBar*)getChildByName("loadingBar"))->setPercent(percent);
}
