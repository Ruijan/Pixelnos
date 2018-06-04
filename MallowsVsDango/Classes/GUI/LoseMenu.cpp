#include "LoseMenu.h"
#include "../AppDelegate.h"
#include "../Scenes/MyGame.h"


LoseMenu* LoseMenu::create(MyGame * game)
{
	LoseMenu* loseMenu = new (std::nothrow) LoseMenu();
	if (loseMenu && loseMenu->init(game))
	{
		loseMenu->autorelease();
		return loseMenu;
	}
	CC_SAFE_DELETE(loseMenu);
	return nullptr;
}

void LoseMenu::showLose()
{
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto* showAction = cocos2d::TargetedAction::create(this,
		cocos2d::EaseBackOut::create(cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2))));
	runAction(showAction);
}

bool LoseMenu::init(MyGame * game)
{
	bool initialized = cocos2d::ui::Layout::init();
	this->game = game;
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	std::string language = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getLanguage();
	Json::Value buttons = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON);
	setPosition(cocos2d::Vec2(cocos2d::Point(visibleSize.width / 2, visibleSize.height * 1.5)));

	addPanel(visibleSize);
	addRetryButton(visibleSize, buttons, language, game);
	addMainMenuButton(visibleSize, buttons, language);
	addYouLoseLabel(buttons, language, visibleSize);
	addAdvice(language, visibleSize);
	return initialized;
}

void LoseMenu::addAdvice(std::string &language, cocos2d::Size &visibleSize)
{
	auto conf = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::ADVICE)[language];
	std::string advice_text = conf[rand() % conf.size()].asString();

	cocos2d::Label* advice = cocos2d::Label::createWithTTF(advice_text, "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	advice->setDimensions(panel->getContentSize().width * panel->getScaleX() * 0.75,
		panel->getContentSize().height * panel->getScaleY() * 0.4);
	advice->setPosition(0, 0);
	advice->setColor(cocos2d::Color3B::BLACK);
	advice->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	advice->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
	addChild(advice, 2, "advice_text");
}

void LoseMenu::addYouLoseLabel(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize)
{
	cocos2d::Label* you_lose = cocos2d::Label::createWithTTF(
		buttons["lose_info"][language].asString(),
		"fonts/LICABOLD.ttf", 50.f * visibleSize.width / 1280);
	you_lose->enableOutline(cocos2d::Color4B::BLACK, 2);
	you_lose->setPosition(0, panel->getContentSize().height*panel->getScaleY() * 0.35);
	you_lose->setColor(cocos2d::Color3B::YELLOW);
	addChild(you_lose, 2, "text");
}

void LoseMenu::addMainMenuButton(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language)
{
	cocos2d::ui::Button* main_menu_back = cocos2d::ui::Button::create("res/buttons/red_button.png");
	main_menu_back->setScale(visibleSize.width / 5 / main_menu_back->getContentSize().width);
	main_menu_back->setTitleText(buttons["main_menu"][language].asString());
	main_menu_back->setTitleFontName("fonts/LICABOLD.ttf");
	main_menu_back->setTitleFontSize(45.f);
	cocos2d::Label* menu_back_label = main_menu_back->getTitleRenderer();
	menu_back_label->enableOutline(cocos2d::Color4B::BLACK, 2);
	main_menu_back->setTitleColor(cocos2d::Color3B::WHITE);
	main_menu_back->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	main_menu_back->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			game->updateTracker("left");
			auto callbackmainmenu = cocos2d::CallFunc::create([&]() {
				SceneManager::getInstance()->setScene(SceneManager::LEVELS);
			});
			this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackmainmenu, nullptr));
		}
	});
	main_menu_back->setPosition(cocos2d::Vec2(panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 -
		main_menu_back->getContentSize().height*main_menu_back->getScaleY() * 0.41));
	addChild(createButtonShadow(main_menu_back), -1);
	addChild(main_menu_back, 1, "main_menu_back");
}

void LoseMenu::addRetryButton(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language, MyGame * game)
{
	cocos2d::ui::Button* retry = cocos2d::ui::Button::create("res/buttons/yellow_button.png");
	retry->setScale(visibleSize.width / 5 / retry->getContentSize().width);
	retry->setTitleText(buttons["retry"][language].asString());
	retry->setTitleFontName("fonts/LICABOLD.ttf");
	retry->setTitleFontSize(45.f);
	cocos2d::Label* retry_label = retry->getTitleRenderer();
	retry_label->enableOutline(cocos2d::Color4B::BLACK, 2);
	retry->setTitleColor(cocos2d::Color3B::WHITE);
	retry->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	retry->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
			auto callreloadmenu = cocos2d::CallFunc::create([&]() {
				this->game->setReloading(true);
			});
			this->runAction(cocos2d::Sequence::create(createHideAction(this), callreloadmenu, nullptr));
			
		}
	});
	retry->setPosition(cocos2d::Vec2(-panel->getContentSize().width*panel->getScaleX() / 4,
		-panel->getContentSize().height*panel->getScaleY() / 2 -
		retry->getContentSize().height*retry->getScaleY() * 0.41));
	addChild(createButtonShadow(retry), -1);
	addChild(retry, 1, "retry");
}

void LoseMenu::addPanel(cocos2d::Size &visibleSize)
{
	panel = cocos2d::ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setZoomScale(0);
	addChild(panel, 1, "panel");
	panel->setScaleX(0.45*visibleSize.width / panel->getContentSize().width);
	panel->setScaleY(0.45*visibleSize.width / panel->getContentSize().width);
}

cocos2d::TargetedAction* LoseMenu::createHideAction(cocos2d::Node* target) {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	return cocos2d::TargetedAction::create(target,
		cocos2d::EaseBackIn::create(
			cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(visibleSize.width / 2, visibleSize.height * 1.5))));
}

cocos2d::Sprite* LoseMenu::createButtonShadow(cocos2d::ui::Button* button) {
	cocos2d::Sprite* buttonShadow = cocos2d::Sprite::create("res/buttons/shadow_button.png");
	buttonShadow->setScale(button->getScale());
	buttonShadow->setPosition(button->getPosition());
	return buttonShadow;
}