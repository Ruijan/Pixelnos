#include "RewardTower.h"
#include "../../../AppDelegate.h"

USING_NS_CC;

RewardTower::RewardTower(std::string towername) : VisualReward(), tower_name(towername) {}

RewardTower* RewardTower::create(std::string nfile, std::string towername, cocos2d::Vec2 pos,
	const cocos2d::ui::Widget::ccWidgetTouchCallback& callback, GUISettings* settings) {
	RewardTower* reward = new RewardTower(towername);
	if (reward->init()) {
		Size visibleSize = settings->getVisibleSize();

		auto new_tower_title = Label::createWithTTF(settings->getButton("new_tower"),
			"fonts/LICABOLD.ttf",
			90.f * visibleSize.width / 1280);
		new_tower_title->setPosition(Vec2(visibleSize.width * 3 / 8, visibleSize.height * 3 / 4));
		new_tower_title->setColor(Color3B::YELLOW);
		new_tower_title->enableOutline(Color4B::BLACK, 2);
		new_tower_title->setScale(0.1f);
		new_tower_title->setOpacity(0.0f);
		reward->addChild(new_tower_title, 2, "new_tower_title");

		Node* tower_layout = Node::create();
		auto tower = ui::Button::create(nfile);
		tower->setScale(Cell::getCellWidth() / tower->getContentSize().width);
		tower->addTouchEventListener(callback);
		Sprite* shining = Sprite::create("res/buttons/shining.png");
		shining->setScale(1.5 * tower->getContentSize().width / shining->getContentSize().width);
		shining->setPosition(Vec2(tower->getContentSize().width / 2, tower->getContentSize().height / 2));
		tower->addChild(shining, -1, "shining");
		tower_layout->addChild(tower, 1, "tower");
		tower_layout->setPosition(pos);
		reward->addChild(tower_layout, 1, "tower_layout");

		return reward;
	}
	CC_SAFE_DELETE(reward);
	return NULL;
}

RewardTower::~RewardTower() {}

void RewardTower::animate() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto* move1 = EaseOut::create(MoveBy::create(0.25f, Vec2(0, 75)), 2);
	auto* move2 = EaseBounceOut::create(MoveBy::create(1.0f, Vec2(0, -75)));
	ScaleBy* scale_to1 = ScaleBy::create(1.5f, 1.5f);
	ScaleBy* scale_to2 = ScaleBy::create(1.5f, 1 / 1.5f);
	RotateBy* rotate = RotateBy::create(3.f, 360);
	getChildByName("tower_layout")->getChildByName("tower")->getChildByName("shining")->runAction(RepeatForever::create(Spawn::create(rotate, Sequence::create(scale_to1, scale_to2, nullptr), nullptr)));

	MoveBy* move_h1 = nullptr;
	MoveBy* move_h2 = nullptr;
	if (60 + _position.x < visibleSize.width * 2 / 3) {
		move_h1 = MoveBy::create(0.25f, Vec2(15, 0));
		move_h2 = MoveBy::create(1.0f, Vec2(45, 0));
	}
	else {
		move_h1 = MoveBy::create(0.25f, Vec2(-25, 0));
		move_h2 = MoveBy::create(1.0f, Vec2(-75, 0));
	}
	auto* scale = EaseOut::create(ScaleTo::create(1.0f, visibleSize.width / 6 /
		getChildByName("tower_layout")->getChildByName("tower")->getContentSize().width), 2);
	auto* scale_text = EaseOut::create(ScaleTo::create(1.0f, 1.0f), 2);

	// Since the level takes 3/4 of the screen. The center of the level is at 3/8.
	auto* movetocenter = EaseIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width * 3 / 8,
		visibleSize.height / 3)), 2);

	getChildByName("tower_layout")->runAction(
		Sequence::create(
			Spawn::createWithTwoActions(move1->clone(), move_h1),
			Spawn::createWithTwoActions(move2->clone(), move_h2),
			Spawn::create(
				TargetedAction::create(getChildByName("tower_layout")->getChildByName("tower"), scale),
				movetocenter,
				TargetedAction::create(getChildByName("new_tower_title"), scale_text),
				TargetedAction::create(getChildByName("new_tower_title"), FadeIn::create(0.5f)),
				nullptr),
			nullptr));
}