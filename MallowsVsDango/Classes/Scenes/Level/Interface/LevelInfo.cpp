#include "LevelInfo.h"
#include "../ChallengeHandler.h"

LevelInfo * LevelInfo::create(ChallengeHandler* challenges)
{
	LevelInfo* levelInfo = new (std::nothrow) LevelInfo();
	if (levelInfo && levelInfo->init(challenges))
	{
		levelInfo->autorelease();
		return levelInfo;
	}
	CC_SAFE_DELETE(levelInfo);
	return nullptr;
}

bool LevelInfo::init(ChallengeHandler* challenges) {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	const double sizeButton = visibleSize.width / 15;

	setPosition(cocos2d::Vec2(0, visibleSize.height));

	cocos2d::Sprite* sugar = cocos2d::Sprite::create("res/buttons/sugar.png");
	sugar->setScale(sizeButton / sugar->getContentSize().width * 0.75);
	sugar->setPosition(cocos2d::Point(sizeButton * 0.125,
		-sugar->getContentSize().height * sugar->getScale()));
	sugar->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
	addChild(sugar, 3, "sugarImage");

	sugarLabel = cocos2d::Label::createWithTTF("x3000", "fonts/LICABOLD.ttf", 42 * visibleSize.width / 1280);
	addChild(sugarLabel, 3, "sugar");
	sugarLabel->setPosition(cocos2d::Point(sugar->getPosition().x + sugar->getContentSize().width * sugar->getScale(),
		sugar->getPosition().y));
	sugarLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
	sugarLabel->setColor(cocos2d::Color3B::WHITE);
	sugarLabel->enableOutline(cocos2d::Color4B::BLACK, 3);

	cocos2d::Sprite* life = cocos2d::Sprite::create("res/buttons/life2.png");
	life->setPosition(cocos2d::Point(visibleSize.width * 3 / 4,
		sugar->getPosition().y));
	life->setAnchorPoint(cocos2d::Vec2(1, 0.5f));
	life->setScale(sizeButton / life->getContentSize().width * 0.75);
	addChild(life, 3);

	lifeLabel = cocos2d::Label::createWithTTF("", "fonts/LICABOLD.ttf", 42 * visibleSize.width / 1280);
	addChild(lifeLabel, 3, "life");
	lifeLabel->setPosition(cocos2d::Point(life->getPosition().x - life->getContentSize().width * life->getScale(),
		sugar->getPosition().y));
	lifeLabel->setAnchorPoint(cocos2d::Vec2(1, 0.5f));
	lifeLabel->setColor(cocos2d::Color3B::WHITE);
	lifeLabel->enableOutline(cocos2d::Color4B::BLACK, 3);

	loadingBar = cocos2d::ui::LoadingBar::create("res/buttons/level_progression.png");
	loadingBar->setPercent(75);
	loadingBar->setScale(visibleSize.width / 6 / loadingBar->getContentSize().width);
	loadingBar->setDirection(cocos2d::ui::LoadingBar::Direction::LEFT);
	loadingBar->setPosition(cocos2d::Vec2(visibleSize.width * 3 / 8, -sizeButton / 2));
	loadingBar->setVisible(false);
	addChild(loadingBar, 3, "loading_bar");

	cocos2d::Sprite* dango_head = cocos2d::Sprite::create("res/buttons/minidango.png");
	dango_head->setPosition(loadingBar->getPosition() +
		cocos2d::Vec2((loadingBar->getPercent() - 50) / 100 * visibleSize.width / 6, 0));
	dango_head->setScale(loadingBar->getScale());
	dango_head->setVisible(false);
	addChild(dango_head, 4, "dango_head");

	cocos2d::Sprite* loadingBarBackground = cocos2d::Sprite::create("res/buttons/level_progression_background.png");
	loadingBarBackground->setPosition(loadingBar->getPosition());
	loadingBarBackground->setScale(loadingBar->getScale());
	loadingBarBackground->setVisible(false);
	addChild(loadingBarBackground, 3, "loading_bar_background");

	cocos2d::Vec2 position = cocos2d::Vec2(sugar->getPosition().x + sugar->getContentSize().width * sugar->getScale() / 2,
		sugar->getPosition().y - sugar->getContentSize().height * sugar->getScale());
	challenges->setPosition(position);
	addChild(challenges, 1, "challenges");

	return true;
}

void LevelInfo::update(int sugarQuantity, int lifeQuantity, double progress) {
	std::string sugarText = " " + std::to_string(sugarQuantity);
	std::string lifeText = std::to_string(lifeQuantity);
	if (sugarLabel->getString() != sugarText) {
		sugarLabel->setString(sugarText);
		sugarLabel->enableOutline(cocos2d::Color4B::BLACK, 2.0);
	}
	if (lifeLabel->getString() != lifeText) {
		lifeLabel->setString(lifeText);
		lifeLabel->enableOutline(cocos2d::Color4B::BLACK, 2.0);
	}
	if (abs(loadingBar->getPercent() - progress * 100) > 0.01) {
		cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

		loadingBar->setPercent(progress * 100);
		(getChildByName("dango_head"))->runAction(cocos2d::MoveTo::create(0.5f,
			loadingBar->getPosition() + cocos2d::Vec2((loadingBar->getPercent() - 50) / 100 * visibleSize.width / 6 * 0.98, 0))
		);
	}
}

void LevelInfo::reset(ChallengeHandler* challenges) {
	removeChild(getChildByName("challenges"));
	getChildByName("dango_head")->setVisible(false);
	getChildByName("loading_bar_background")->setVisible(false);
	loadingBar->setVisible(false);

	cocos2d::Node* sugar = getChildByName("sugarImage");
	cocos2d::Vec2 position = cocos2d::Vec2(sugar->getPosition().x + sugar->getContentSize().width * sugar->getScale() / 2,
		sugar->getPosition().y - sugar->getContentSize().height * sugar->getScale());
	challenges->setPosition(position);
	addChild(challenges, 1, "challenges");
}

void LevelInfo::resetAnimations() {
	getChildByName("sugar")->stopAllActions();
	getChildByName("sugar")->setRotation(0.f);
	getChildByName("sugar")->setScale(1.0f);
}

void LevelInfo::showProgress() {
	getChildByName("dango_head")->setVisible(true);
	getChildByName("loading_bar_background")->setVisible(true);
	getChildByName("loading_bar")->setVisible(true);
}