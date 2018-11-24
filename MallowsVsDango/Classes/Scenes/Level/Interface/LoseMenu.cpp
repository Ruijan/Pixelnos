#include "LoseMenu.h"
#include "../../../AppDelegate.h"
#include "../MyGame.h"


LoseMenu* LoseMenu::create(MyGame * game, GUISettings* settings, const Json::Value& advice)
{
	LoseMenu* loseMenu = new (std::nothrow) LoseMenu();
	if (loseMenu && loseMenu->init(game, settings, advice))
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

bool LoseMenu::init(MyGame * game, GUISettings* settings, const Json::Value& advice)
{
	bool initialized = cocos2d::ui::Layout::init();
	this->game = game;
	this->settings = settings;
	cocos2d::Size visibleSize = settings->getVisibleSize();
	setPosition(cocos2d::Vec2(cocos2d::Point(visibleSize.width / 2, visibleSize.height * 1.5)));

	addPanel(visibleSize, 0.45);
	addRightButton(settings->getButton("main_menu"));
	addLeftButton(settings->getButton("retry"));
	addYouLoseLabel();
	addAdvice(advice);
	return initialized;
}

void LoseMenu::addAdvice(const Json::Value& advice)
{
	auto conf = advice[settings->getLanguage()];
	std::string advice_text = conf[rand() % conf.size()].asString();

	cocos2d::Label* adviceLabel = cocos2d::Label::createWithTTF(advice_text, "fonts/LICABOLD.ttf", 30.f * settings->getVisibleSize().width / 1280);
	adviceLabel->setDimensions(panel->getContentSize().width * panel->getScaleX() * 0.75,
		panel->getContentSize().height * panel->getScaleY() * 0.4);
	adviceLabel->setPosition(0, 0);
	adviceLabel->setColor(cocos2d::Color3B::BLACK);
	adviceLabel->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	adviceLabel->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
	addChild(adviceLabel, 2, "advice_text");
}

void LoseMenu::addYouLoseLabel()
{
	cocos2d::Label* you_lose = cocos2d::Label::createWithTTF(
		settings->getButton("lose_info"),
		"fonts/LICABOLD.ttf", 50.f * settings->getVisibleSize().width / 1280);
	you_lose->enableOutline(cocos2d::Color4B::BLACK, 2);
	you_lose->setPosition(0, panel->getContentSize().height*panel->getScaleY() * 0.35);
	you_lose->setColor(cocos2d::Color3B::YELLOW);
	addChild(you_lose, 2, "text");
}

void LoseMenu::rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		game->updateTracker("left");
		auto callbackmainmenu = cocos2d::CallFunc::create([&]() {
			SceneManager::getInstance()->setScene(SceneFactory::LEVELS);
		});
		this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackmainmenu, nullptr));
	}
}

void LoseMenu::leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
		auto callreloadmenu = cocos2d::CallFunc::create([&]() {
			this->game->setReloading(true);
		});
		this->runAction(cocos2d::Sequence::create(createHideAction(this), callreloadmenu, nullptr));
	}
}