#include "RewardSugar.h"
#include "AppDelegate.h"

USING_NS_CC;

RewardSugar::RewardSugar() {}

RewardSugar* RewardSugar::create(cocos2d::Vec2 pos) {
	RewardSugar* reward = new RewardSugar();
	if (reward->init()) {
		Sprite* sugar = Sprite::create("res/buttons/holy_sugar.png");
		sugar->setScale(Cell::getCellWidth() / sugar->getContentSize().width);
		reward->addChild(sugar,1,"sugar");
		reward->setPosition(pos);

		return reward;
	}
	CC_SAFE_DELETE(reward);
	return NULL;
}

RewardSugar::~RewardSugar() {}

void RewardSugar::animate() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto* move1 = EaseOut::create(MoveBy::create(0.25f, Vec2(0, 75)), 2);
	auto* move2 = EaseBounceOut::create(MoveBy::create(1.0f, Vec2(0, -75)));

	MoveBy* move_h1 = nullptr;
	MoveBy* move_h2 = nullptr;
	if (_position.x - 60 < 0) {
		move_h1 = MoveBy::create(0.25f, Vec2(25, 0));
		move_h2 = MoveBy::create(1.0f, Vec2(75, 0));
	}
	else {
		move_h1 = MoveBy::create(0.25f, Vec2(-15, 0));
		move_h2 = MoveBy::create(1.0f, Vec2(-45, 0));
	}
	EaseIn* move = EaseIn::create(MoveTo::create(1.5f, Vec2(visibleSize.width / 2, visibleSize.height)), 2);
	EaseIn* scale = EaseIn::create(ScaleTo::create(1.5f, 0.01f), 2);

	runAction(Sequence::create(Spawn::createWithTwoActions(move1, move_h1),
		Spawn::createWithTwoActions(move2, move_h2),
		Spawn::createWithTwoActions(move, scale), nullptr));
}
