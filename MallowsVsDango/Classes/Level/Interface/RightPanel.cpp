#include "RightPanel.h"
#include "../../AppDelegate.h"
#include "../../GUI/ParametersMenu.h"

RightPanel * RightPanel::create(MyGame * game, Config* config)
{
	RightPanel* rightPanel = new (std::nothrow) RightPanel();
	if (rightPanel && rightPanel->init(game, config))
	{
		rightPanel->autorelease();
		return rightPanel;
	}
	CC_SAFE_DELETE(rightPanel);
	return nullptr;
}

bool RightPanel::init(MyGame * myGame, Config* config) {
	this->game = myGame;
	configClass = config;
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	setPosition(cocos2d::Vec2(visibleSize.width * (3 / 4.0 + 1 / 8.0), visibleSize.height / 2));

	createPanel();
	createButtons();
	createTowersLayout();
	createTowerInfo();
	return true;
}

void RightPanel::createPanel() {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	auto panel = cocos2d::Sprite::create("res/buttons/menupanel5.png");
	panel->setScaleY(visibleSize.height / panel->getContentSize().height * 1.025);
	panel->setScaleX(visibleSize.width / panel->getContentSize().width / 4);
	addChild(panel, 2, "panel");
}

void RightPanel::createButtons() {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto panel = getChildByName("panel");
	const double sizeButton = visibleSize.width / 15;

	auto parameters = cocos2d::ui::Button::create("res/buttons/parameters_button.png");
	parameters->addTouchEventListener([&](cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			game->getMenu()->getPauseMenu()->displayWithAnimation();
			game->pause();
		}
	});
	parameters->setScale(1 / (parameters->getBoundingBox().size.width / sizeButton));
	parameters->setPosition(cocos2d::Vec2(panel->getContentSize().width*panel->getScaleX() * 0.3,
		panel->getContentSize().height*panel->getScaleY() * 0.48 -
		parameters->getContentSize().height*parameters->getScaleY() / 2));
	addChild(parameters, 2, "parameters");

	auto speed_up = cocos2d::ui::CheckBox::create("res/buttons/speed_up.png", "res/buttons/normal_speed.png", cocos2d::ui::Widget::TextureResType::LOCAL);
	speed_up->addTouchEventListener([&, speed_up](cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			if (!speed_up->isSelected()) {
				game->increaseSpeed();
			}
			else {
				game->setNormalSpeed();
			}
		}
	});
	speed_up->setScale(1 / (speed_up->getBoundingBox().size.width / sizeButton));
	speed_up->setPosition(cocos2d::Vec2(0,
		panel->getContentSize().height*panel->getScaleY() * 0.48 -
		speed_up->getContentSize().height*speed_up->getScaleY() / 2));
	addChild(speed_up, 2, "speed_up");

	auto reload = cocos2d::ui::Button::create("res/buttons/restart_button.png");
	reload->addTouchEventListener([&](cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			game->updateTracker("reloaded");
			game->setReloading(true);
		}
	});
	reload->setScale(1 / (reload->getBoundingBox().size.width / sizeButton));
	reload->setPosition(cocos2d::Vec2(-panel->getContentSize().width*panel->getScaleX() * 0.3,
		panel->getContentSize().height*panel->getScaleY() * 0.48 -
		reload->getContentSize().height*reload->getScaleY() / 2));
	addChild(reload, 2, "reload");
}

void RightPanel::createTowersLayout() {
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	towerLayout = cocos2d::Layer::create();
	towerLayout->setContentSize(cocos2d::Size(visibleSize.width / 4 * 0.9, visibleSize.height * 3 / 5));
	towerLayout->setPosition(cocos2d::Vec2(-visibleSize.width / 8 * 0.9, getChildByName("reload")->getPosition().y -
		getChildByName("reload")->getContentSize().height * getChildByName("reload")->getScaleY() * 2 / 3));
	towerLayout->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	addChild(towerLayout, 2, "towers");

	towerSize = cocos2d::Director::getInstance()->getVisibleSize().width / 10;
	auto save = configClass->getSaveValues()["towers"];

	int j(0);
	std::vector<std::string> tower_names = Tower::getConfig().getMemberNames();
	Json::Value tower_config = Tower::getConfig();
	for (unsigned int i(0); i < tower_names.size(); ++i) {
		if (save[save.getMemberNames()[i]]["unlocked"].asBool()) {
			std::string sprite3_filename = tower_config[tower_names[i]]["image"].asString();
			std::string sprite3_background_filename = "res/buttons/tower_button2.png";
			std::string sprite3_disable_filename = "res/buttons/tower_button2_disabled.png";

			auto tower = cocos2d::Sprite::create(sprite3_background_filename);
			cocos2d::Sprite* inactive_background = cocos2d::Sprite::create(sprite3_disable_filename);
			inactive_background->setScale(tower->getContentSize().width / inactive_background->getContentSize().width);
			inactive_background->setPosition(cocos2d::Vec2(tower->getContentSize().width / 2,
				tower->getContentSize().height / 2));
			tower->addChild(inactive_background, 4, "disable");
			tower->addChild(cocos2d::Sprite::create(sprite3_filename), 5, "sprite");
			towerLayout->addChild(tower, 1, Json::Value(tower_names[i]).asString());
			tower->setScale(towerSize / tower->getContentSize().width);
			tower->getChildByName("sprite")->setScale(tower->getContentSize().width * 0.75 /
				tower->getChildByName("sprite")->getContentSize().width);
			tower->setPosition(cocos2d::Vec2(towerSize / 2 + j % 2 * (towerSize + towerLayout->getContentSize().width / 20) +
				towerLayout->getContentSize().width / 40,
				-tower->getContentSize().height * tower->getScale() / 2 - (j / 2) *
				(tower->getContentSize().height * tower->getScale() + towerLayout->getContentSize().width / 20) -
				towerLayout->getContentSize().width / 20));
			tower->getChildByName("sprite")->setPosition(cocos2d::Vec2(tower->getContentSize().width / 2,
				tower->getContentSize().height * 0.55));

			cocos2d::Label* cost = cocos2d::Label::createWithTTF(tower_config[tower_names[i]]["cost"][0].asString(),
				"fonts/LICABOLD.ttf", 25);
			cost->setColor(cocos2d::Color3B::RED);
			cost->setPosition(cocos2d::Vec2(tower->getContentSize().width / 2 + cost->getContentSize().width / 2,
				cost->getContentSize().height));
			tower->addChild(cost, 6, "cost");

			cocos2d::Sprite* sugar = cocos2d::Sprite::create("res/buttons/sugar.png");
			sugar->setScale(tower->getContentSize().width / 5 / sugar->getContentSize().width);
			sugar->setPosition(cocos2d::Vec2(cost->getPosition().x - cost->getContentSize().width / 2 - sugar->getContentSize().width * sugar->getScale() / 2,
				cost->getPosition().y));
			tower->addChild(sugar, 6, "sugar");

			towers[tower_names[i]] = std::make_pair(tower, tower_config[tower_names[i]]["cost"][0].asDouble());
			++j;
		}
	}
}

void RightPanel::createTowerInfo() {
	auto informations = cocos2d::ui::Layout::create();
	auto panel = getChildByName("panel");
	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	const double sizeButton = visibleSize.width / 15;

	informations->setContentSize(cocos2d::Size(towerLayout->getContentSize().width, panel->getContentSize().height / 4));
	informations->setPosition(cocos2d::Vec2(towerLayout->getPosition().x + towerLayout->getContentSize().width / 0.9 / 2,
		towerLayout->getPosition().y - towerLayout->getContentSize().height));
	informations->setVisible(false);

	cocos2d::Node* animation = cocos2d::Node::create();
	informations->addChild(animation, 1, "animation");

	cocos2d::Sprite* attack = cocos2d::Sprite::create("res/buttons/attack.png");
	cocos2d::Sprite* speed = cocos2d::Sprite::create("res/buttons/speed.png");
	cocos2d::Sprite* range = cocos2d::Sprite::create("res/buttons/range.png");

	attack->setScale(informations->getContentSize().width / 10 / attack->getContentSize().width);
	speed->setScale(informations->getContentSize().width / 10 / speed->getContentSize().width);
	range->setScale(informations->getContentSize().width / 10 / range->getContentSize().width);

	attack->setPosition(0, 0);
	speed->setPosition(0, attack->getPosition().y - attack->getContentSize().height*
		attack->getScaleY() / 2 - speed->getContentSize().height * speed->getScaleY() * 3 / 4);
	range->setPosition(0, speed->getPosition().y - speed->getContentSize().height*
		speed->getScaleY() / 2 - range->getContentSize().height * range->getScaleY() * 3 / 4);
	animation->setPosition(-informations->getContentSize().width * 0.33, range->getPosition().y);

	informations->addChild(attack, 1, "attack");
	informations->addChild(speed, 1, "speed");
	informations->addChild(range, 1, "range");

	cocos2d::Label* attack_label = cocos2d::Label::createWithTTF("10", "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	attack_label->setColor(cocos2d::Color3B::BLACK);
	attack_label->setPosition(cocos2d::Vec2(attack->getPosition().x + attack->getContentSize().width * attack->getScale(),
		attack->getPosition().y));
	attack_label->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	informations->addChild(attack_label, 1, "attack_label");

	cocos2d::Label* range_label = cocos2d::Label::createWithTTF("2", "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	range_label->setColor(cocos2d::Color3B::BLACK);
	range_label->setPosition(cocos2d::Vec2(range->getPosition().x + attack->getContentSize().width * attack->getScale(),
		range->getPosition().y));
	range_label->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	informations->addChild(range_label, 1, "range_label");

	cocos2d::Label* speed_label = cocos2d::Label::createWithTTF("1.5", "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	speed_label->setColor(cocos2d::Color3B::BLACK);
	speed_label->setPosition(cocos2d::Vec2(speed->getPosition().x + attack->getContentSize().width * attack->getScale(),
		speed->getPosition().y));
	speed_label->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
	informations->addChild(speed_label, 1, "speed_label");

	cocos2d::Label* description_tower = cocos2d::Label::createWithTTF("Sword Master: Attack all the dangos around him. Unavoidable damages.",
		"fonts/LICABOLD.ttf",
		20 * visibleSize.width / 1280);
	description_tower->setDimensions(informations->getContentSize().width, informations->getContentSize().height * 2 / 5);
	description_tower->setColor(cocos2d::Color3B::BLACK);
	description_tower->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
	description_tower->setPosition(cocos2d::Vec2(0, range->getPosition().y -
		range->getContentSize().height * range->getScaleY() -
		description_tower->getDimensions().height / 2));
	informations->addChild(description_tower, 1, "description_tower");

	cocos2d::Sprite* sugar = cocos2d::Sprite::create("res/buttons/sugar.png");
	sugar->setScale(sizeButton / 2 / sugar->getContentSize().width);
	sugar->setPosition(cocos2d::Vec2(0, description_tower->getPosition().y - sugar->getContentSize().height * sugar->getScaleY() / 2));
	sugar->setAnchorPoint(cocos2d::Vec2(1.f, 0.5f));
	informations->addChild(sugar, 1, "sugar");

	cocos2d::Label* sugar_label = cocos2d::Label::createWithTTF("30", "fonts/LICABOLD.ttf", 35 * visibleSize.width / 1280);
	sugar_label->setColor(cocos2d::Color3B::GREEN);
	sugar_label->setPosition(cocos2d::Vec2(sugar->getPosition().x, sugar->getPosition().y));
	sugar_label->setAnchorPoint(cocos2d::Vec2(0, 0.5f));
	sugar_label->enableOutline(cocos2d::Color4B::BLACK, 1);
	informations->addChild(sugar_label, 1, "sugar_label");

	addChild(informations, 2, "informations");
}

void RightPanel::resetAnimations()
{
	for (auto it : towers) {
		it.second.first->stopAllActions();
		it.second.first->setRotation(0);
	}
}

void RightPanel::reset()
{
	getChildByName("informations")->setVisible(false);
	((cocos2d::ui::CheckBox*)getChildByName("speed_up"))->setSelected(false);
	towers.clear();
	towerLayout->removeAllChildren();
	createTowersLayout();
}

void RightPanel::update() {
	for (auto& tower : towers) {
		auto cost_label = (cocos2d::Label*)tower.second.first->getChildByName("cost");
		double cost = tower.second.second;
		double quantity = game->getLevel()->getQuantity();
		if (quantity < cost) {
			if (cost_label->getColor() != cocos2d::Color3B::RED) {
				cost_label->setColor(cocos2d::Color3B::RED);
				cost_label->enableOutline(cocos2d::Color4B::BLACK, 1.0);
			}
			if (!tower.second.first->getChildByName("disable")->isVisible()) {
				tower.second.first->getChildByName("disable")->setVisible(true);
				tower.second.first->getChildByName("sprite")->setColor(cocos2d::Color3B::GRAY);
			}
		}
		else {
			if (cost_label->getColor() != cocos2d::Color3B::GREEN) {
				cost_label->setColor(cocos2d::Color3B::GREEN);
				cost_label->enableOutline(cocos2d::Color4B::BLACK, 1.0);
			}
			if (tower.second.first->getChildByName("disable")->isVisible()) {
				tower.second.first->getChildByName("disable")->setVisible(false);
				tower.second.first->getChildByName("sprite")->setColor(cocos2d::Color3B::WHITE);
			}
		}
	}
}

std::pair<std::string, cocos2d::Sprite*> RightPanel::getTowerFromPoint(cocos2d::Vec2 location) {
	cocos2d::Vec2 origin = getBoundingBox().origin + towerLayout->getPosition();
	for (auto& item : towers) {
		cocos2d::Vec2 pointInSprite = location - item.second.first->getPosition() - origin;
		pointInSprite.x += item.second.first->getSpriteFrame()->getRect().size.width *
			item.second.first->getScale() / 2;
		pointInSprite.y += item.second.first->getSpriteFrame()->getRect().size.height *
			item.second.first->getScale() / 2;
		cocos2d::Rect itemRect = cocos2d::Rect(0, 0, item.second.first->getSpriteFrame()->getRect().size.width *
			item.second.first->getScale(),
			item.second.first->getSpriteFrame()->getRect().size.height * item.second.first->getScale());
		if (itemRect.containsPoint(pointInSprite)) {
			return std::make_pair(item.first, item.second.first);
		}
	}
	std::pair<std::string, cocos2d::Sprite*> item;
	item.first = "nullptr";
	return item;
}

void RightPanel::displayTowerInfos(std::string item_name, const std::string& language) { //configClass->getSettings()->getLanguage()
	if (item_name != "") {
		getChildByName("informations")->setVisible(true);
		cocos2d::Node* batch = getChildByName("informations")->getChildByName("animation");
		batch->removeAllChildren();

		cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
		Json::Value tower_config = Tower::getConfig()[item_name];
		unsigned int required_quantity(0);
		((cocos2d::Label*)getChildByName("informations")->getChildByName("attack_label"))->setString(
			tower_config["damages"][0].asString());
		std::string s = tower_config["attack_speed"][0].asString();
		s.resize(4);
		((cocos2d::Label*)getChildByName("informations")->getChildByName("speed_label"))->setString(s);
		double range = round(tower_config["range"][0].asDouble() / Cell::getCellWidth() * 100) / 100;
		s = Json::Value(range).asString();
		s.resize(4);
		((cocos2d::Label*)getChildByName("informations")->getChildByName("range_label"))->setString(
			s);
		((cocos2d::Label*)getChildByName("informations")->getChildByName("description_tower"))->setString(
			tower_config["description_" + language].asString());
		((cocos2d::Label*)getChildByName("informations")->getChildByName("sugar_label"))->setString(
			tower_config["cost"][0].asString());
		required_quantity = tower_config["cost"][0].asDouble();
		if (game->getLevel()->getQuantity() < required_quantity) {
			((cocos2d::Label*)getChildByName("informations")->getChildByName("sugar_label"))->setColor(cocos2d::Color3B::RED);
		}
		else {
			((cocos2d::Label*)getChildByName("informations")->getChildByName("sugar_label"))->setColor(cocos2d::Color3B::GREEN);
		}

		auto animated_skeleton = Tower::getSkeletonAnimationFromName(item_name);
		animated_skeleton->setScale(
			getChildByName("informations")->getContentSize().width *
			0.40 / animated_skeleton->getSkeleton()->data->width);
		batch->addChild(animated_skeleton);
	}
	else {
		getChildByName("informations")->setVisible(false);
	}

}

cocos2d::Vec2 RightPanel::getAbsoluteMenuTowerPosition(std::string towerName)
{
	return towers[towerName].first->getPosition() - cocos2d::Vec2(0, towerSize) + towerLayout->getPosition() + getPosition();
}

cocos2d::Sprite* RightPanel::getTower(std::string towerName)
{
	return towers[towerName].first;
}