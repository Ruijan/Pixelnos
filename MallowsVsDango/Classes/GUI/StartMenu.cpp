#include "StartMenu.h"
#include "../Config/json.h"
#include "../AppDelegate.h"


StartMenu::StartMenu() :
	interfaceGame(nullptr),
	hidden(false)
{

}

StartMenu* StartMenu::create(InterfaceGame* interfaceGame, int levelId) {
	StartMenu* menu = new (std::nothrow) StartMenu();
	if (menu && menu->init(interfaceGame, levelId))
	{
		menu->autorelease();
		return menu;
	}
	CC_SAFE_DELETE(menu);
	return nullptr;
}

void StartMenu::displayWithAnimation()
{
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	getChildByName("start")->setVisible(true);
	getChildByName("title")->setVisible(true);
	getChildByName("advice")->setVisible(true);
	getChildByName("title")->runAction(cocos2d::Sequence::create(
		cocos2d::EaseBackOut::create(
			cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(getChildByName("title")->getPosition().x, visibleSize.height / 2))),
		cocos2d::DelayTime::create(1.f),
		cocos2d::FadeOut::create(0.5f), nullptr));
	getChildByName("start")->runAction(cocos2d::EaseBackOut::create(
		cocos2d::MoveTo::create(0.5f,
			cocos2d::Vec2(getChildByName("start")->getPosition().x, getChildByName("start")->getContentSize().height * getChildByName("start")->getScaleY()))));
}

void StartMenu::hide() {
	if (!hidden) {
		getChildByName("start")->setVisible(false);
		getChildByName("title")->setVisible(false);
		getChildByName("advice")->setVisible(false);
	}
}

StartMenu::~StartMenu()
{
}

void StartMenu::reset(int levelId)
{
	Config* config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass();
	std::string language = config->getLanguage();
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	getChildByName("start")->setPosition(cocos2d::Vec2(getChildByName("start")->getPosition().x,
		-getChildByName("start")->getContentSize().height * getChildByName("start")->getScaleY()));
	getChildByName("title")->setPosition(cocos2d::Vec2(getChildByName("title")->getPosition().x,
		visibleSize.height + getChildByName("title")->getContentSize().height * getChildByName("title")->getScaleY()));
	((cocos2d::Label*)getChildByName("title"))->runAction(cocos2d::FadeIn::create(0.5f));
	((cocos2d::Label*)getChildByName("title"))->setString(
		((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON)["level"][language].asString() + " " +
		Json::Value((int)(levelId + 1)).asString());
	getChildByName("advice")->setVisible(false);
	getChildByName("advice")->runAction(cocos2d::RepeatForever::create(cocos2d::Sequence::create(
		cocos2d::FadeIn::create(1.f), cocos2d::FadeOut::create(0.5f), cocos2d::DelayTime::create(1.f), NULL)));
}

bool StartMenu::init(InterfaceGame* interfaceGame, int levelId) {
	bool initialized = cocos2d::ui::Layout::init();
	this->interfaceGame = interfaceGame;
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	std::string language = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getLanguage();
	Json::Value buttons = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::BUTTON);

	addTitleLabel(buttons, language, levelId, visibleSize);
	addAdviceLabel(buttons, language, visibleSize);
	addStartButton(visibleSize, buttons, language);
	return initialized;
}

void StartMenu::addTitleLabel(Json::Value &buttons, std::string &language, int levelId, cocos2d::Size &visibleSize) {
	cocos2d::Label* title = cocos2d::Label::createWithTTF(buttons["level"][language].asString() + " " +
		Json::Value((int)(levelId + 1)).asString(),
		"fonts/LICABOLD.ttf", round(visibleSize.width / 12.0));
	title->setColor(cocos2d::Color3B::YELLOW);
	title->enableOutline(cocos2d::Color4B::BLACK, 3);
	title->setPosition(round(visibleSize.width * 3 / 8.0), visibleSize.height + title->getContentSize().height / 2);
	addChild(title, 2, "title");
}

void StartMenu::addAdviceLabel(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize) {
	cocos2d::Label* advice = cocos2d::Label::createWithTTF(buttons["advice_game_drag"][language].asString(),
		"fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	advice->setDimensions(visibleSize.width / 2, visibleSize.height / 10);
	advice->setPosition(round(visibleSize.width * 3 / 8.0), visibleSize.height - advice->getContentSize().height);
	advice->setColor(cocos2d::Color3B::WHITE);
	advice->enableOutline(cocos2d::Color4B::BLACK, 2);
	advice->setAlignment(cocos2d::TextHAlignment::CENTER);
	advice->setVisible(false);
	advice->runAction(cocos2d::RepeatForever::create(cocos2d::Sequence::create(
		cocos2d::FadeIn::create(1.f), cocos2d::FadeOut::create(0.5f), cocos2d::DelayTime::create(1.f), NULL)));
	addChild(advice, 2, "advice");
}

void StartMenu::addStartButton(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language)
{
	cocos2d::ui::Button* start = cocos2d::ui::Button::create("res/buttons/blue_button.png");
	start->setScale(visibleSize.width / 4 / start->getContentSize().width);
	start->setTitleText(buttons["start_game"][language].asString());
	start->setTitleFontName("fonts/LICABOLD.ttf");
	start->setTitleFontSize(45.f);
	cocos2d::Label* start_label = start->getTitleRenderer();
	start_label->enableOutline(cocos2d::Color4B::BLACK, 2);
	start->setTitleColor(cocos2d::Color3B::YELLOW);
	start->setTitleAlignment(cocos2d::TextHAlignment::CENTER);
	start->setPosition(cocos2d::Vec2(round(visibleSize.width * 3 / 8.0), -start->getContentSize().height * start->getScaleY()));
	start->addTouchEventListener(CC_CALLBACK_2(StartMenu::startButtonCallback, this));
	addChild(start, 2, "start");
}

void StartMenu::startButtonCallback(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		interfaceGame->setListening(true);
		getChildByName("start")->runAction(cocos2d::EaseBackIn::create(cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(getChildByName("start")->getPosition().x,
			-getChildByName("start")->getContentSize().height * getChildByName("start")->getScaleY()))));
		getChildByName("title")->setVisible(false);
		getChildByName("advice")->setVisible(false);
		interfaceGame->showLabelInformation();
		interfaceGame->setGameState(InterfaceGame::RUNNING);
	}
}
