#include "LoseMenu.h"
#include "../../../AppDelegate.h"
#include "../MyGame.h"


LoseMenu* LoseMenu::create(MyGame * game, const std::string& language, const Json::Value& buttons, const Json::Value& advice)
{
	LoseMenu* loseMenu = new (std::nothrow) LoseMenu();
	if (loseMenu && loseMenu->init(game, language, buttons, advice))
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

bool LoseMenu::init(MyGame * game, const std::string& language, const Json::Value& buttons, const Json::Value& advice)
{
	bool initialized = cocos2d::ui::Layout::init();
	this->game = game;
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	setPosition(cocos2d::Vec2(cocos2d::Point(visibleSize.width / 2, visibleSize.height * 1.5)));

	addPanel(visibleSize, 0.45);
	addRightButton(buttons["main_menu"][language].asString());
	addLeftButton(buttons["retry"][language].asString());
	addYouLoseLabel(buttons, language, visibleSize);
	addAdvice(language, visibleSize, advice);
	return initialized;
}

void LoseMenu::addAdvice(const std::string &language, cocos2d::Size &visibleSize, const Json::Value& advice)
{
	auto conf = advice[language];
	std::string advice_text = conf[rand() % conf.size()].asString();

	cocos2d::Label* adviceLabel = cocos2d::Label::createWithTTF(advice_text, "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	adviceLabel->setDimensions(panel->getContentSize().width * panel->getScaleX() * 0.75,
		panel->getContentSize().height * panel->getScaleY() * 0.4);
	adviceLabel->setPosition(0, 0);
	adviceLabel->setColor(cocos2d::Color3B::BLACK);
	adviceLabel->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	adviceLabel->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
	addChild(adviceLabel, 2, "advice_text");
}

void LoseMenu::addYouLoseLabel(const Json::Value &buttons, const std::string &language, cocos2d::Size &visibleSize)
{
	cocos2d::Label* you_lose = cocos2d::Label::createWithTTF(
		buttons["lose_info"][language].asString(),
		"fonts/LICABOLD.ttf", 50.f * visibleSize.width / 1280);
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