#include "MainMenu.h"
#include "../SceneManager.h"
#include "../AppDelegate.h"

USING_NS_CC;


bool MainMenu::init()
{
	if(!Scene::init()){ return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	/*loading sprites, setting position, scaling for main menu*/

	Sprite* loadingBackground = Sprite::create("res/background/crissXcross.png");
	loadingBackground->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	loadingBackground->setScale(visibleSize.width / loadingBackground->getContentSize().width);
	addChild(loadingBackground);

	Sprite* bglogo = Sprite::create("res/background/logo.png");
	addChild(bglogo);
	bglogo->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	bglogo->setScale((visibleSize.width / 2) / bglogo->getContentSize().width);
	bglogo->setOpacity(0.0);

	Sprite* bg1 = Sprite::create("res/background/menu_background1.png");
	addChild(bg1);
	bg1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	bg1->setScale((visibleSize.width) / bg1->getContentSize().width);
	bg1->setOpacity(0.0);
	
	Sprite* mallowstxt = Sprite::create("res/background/mallowstextgreen.png");
	addChild(mallowstxt,2);
	mallowstxt->setPosition(visibleSize.width / 2, visibleSize.height / 1.15);
	mallowstxt->setOpacity(0.0);
	float coeff1 = visibleSize.width*0.5 / mallowstxt->getContentSize().width;

	Sprite* dangotxt = Sprite::create("res/background/dangotext2.png");
	addChild(dangotxt,2);
	dangotxt->setPosition(visibleSize.width / 2, visibleSize.height / 1.55);
	dangotxt->setOpacity(0.0);
	float coeff2 = visibleSize.width*0.35 / dangotxt->getContentSize().width;

	Sprite* vs = Sprite::create("res/background/vs_bubble.png");
	addChild(vs,1);
	vs->setPosition(-visibleSize.width / 4, visibleSize.height * (9.0 / 12.0));
	vs->setScale((visibleSize.width)/7 / vs->getContentSize().width);
	vs->setOpacity(0.0);

	Sprite* flash = Sprite::create("res/background/flash.png");
	addChild(flash,3);
	flash->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	flash->setOpacity(0.0);

	Sprite* bg2 = Sprite::create("res/background/menu_background_battle.png");
	addChild(bg2);
	bg2->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	bg2->setScale((visibleSize.width) / bg2->getContentSize().width);
	bg2->setOpacity(0.0);

	cocos2d::ui::Button* button = ui::Button::create();
	button->setTitleText("Start");
	button->setTitleColor(Color3B::YELLOW);
	button->setTitleFontName("fonts/LICABOLD.ttf");
	button->setTitleFontSize(75.f* visibleSize.width / 960);
	button->setPosition(Vec2(visibleSize.width/2, visibleSize.height/4));
	button->setOpacity(0.0);
	button->addTouchEventListener(CC_CALLBACK_2(MainMenu::menuContinueCallback, this));

	cocos2d::ui::Layout* layout = ui::Layout::create();
	addChild(layout);
	layout->addChild(button);

	/*actions of sprites, fadein/out, flash*/

	TargetedAction* fadein = TargetedAction::create(bglogo, FadeIn::create(1.0f));
	TargetedAction* delay = TargetedAction::create(bglogo, DelayTime::create(1.0f));
	TargetedAction* fadeout = TargetedAction::create(bglogo, FadeOut::create(1.0f));

	TargetedAction* fadeinbg1 = TargetedAction::create(bg1, FadeIn::create(1.0f));
	TargetedAction* fadeintxt1 = TargetedAction::create(mallowstxt, Spawn::createWithTwoActions(FadeIn::create(1.0f), EaseElasticOut::create(ScaleTo::create(1.0f,coeff1))));
	TargetedAction* fadeintxt2 = TargetedAction::create(dangotxt, Spawn::createWithTwoActions(FadeIn::create(1.0f), EaseElasticOut::create(ScaleTo::create(1.0f, coeff2))));


	TargetedAction* fadeinvs = TargetedAction::create(vs, FadeIn::create(0.3f));
	TargetedAction* actionvs = TargetedAction::create(vs, Spawn::createWithTwoActions(MoveTo::create(1.0f, Vec2(visibleSize.width * (8.0/12.0), visibleSize.height * (9.0 / 12.0))), RotateTo::create(1.0f, 720)));

	TargetedAction* fadeinf = TargetedAction::create(flash, FadeIn::create(0.f));
	TargetedAction* fadeoutf1 = TargetedAction::create(flash, FadeOut::create(0.3f));
	TargetedAction* fadeinf1 = TargetedAction::create(flash, FadeIn::create(0.3f));

	TargetedAction* fadeinbg2 = TargetedAction::create(bg2, FadeIn::create(0.0f));
	Spawn* actionbg2 = Spawn::createWithTwoActions(fadeoutf1, fadeinbg2);

	TargetedAction* fadeinbutton = TargetedAction::create(button, FadeIn::create(1.0f));

	
	/*action sequence*/
	auto sequence = Sequence::create(fadein, delay, fadeout, fadeinbg1, fadeintxt1, fadeintxt2, fadeinvs, actionvs, fadeinf, fadeoutf1, fadeinf, fadeoutf1, fadeinf, actionbg2, fadeinbutton, nullptr);
	bglogo->runAction(sequence);

	return true;
	
}

void MainMenu::update(float dt){

}

void MainMenu::menuContinueCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		SceneManager::getInstance()->setScene(SceneManager::LEVELS);
	}
}


void MainMenu::onEnter(){
	Scene::onEnter();
}

void MainMenu::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
	scheduleUpdate();
}
