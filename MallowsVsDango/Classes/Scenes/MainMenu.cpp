#include "MainMenu.h"
#include "../SceneManager.h"
#include "../AppDelegate.h"

USING_NS_CC;


bool MainMenu::init()
{
	if(!Scene::init()){ return false; }
	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();

	/*loading sprites, setting position, scaling for main menu*/

	Sprite* loadingBackground = Sprite::create("res/background/crissXcross.png");
	loadingBackground->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	loadingBackground->setScale(visibleSize.width / loadingBackground->getContentSize().width);
	addChild(loadingBackground, 1, "logo_background");

	Sprite* bglogo = Sprite::create("res/background/logo.png");
	addChild(bglogo, 1 ,"logo");
	bglogo->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	bglogo->setScale((visibleSize.width / 2) / bglogo->getContentSize().width);
	bglogo->setOpacity(0.0);

	Sprite* bg1 = Sprite::create("res/background/menu_background1.png");
	addChild(bg1, 1, "first_background");
	bg1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	bg1->setScale((visibleSize.width) / bg1->getContentSize().width);
	bg1->setOpacity(0.0);

	Sprite* flash = Sprite::create("res/background/flash.png");
	addChild(flash, 3);
	flash->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	flash->setOpacity(0.0);

	Sprite* bg2 = Sprite::create("res/background/menu_background_battle.png");
	addChild(bg2,1,"last_background");
	bg2->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	bg2->setScale((visibleSize.width) / bg2->getContentSize().width);
	bg2->setOpacity(0.0);

	Sprite* vs = Sprite::create("res/background/vs_bubble.png");
	addChild(vs, 1, "vs");
	vs->setPosition(-visibleSize.width / 4, visibleSize.height * (9.0 / 12.0));
	vs->setScale((visibleSize.width) / 7 / vs->getContentSize().width);
	vs->setOpacity(0.0);

	Sprite* mallowstxt = Sprite::create("res/background/mallowstextgreen.png");
	addChild(mallowstxt, 2, "mallowstxt");
	mallowstxt->setPosition(visibleSize.width / 2, visibleSize.height / 1.15);
	mallowstxt->setOpacity(0.0);
	mallowstxt->setScale(visibleSize.width * 0.75 / mallowstxt->getContentSize().width);
	float coeff1 = visibleSize.width*0.5 / mallowstxt->getContentSize().width;

	Sprite* dangotxt = Sprite::create("res/background/dangotext2.png");
	addChild(dangotxt, 2, "dangotxt");
	dangotxt->setPosition(visibleSize.width / 2, visibleSize.height / 1.55);
	dangotxt->setOpacity(0.0);
	dangotxt->setScale(visibleSize.width * 0.75 / dangotxt->getContentSize().width);
	float coeff2 = visibleSize.width*0.35 / dangotxt->getContentSize().width;

	cocos2d::ui::Button* button = ui::Button::create();
	button->setTitleText(((AppDelegate*)Application::getInstance())->getConfig()
		["buttons"]["start"][language].asString());
	button->setTitleColor(Color3B::YELLOW);
	button->setTitleFontName("fonts/LICABOLD.ttf");
	button->setTitleFontSize(75.f* visibleSize.width / 960);
	button->setPosition(Vec2(visibleSize.width/2, visibleSize.height/4));
	button->setOpacity(0.0);
	Label* start_label = button->getTitleRenderer();
	start_label->enableOutline(Color4B::BLACK, 2.f * visibleSize.width / 1280);
	button->addTouchEventListener(CC_CALLBACK_2(MainMenu::menuContinueCallback, this));

	cocos2d::ui::Button* language_button = ui::Button::create("res/buttons/yellow_button.png");
	language_button->setScale(visibleSize.width * 0.2 / language_button->getContentSize().width);
	int nb_languages = ((AppDelegate*)Application::getInstance())->getConfig()["languages"].size();
	if (language == "en") {
		language = "English";
	}
	else if (language == "fr") {
		language = "Francais";
	}
	else {
		language = "Language";
	}
	language_button->setTitleText(language);
	language_button->setTitleColor(Color3B::BLACK);
	language_button->setTitleFontName("fonts/LICABOLD.ttf");
	language_button->setTitleFontSize(60.f);
	language_button->setPosition(Vec2(visibleSize.width - 
		language_button->getContentSize().width * language_button->getScaleX() * 2 / 3, 
		visibleSize.height * 9 / 10));
	
	auto list_languages_levels = ui::Layout::create();
	list_languages_levels->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
	list_languages_levels->setBackGroundColor(Color3B::GREEN);
	list_languages_levels->setAnchorPoint(Vec2(0.5f, 0.5f));
	list_languages_levels->setPosition(language_button->getPosition() - 
		Vec2(0, language_button->getContentSize().height * language_button->getScaleY() / 2 + 
			language_button->getContentSize().height * language_button->getScaleY() / 2));
	
	list_languages_levels->setScale(0);
	language_button->addTouchEventListener([&, list_languages_levels](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			if (list_languages_levels->getScale() == 1.f) {
				list_languages_levels->runAction(ScaleTo::create(0.125f, 0.f));
			}
			else {
				list_languages_levels->setScale(0.f);
				list_languages_levels->runAction(ScaleTo::create(0.125f, 1.f));
			}
		}
	});

	cocos2d::ui::Layout* layout = ui::Layout::create();
	addChild(layout,1,"interface");
	layout->addChild(button,1,"start");
	layout->addChild(language_button,1,"language");
	layout->addChild(list_languages_levels,1,"list_language");
	initLanguageList();
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
		stopAllActions();
		Size visibleSize = Director::getInstance()->getVisibleSize();

		getChildByName("vs")->runAction(MoveTo::create(0.125f, Vec2(visibleSize.width * (8.0 / 12.0), visibleSize.height * (9.0 / 12.0))));
		getChildByName("logo_background")->runAction(FadeOut::create(0.125f));
		getChildByName("logo")->runAction(FadeOut::create(0.125f));
		getChildByName("last_background")->runAction(FadeIn::create(0.125f));
		getChildByName("mallowstxt")->runAction(FadeIn::create(0.125f));
		getChildByName("dangotxt")->runAction(FadeIn::create(0.125f));
		getChildByName("vs")->runAction(FadeIn::create(0.125f));
		getChildByName("interface")->getChildByName("start")->runAction(FadeIn::create(0.125f));
		auto switchScenes = CallFunc::create([]() {
			SceneManager::getInstance()->setScene(SceneManager::LEVELS);
		});
		runAction(Sequence::createWithTwoActions(DelayTime::create(0.200f), switchScenes));
	}
}


void MainMenu::onEnter(){
	Scene::onEnter();
}

void MainMenu::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
	scheduleUpdate();
}

void MainMenu::initLanguageList() {
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();
	int nb_languages = ((AppDelegate*)Application::getInstance())->getConfig()["languages"].size();
	ui::Button* language_button = ((ui::Button*)getChildByName("interface")->getChildByName("language"));
	ui::Layout* list_languages_levels = (ui::Layout*)getChildByName("interface")->getChildByName("list_language");

	int nb_displayed_languages = 0;
	for (int i(0); i < nb_languages; ++i) {
		std::string n_language = ((AppDelegate*)Application::getInstance())->getConfig()["languages"][i].asString();
		std::string l;
		if (n_language == "en") {
			l = "English";
		}
		else if (n_language == "fr") {
			l = "Francais";
		}
		if (n_language != language) {
			auto n_language_button = cocos2d::ui::Button::create("res/buttons/blue_button.png");
			n_language_button->setScale(language_button->getContentSize().height * language_button->getScaleY() /
				n_language_button->getContentSize().height * 2 / 3);
			n_language_button->setPosition(Vec2(0, -n_language_button->getContentSize().height *
				n_language_button->getScaleY() * nb_displayed_languages));
			n_language_button->addTouchEventListener([&, n_language, l, n_language_button, list_languages_levels]
			(Ref* sender, ui::Widget::TouchEventType type) {
				if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
					((AppDelegate*)Application::getInstance())->getConfigClass()->setLanguage(n_language);
					((ui::Button*)getChildByName("interface")->getChildByName("language"))->setTitleText(l);
					list_languages_levels->removeAllChildren();
					initLanguageList();
					((AppDelegate*)Application::getInstance())->switchLanguage();
				}
			});
			Label* language_name = Label::createWithTTF(l, "fonts/arial.ttf", 35);
			language_name->setColor(Color3B::BLACK);
			language_name->setAlignment(TextHAlignment::CENTER);
			language_name->setPosition(Vec2(n_language_button->getContentSize().width / 2,
				n_language_button->getContentSize().height / 2.0f));
			n_language_button->addChild(language_name, 1, "language_name");
			list_languages_levels->addChild(n_language_button);
			++nb_displayed_languages;
		}
	}
}

void MainMenu::switchLanguage() {
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();
	Size visibleSize = Director::getInstance()->getVisibleSize();


	((ui::Button*)getChildByName("interface")->getChildByName("start"))->setTitleText(
		((AppDelegate*)Application::getInstance())->getConfig()
		["buttons"]["start"][language].asString());

	auto menu_restart = ui::Layout::create();
	menu_restart->setPosition(Vec2(Point(visibleSize.width / 2, visibleSize.height * 1.5)));

	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setZoomScale(0);
	menu_restart->addChild(panel, 1, "panel");
	panel->setScaleX(0.45*visibleSize.width / panel->getContentSize().width);
	panel->setScaleY(0.45*visibleSize.width / panel->getContentSize().width);

	ui::Button* quit = ui::Button::create("res/buttons/yellow_button.png");
	quit->setScale(visibleSize.width / 5 / quit->getContentSize().width);
	quit->setTitleText(((AppDelegate*)Application::getInstance())->getConfig()
		["buttons"]["quit"][language].asString());
	quit->setTitleFontName("fonts/LICABOLD.ttf");
	quit->setTitleFontSize(45.f * visibleSize.width / 1280);
	Label* quit_label = quit->getTitleRenderer();
	quit_label->enableOutline(Color4B::BLACK, 2);
	quit->setTitleColor(Color3B::WHITE);
	quit->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	quit->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
						CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
			#else
						Director::getInstance()->end();
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
						exit(0);
			#endif
			#endif
		}
	});
	quit->setPosition(Vec2(0,
		-panel->getContentSize().height*panel->getScaleY() / 2 -
		quit->getContentSize().height*quit->getScaleY() * 0.41));
	menu_restart->addChild(quit, 1, "quit");

	Label* advice = Label::createWithTTF(((AppDelegate*)Application::getInstance())->getConfig()["buttons"]["quit_info"][
		language].asString(), "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	advice->setDimensions(panel->getContentSize().width * panel->getScaleX() * 0.75,
		panel->getContentSize().height * panel->getScaleY() * 0.4);
	advice->setPosition(0, 0);
	advice->setColor(Color3B::BLACK);
	advice->setHorizontalAlignment(TextHAlignment::CENTER);
	advice->setVerticalAlignment(TextVAlignment::CENTER);
	menu_restart->addChild(advice, 2, "advice_text");

	addChild(menu_restart, 4);

	auto* showAction = EaseBackOut::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height / 2)));
	menu_restart->runAction(showAction);

	addChild(ui::Layout::create(), 3, "black_mask");
	ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("black_mask")->addChild(mask);
}