#include "WinMenu.h"
#include "../AppDelegate.h"
#include "../Scenes/MyGame.h"


WinMenu * WinMenu::create(MyGame* game)
{
	WinMenu* winMenu = new (std::nothrow) WinMenu();
	if (winMenu && winMenu->init(game))
	{
		winMenu->autorelease();
		return winMenu;
	}
	CC_SAFE_DELETE(winMenu);
	return nullptr;
}

void WinMenu::showWin()
{
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto* showAction = cocos2d::TargetedAction::create(this, cocos2d::EaseBackOut::create(
		cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2))));
	Json::Value root = ((AppDelegate*)cocos2d::Application::getInstance())->getSave();

	rewardSugarValueLabel->setString("+" + Json::Value(game->getLevel()->getHolySugar()).asString());
	rewardSugarValueLabel->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.5f),
			cocos2d::ScaleTo::create(0.25f, 1.5f),
			cocos2d::ScaleTo::create(0.25f, 1.f),
			nullptr));
	for (unsigned int i(0); i < root["towers"].getMemberNames().size(); ++i) {
		std::string tower_name = root["towers"].getMemberNames()[i];
		if (root["towers"][tower_name]["unlocked"].asBool()) {
			getChildByName(tower_name + "_levelup")->setVisible(false);
			auto exp_label = ((cocos2d::Label*)getChildByName(tower_name + "_exp"));
			int max_level = root["towers"][tower_name]["max_level"].asInt();
			exp_label->setColor(cocos2d::Color3B::WHITE);
			auto loading_bar = ((cocos2d::ui::LoadingBar*)getChildByName(tower_name + "_bar"));
			int diff_exp = game->getLevel()->getTowerXP(tower_name);
			exp_label->setString("+" + Json::Value(game->getLevel()->getTowerXP(tower_name)).asString());
			float* increment = new float(0);
			int initial_xp = root["towers"][tower_name]["exp"].asInt();
			int loop(0);
			auto incrementExp = cocos2d::CallFunc::create([this, exp_label, loading_bar, tower_name, increment, initial_xp, diff_exp, loop, max_level]() {
				updateIncrementXP(exp_label, loading_bar, tower_name, increment, initial_xp, diff_exp, loop, max_level);
			});
			exp_label->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(0.1f), cocos2d::ScaleTo::create(0.25f, 1.5f), cocos2d::ScaleTo::create(0.25f, 1.f), incrementExp, nullptr));
		}
	}
	runAction(showAction);
}

void WinMenu::updateIncrementXP(cocos2d::Label* exp_label, cocos2d::ui::LoadingBar* loading_bar, std::string tower_name,
	float* increment, int initial_value, int diff_exp, int loop, int max_level) {
	Json::Value root = ((AppDelegate*)cocos2d::Application::getInstance())->getSave();
	Json::Value config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TOWER);

	int c_exp = initial_value;
	float duration = 4.0 / ((float)loop + 1.0);
	float speed = 0.05f;
	int nb_iterations = duration / speed;

	if (root["towers"][tower_name]["unlocked"].asBool()) {
		c_exp += (*increment);
		while (max_level < (int)config[tower_name]["xp_level"].size() &&
			c_exp > config[tower_name]["xp_level"][max_level + 1].asInt()) {
			c_exp -= config[tower_name]["xp_level"][max_level + 1].asInt();
			++max_level;
		}
		if (initial_value != 0 && c_exp - (*increment) <= 0) {
			getChildByName(tower_name + "_levelup")->setVisible(true);
			getChildByName(tower_name + "_levelup")->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.125f, 1.5f),
				cocos2d::ScaleTo::create(0.125f, 1.f), nullptr));
		}
	}
	loading_bar->setPercent(100 * (float)c_exp / Tower::getConfig()[tower_name]["xp_level"][max_level + 1].asDouble());
	exp_label->setString("+" + Json::Value(diff_exp - (int)(*increment)).asString());
	if (exp_label->getString() != "+0") {
		(*increment) += (float)diff_exp / (float)nb_iterations;
		if ((*increment) > diff_exp) {
			(*increment) = diff_exp;
		}
		auto incrementExp = cocos2d::CallFunc::create([this, exp_label, loading_bar, tower_name,
			increment, initial_value, diff_exp, loop, max_level]() {
			updateIncrementXP(exp_label, loading_bar, tower_name, increment, initial_value,
				diff_exp, loop, max_level);
		});
		exp_label->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(0.04f), incrementExp, nullptr));
	}
	else if (loop == 0) {
		++loop;
		initial_value = c_exp;
		diff_exp = game->getLevel()->getTotalExperience();
		exp_label->setString("+" + Json::Value(diff_exp).asString());
		exp_label->setColor(cocos2d::Color3B::YELLOW);
		(*increment) = 0;
		auto incrementExp = cocos2d::CallFunc::create([this, exp_label, loading_bar, tower_name,
			increment, initial_value, diff_exp, loop, max_level]() {
			updateIncrementXP(exp_label, loading_bar, tower_name, increment, initial_value,
				diff_exp, loop, max_level);
		});
		exp_label->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.25f, 1.5f),
			cocos2d::ScaleTo::create(0.25f, 1.f), cocos2d::DelayTime::create(0.04f), incrementExp, nullptr));
	}
}

bool WinMenu::init(MyGame* game)
{
	bool initialized = cocos2d::ui::Layout::init();

	this->game = game;
	cocos2d::Color3B color1(255, 200, 51);
	cocos2d::Color4F grey(102 / 255.0f, 178 / 255.0f, 255 / 255.0f, 0.66f);
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	Config* config = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass();
	std::string language = config->getLanguage();
	Json::Value buttons = config->getConfigValues(Config::ConfigType::BUTTON);

	setPosition(cocos2d::Vec2(cocos2d::Point(visibleSize.width / 2, visibleSize.height * 1.5)));

	addPanel(visibleSize, 0.55);
	addLeftButton(buttons["next_level"][language].asString());
	addRightButton(buttons["main_menu"][language].asString());
	addYouWinLabel(buttons, language, visibleSize);
	addStars();
	addRewardSugar(buttons, language, visibleSize);
	addTowerExperiences(visibleSize);
	addWinMallowsImages();
	return initialized;
}

void WinMenu::addTowerExperiences(cocos2d::Size &visibleSize)
{
	Json::Value root = ((AppDelegate*)cocos2d::Application::getInstance())->getConfigClass()->getSaveValues()["towers"];
	previousObjectPos = previousObjectPos - cocos2d::Vec2(0, previousObjectSize.height);
	std::vector<std::string> towerNames = root.getMemberNames();
	for (auto towerName : towerNames) {
		if (root[towerName]["unlocked"].asBool()) {
			addTowerLoadingExp(towerName, visibleSize, root);
		}
	}
}

void WinMenu::addWinMallowsImages()
{
	cocos2d::Sprite* win_mallow = cocos2d::Sprite::create("res/buttons/win.png");
	win_mallow->setScale(panel->getContentSize().width * panel->getScaleX() / 7 / win_mallow->getContentSize().width);
	win_mallow->setPosition(youWinLabel->getPosition().x - youWinLabel->getContentSize().width / 2 - win_mallow->getContentSize().width*win_mallow->getScale() * 3 / 4,
		youWinLabel->getPosition().y);
	addChild(win_mallow, 2, "win_mallow");
	/*Sprite* win_mallow2 = Sprite::create("res/buttons/win.png");
	win_mallow2->setScale(panel->getContentSize().width * panel->getScaleX() / 6 / win_mallow2->getContentSize().width);
	win_mallow2->setPosition(0,
	-panel->getContentSize().height * panel->getScaleY() * 2 / 5);
	addChild(win_mallow2, 2, "win_mallow2");*/
	cocos2d::Sprite* win_mallow3 = cocos2d::Sprite::create("res/buttons/win.png");
	win_mallow3->setScale(panel->getContentSize().width * panel->getScaleX() / 7 / win_mallow3->getContentSize().width);
	win_mallow3->setPosition(youWinLabel->getPosition().x + youWinLabel->getContentSize().width / 2 + win_mallow3->getContentSize().width*win_mallow3->getScale() * 3 / 4,
		youWinLabel->getPosition().y);
	addChild(win_mallow3, 2, "win_mallow3");
}

void WinMenu::addTowerLoadingExp(std::string & towerName, cocos2d::Size &visibleSize, Json::Value &root)
{
	cocos2d::Label* exp_tower = cocos2d::Label::createWithTTF("Exp " + towerName, "fonts/LICABOLD.ttf", 40.f * visibleSize.width / 1280);
	exp_tower->enableOutline(cocos2d::Color4B::BLACK, 2);
	exp_tower->setPosition(-panel->getContentSize().width * panel->getScaleX() * 2 / 5, previousObjectPos.y - exp_tower->getContentSize().height / 2);
	exp_tower->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	exp_tower->setColor(cocos2d::Color3B::WHITE);
	addChild(exp_tower, 2, towerName);

	cocos2d::Label* exp_tower_n = cocos2d::Label::createWithTTF("0", "fonts/LICABOLD.ttf", 40.f * visibleSize.width / 1280);
	exp_tower_n->enableOutline(cocos2d::Color4B::BLACK, 2);
	exp_tower_n->setPosition(panel->getContentSize().width * panel->getScaleX() / 3, exp_tower->getPosition().y);
	exp_tower_n->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	exp_tower_n->setColor(cocos2d::Color3B::WHITE);
	addChild(exp_tower_n, 2, towerName + "_exp");

	cocos2d::ui::LoadingBar* loading_bar = cocos2d::ui::LoadingBar::create("res/buttons/loaderProgress.png");
	loading_bar->setPercent(100 * root[towerName]["exp"].asDouble() /
		Tower::getConfig()[towerName]["xp_level"][root[towerName]["max_level"].asInt() + 1].asDouble());
	loading_bar->setScaleX(panel->getContentSize().width * panel->getScaleX() * 0.25 / loading_bar->getContentSize().width);
	loading_bar->setScaleY(exp_tower->getContentSize().height * 0.35f / loading_bar->getContentSize().height);
	loading_bar->setDirection(cocos2d::ui::LoadingBar::Direction::LEFT);
	loading_bar->setPosition(cocos2d::Vec2(loading_bar->getContentSize().width * loading_bar->getScaleX() / 2, exp_tower->getPosition().y));
	addChild(loading_bar, 2, towerName + "_bar");

	cocos2d::Sprite* loadingBarBackground = cocos2d::Sprite::create("res/buttons/loaderBackground.png");
	loadingBarBackground->setPosition(loading_bar->getPosition());
	loadingBarBackground->setScale(panel->getContentSize().width * panel->getScaleX() * 0.26 / loadingBarBackground->getContentSize().width);
	loadingBarBackground->setScaleY(exp_tower->getContentSize().height * 0.40f / loading_bar->getContentSize().height);
	addChild(loadingBarBackground, 1, towerName + "_bar_background");

	cocos2d::Label* level_up = cocos2d::Label::createWithTTF("Level up!", "fonts/LICABOLD.ttf", 23.f * visibleSize.width / 1280);
	level_up->setColor(cocos2d::Color3B::YELLOW);
	level_up->enableOutline(cocos2d::Color4B::RED, 1);
	level_up->setVisible(false);
	level_up->setPosition(loading_bar->getPosition().x, loading_bar->getPosition().y +
		loading_bar->getContentSize().height * loading_bar->getScaleY() / 2 +
		level_up->getContentSize().height / 2);
	addChild(level_up, 2, towerName + "_levelup");
	previousObjectPos.y = exp_tower->getPosition().y - exp_tower->getContentSize().height / 2;
}

void WinMenu::addRewardSugar(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize)
{
	cocos2d::Label* rewardSugar = cocos2d::Label::createWithTTF(buttons["holy_sugar"][language].asString(),
		"fonts/LICABOLD.ttf", 40.f * visibleSize.width / 1280);
	rewardSugar->enableOutline(cocos2d::Color4B::BLACK, 2);
	rewardSugar->setPosition(-panel->getContentSize().width * panel->getScaleX() * 2 / 5, previousObjectPos.y -
		rewardSugar->getContentSize().height / 2 - previousObjectSize.height * 3 / 4);
	rewardSugar->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	rewardSugar->setColor(cocos2d::Color3B::WHITE);
	addChild(rewardSugar, 2, "reward_sugar_label");

	previousObjectPos = rewardSugar->getPosition();
	previousObjectSize = rewardSugar->getContentSize();

	rewardSugarValueLabel = cocos2d::Label::createWithTTF("0", "fonts/LICABOLD.ttf", 45.f * visibleSize.width / 1280);
	rewardSugarValueLabel->enableOutline(cocos2d::Color4B::BLACK, 2);
	rewardSugarValueLabel->setPosition(panel->getContentSize().width * panel->getScaleX() / 3, previousObjectPos.y);
	rewardSugarValueLabel->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	rewardSugarValueLabel->setColor(cocos2d::Color3B::YELLOW);
	addChild(rewardSugarValueLabel, 2, "rewardSugarValueLabel");

	cocos2d::Sprite* holySugar = cocos2d::Sprite::create("res/buttons/holy_sugar.png");
	holySugar->setScale(panel->getContentSize().width * panel->getScaleX() / 10 / holySugar->getContentSize().width);
	holySugar->setPosition(panel->getContentSize().width * panel->getScaleX() * 0.125, previousObjectPos.y);
	addChild(holySugar, 2, "holySugar");
}

void WinMenu::addStars()
{
	cocos2d::Sprite* star_left = cocos2d::Sprite::create("res/levels/rewards/star_empty.png");
	star_left->setScale(panel->getContentSize().width * panel->getScaleX() / 8 / star_left->getContentSize().width);
	star_left->setPosition(cocos2d::Vec2(-panel->getContentSize().width * panel->getScaleX() * 1 / 5,
		previousObjectPos.y -
		star_left->getContentSize().height / 2 - previousObjectSize.height));
	star_left->setRotation(-35);
	cocos2d::Sprite* star_middle = cocos2d::Sprite::create("res/levels/rewards/star_empty.png");
	star_middle->setScale(panel->getContentSize().width * panel->getScaleX() / 7 / star_middle->getContentSize().width);
	star_middle->setPosition(cocos2d::Vec2(0, star_left->getPosition().y + cocos2d::Director::getInstance()->getVisibleSize().width / 45));
	cocos2d::Sprite* star_right = cocos2d::Sprite::create("res/levels/rewards/star_empty.png");
	star_right->setScale(panel->getContentSize().width * panel->getScaleX() / 8 / star_right->getContentSize().width);
	star_right->setRotation(35);
	star_right->setPosition(cocos2d::Vec2(panel->getContentSize().width * panel->getScaleX() * 1 / 5, star_left->getPosition().y));
	addChild(star_left, 2, "star_left");
	addChild(star_middle, 2, "star_middle");
	addChild(star_right, 2, "star_right");
	previousObjectPos = star_left->getPosition();
	previousObjectSize = star_middle->getContentSize() * star_middle->getScale();
}

void WinMenu::addYouWinLabel(Json::Value &buttons, std::string &language, cocos2d::Size &visibleSize)
{
	youWinLabel = cocos2d::Label::createWithTTF(buttons["level_cleared"][language].asString(),
		"fonts/LICABOLD.ttf", 60.f * visibleSize.width / 1280);
	youWinLabel->enableOutline(cocos2d::Color4B::BLACK, 2);
	youWinLabel->setPosition(0, panel->getContentSize().height*panel->getScaleY() * 0.35);
	youWinLabel->setColor(cocos2d::Color3B::YELLOW);
	addChild(youWinLabel, 2, "you_win");
	previousObjectPos = youWinLabel->getPosition();
	previousObjectSize = youWinLabel->getContentSize();
}

void WinMenu::rightButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		game->updateTracker("left");
		auto callbackmainmenu = cocos2d::CallFunc::create([&]() {
			SceneManager::getInstance()->setScene(SceneManager::LEVELS);
		});
		this->runAction(cocos2d::Sequence::create(createHideAction(this), callbackmainmenu, nullptr));
	}
}

void WinMenu::leftButtonCallback(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
		auto* hideAction = cocos2d::TargetedAction::create(this,
			cocos2d::EaseBackIn::create(cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(visibleSize.width / 2, visibleSize.height * 1.5))));
		auto callbacknextlevel = cocos2d::CallFunc::create([&]() {
			game->switchLevel();
		});
		this->runAction(cocos2d::Sequence::create(hideAction, callbacknextlevel, nullptr));
	}
}