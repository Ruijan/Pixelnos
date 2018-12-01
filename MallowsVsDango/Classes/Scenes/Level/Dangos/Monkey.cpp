#include "Monkey.h"
#include "../Cell.h"
#include "../../../SceneManager.h"

Monkey * Monkey::create(std::vector<Tower*>& towers, std::vector<int>& blockedTowerIndices)
{
	Monkey* monkey = new Monkey();
	monkey->init(towers, blockedTowerIndices);
	return monkey;
}

void Monkey::init(std::vector<Tower*>& towers, std::vector<int>& tower_indices) {
	targetTower = getFreeTowerToBlock(towers, tower_indices);
	targetTower->blockTower();
	toRemove = false;

	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	auto* showAction = cocos2d::EaseBackIn::create(cocos2d::MoveTo::create(0.5f, targetTower->getPosition()));

	liane = cocos2d::Sprite::create("res/dango/liane.png");
	liane->setScale(Cell::getCellWidth() * 0.8 / liane->getContentSize().width);
	liane->setPosition(cocos2d::Vec2(targetTower->getPosition().x, visibleSize.height / 2 + liane->getContentSize().height / 2));
	liane->setAnchorPoint(cocos2d::Vec2(0.5, 0));
	liane->runAction(cocos2d::Sequence::createWithTwoActions(cocos2d::DelayTime::create(0.25f), showAction->clone()));

	shadow = cocos2d::Sprite::create("res/buttons/monkey_shadow.png");
	shadow->setPosition(targetTower->getPosition());
	shadow->setScale(Cell::getCellWidth() / shadow->getContentSize().width * 0.01f);
	shadow->runAction(cocos2d::ScaleTo::create(0.5f, Cell::getCellWidth() / shadow->getContentSize().width));

	monkey = cocos2d::ui::Button::create("res/dango/monkey.png");
	monkey->setScale(Cell::getCellWidth() / monkey->getContentSize().width);
	monkey->setPosition(cocos2d::Vec2(targetTower->getPosition().x,
		visibleSize.height + monkey->getContentSize().height * monkey->getScaleY() * 0.6));

	monkey->runAction(cocos2d::Sequence::createWithTwoActions(cocos2d::DelayTime::create(0.25f), showAction));
	monkey->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			goAway();
		}
	});

	addChild(liane);
	addChild(shadow);
	addChild(monkey);
}

void Monkey::goAway()
{
	targetTower->freeTower();
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto* hideAction = cocos2d::EaseBackIn::create(cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(monkey->getPosition().x, visibleSize.height +
		monkey->getContentSize().height * monkey->getScaleY() * 0.6)));
	auto delete_monkey = cocos2d::CallFunc::create([this]() {
		toRemove = true;
	});
	monkey->runAction(cocos2d::Sequence::createWithTwoActions(hideAction, delete_monkey));
	liane->runAction(cocos2d::EaseBackIn::create(cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(liane->getPosition().x, visibleSize.height))));
	shadow->runAction(cocos2d::ScaleTo::create(0.5f, Cell::getCellWidth() / shadow->getContentSize().width * 0.01f));
	monkey->setEnabled(false);
}

bool Monkey::hasToBeRemoved() {
	return toRemove;
}

Tower* Monkey::getFreeTowerToBlock(std::vector<Tower *> &towers, std::vector<int> &tower_indices)
{
	int index;
	bool already_index = false;
	do {
		already_index = false;
		index = rand() % towers.size();
		if (!towers[index]->isTowerBlocked()) {
			for (unsigned int j(0); j < tower_indices.size(); ++j) {
				if (tower_indices[j] == index) {
					already_index = true;
				}
			}
		}
		else {
			already_index = true;
		}
	} while (already_index);
	tower_indices.push_back(index);
	return towers[index];
}