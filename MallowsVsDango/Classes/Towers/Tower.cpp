#include "Tower.h"
#include "../Level/Cell.h"
#include "../Dangos/Dango.h"
#include "../Towers/Attack.h"
#include "../AppDelegate.h"
#include "../InterfaceGame.h"

USING_NS_CC;

Tower::Tower(double nspeed, double ndamage, double nrange, double ncost) :
state(State::IDLE), fixed(false), destroy(false), target(nullptr), cost(ncost), 
attackSpeed(nspeed), damage(ndamage), range(nrange), timer(0), timerIDLE(0), level(0), 
currentAction(nullptr), nb_attacks(0)
{}

Tower::~Tower() {}

void Tower::initDebug(){
	setScale(Cell::getCellWidth() / getContentSize().width);
	autorelease();

	loadingCircle = DrawNode::create();
	loadingCircle->setVisible(false);
	addChild(loadingCircle);
		
	DrawNode *circle = DrawNode::create();
	circle->setPosition(Vec2(getSpriteFrame()->getRect().size.width / 2,
		getSpriteFrame()->getRect().size.height / 2));
	circle->drawCircle(Vec2(0, 0), getRange() / getScaleX(),
		0, 60, false, 1, 1, Color4F(0, 0, 0, 1));
	circle->setVisible(false);
	addChild(circle, 0, "range");
}


void Tower::destroyCallback(Ref* sender){
	destroy = true;
	SceneManager::getInstance()->getGame()->getMenu()->removeTower();
}

void Tower::builtCallback(Ref* sender){
	fixed = true;
}

void Tower::upgradeCallback(Ref* sender){
	if(level < (int)getSpecConfig()["cost"].size()){
		++level;
		range = getSpecConfig()["range"][level].asDouble();
		damage = getSpecConfig()["damages"][level].asDouble();
		attackSpeed = getSpecConfig()["attack_speed"][level].asDouble();
		
		removeChild(getChildByName("range"),true);
		DrawNode *circle = DrawNode::create();
		circle->setPosition(Vec2(getSpriteFrame()->getRect().size.width / 2,
			getSpriteFrame()->getRect().size.height / 2));
		circle->drawCircle(Vec2(0, 0), getRange() / getScaleX(),
			360, 60, false, 1, 1, Color4F(0, 0, 0, 1));
		addChild(circle, 0, "range");
	}
}


bool Tower::isFixed(){
	return fixed;
}

void Tower::setFixed(bool f){
	fixed = f;
}

bool Tower::isSelected(){
	return selected;
}

void Tower::removeTarget(Dango* dango) {
	if (target == dango) {
		target = nullptr;
	}
}

void Tower::chooseTarget(std::vector<Dango*> targets){
	if(state != State::ATTACKING && state != State::RELOADING){
		double bestScore(0);
		bool chosen = false;

		for(auto& cTarget : targets){
			if(cTarget != nullptr){
				int first = cTarget->getTargetedCell();
				double dist = cTarget->getPosition().distanceSquared(this->getPosition());
				double minDist = pow(getRange(), 2);
				if (first > bestScore && dist <= minDist && cTarget->willBeAlive()){
					bestScore = first;
					if (target != nullptr) {
						target->removeTargetingTower(this);
					}
					target = cTarget;
					target->addTargetingTower(this);
					chosen = true;
				}
			}
		}
		if(!chosen){
			if (target != nullptr) {
				target->removeTargetingTower(this);
			}
			target = nullptr;
		}
	}
	else if(state == State::RELOADING){
		if (target != nullptr) {
			target->removeTargetingTower(this);
		}
		target = nullptr;
	}
}
void Tower::givePDamages(double damage){
	target->takePDamages(damage);
}

double Tower::getRange(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	return range * visibleSize.width / 960.0;
}

double Tower::getCost(){
	return cost;
}

Dango* Tower::getTarget(){
	return target;
}

double Tower::getDamage(){
	return damage;
}

double Tower::getAttackSpeed(){
	return attackSpeed;
}

cocos2d::Vector<SpriteFrame*> Tower::getAnimation(Tower::State animState){
	std::string action("");
	switch(animState){
		case IDLE:
			action = "steady";
			break;
		case ATTACKING:
			action = "attack";
			break;
		default:
			std::cerr << "No animation found for this state.";
			std::cerr << "Steady state animation used instead." << std::endl;
			action = "steady";
			break;
	};
	
	unsigned int animation_size = getSpecConfig()["animation_"+ action +"_size"].asInt();
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	for (unsigned int i = 0; i <= animation_size; ++i){
		std::string frameName =  getSpecConfig()["name"].asString()+"_"+action+"_movement_%03d.png";
		sprintf(str, frameName.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	return animFrames;
}

cocos2d::Vector<SpriteFrame*> Tower::getAnimationFromName(std::string name, Tower::State animState){
	std::string action("");
	switch(animState){
		case IDLE:
			action = "steady";
			break;
		case ATTACKING:
			action = "attack";
			break;
		default:
			log("No animation found for this state.");
			log("Steady state animation used instead.");
			action = "steady";
			break;
	};
	
	unsigned int animation_size = Tower::getConfig()[name]["animation_"+ action +"_size"].asInt();
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	for (unsigned int i = 0; i <= animation_size; ++i)
	{
		std::string frameName =  name+"_"+action+"_movement_%03d.png";
		sprintf(str, frameName.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	return animFrames;
}

void Tower::update(float dt) {
	// display update
	updateDisplay(dt);
	
	if(fixed){
		timer += dt;
		switch(state){
			case State::IDLE:
				if (target != nullptr){
					state = State::AWARE;
				}
				break;
			case State::AWARE:
				timerIDLE += dt;
				if (target != nullptr){
					state = State::ATTACKING;
					givePDamages(damage);
					startAnimation();
					timerIDLE = 0;
				}
				if(timerIDLE > 2){
					state = State::IDLE;
					timerIDLE = 0;
					startAnimation();
				}
				break;
			case State::ATTACKING:
				timerIDLE += dt;
				if(currentAction->isDone()){
					currentAction->release();
					state = State::RELOADING;
					std::string frameName =  getSpecConfig()["name"].asString()+"_attack_movement_000.png";
					SpriteFrameCache* cache = SpriteFrameCache::getInstance();
					setSpriteFrame(cache->getSpriteFrameByName(frameName.c_str()));
					timer = 0;
					timerIDLE = 0;
					if(target != nullptr){
						attack();
						if (((AppDelegate*)Application::getInstance())->getSave()["auto_limit"].asBool()) {
							startLimit();
						}
					}
				}
				if(timerIDLE > 2){
					log("ERROR WITH TIMER !!!");
				}
				break;
			case State::RELOADING:
				reload();
				break;
			default:
				state = IDLE;
				break;
		}
	}
	else{
		
	}
}

void Tower::updateDisplay(float dt){
	if(!fixed){
		float opacity = getOpacity();
		if (state == BLINKING_UP){
			if(opacity < 250){
				opacity = getOpacity() + 510 * dt < 250 ? getOpacity() + 510 * dt : 250;
				setOpacity(opacity);
			}
			else{
				state = BLINKING_DOWN;
			}
		}
		else if(state == BLINKING_DOWN){
			if(opacity > 50){
				opacity = getOpacity() + 510 * dt >50 ? getOpacity() - 510 * dt : 50;
				setOpacity(opacity);
			}
			else{
				state = BLINKING_UP;
			}
		}
		else{
			state = BLINKING_UP;
		}
	}
	else{
		setOpacity(255);
	}
}

bool Tower::isLimitReached() {
	return nb_attacks >= getSpecConfig()["limit"].asInt();
}

bool Tower::hasToBeDestroyed(){
	return destroy;
}

void Tower::displayRange(bool disp){
	getChildByName("range")->setVisible(disp);
	loadingCircle->setVisible(disp);
}

void Tower::startAnimation(float speed){
	if(currentAction != nullptr){
		stopAllActions();
	}
	cocos2d::Vector<SpriteFrame*> animFrames = getAnimation(state);
	double delay = getSpecConfig()["animation_attack_time"].asDouble() / getSpecConfig()["animation_attack_size"].asDouble() / speed;
	Animation* currentAnimation = Animation::createWithSpriteFrames(animFrames, delay);
	currentAction = runAction(Animate::create(currentAnimation));
	currentAction->retain();
}

Json::Value Tower::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["towers"];
}

void Tower::reload(){
	
	if (timer > attackSpeed){
		state = State::AWARE;
	}
	else{
		double iniAngle = M_PI_2;
		loadingCircle->clear();
		loadingCircle->drawSolidCircle(Vec2(0, 0), 0.11 * 50 / getScaleX(),
			0, 60, 1, 1, Color4F(0, 0, 0, 1));
		loadingCircle->drawSolidCircle(Vec2(0, 0), 0.1 * timer / attackSpeed * 50 / getScaleX(),
			0, 60, 1, 1, Color4F(1 - timer / attackSpeed, timer / attackSpeed, 0, 1));		
	}
}

Tower::TowerType Tower::getTowerTypeFromString(std::string type){
	if (!strcmp(type.c_str(), "archer")){
		return Tower::TowerType::ARCHER;
	}
	else if (!strcmp(type.c_str(), "cutter")){
		return Tower::TowerType::CUTTER;
	}
	else{
		return Tower::TowerType::ARCHER;
	}
}

Tower::State Tower::getState(){
	return state;
}

void Tower::setState(Tower::State nstate){
	state = nstate;
}

int Tower::getLevel(){
	return level;
}

void Tower::setTarget(Dango* dango){
	target = dango;
}

void Tower::setSelected(bool select){
	selected = select;
}

ui::Layout* Tower::getInformationLayout(InterfaceGame* interface_game) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto layout = ui::Layout::create();

	auto panel = ui::Button::create("res/buttons/wood_information_panel.png");
	panel->setScaleX(visibleSize.width / 4 / panel->getContentSize().width);
	panel->setScaleY(visibleSize.width / 3 / panel->getContentSize().width);
	panel->setZoomScale(0);
	layout->addChild(panel, 0);

	auto current_level_layout = ui::Layout::create();
	current_level_layout->setContentSize(Size(panel->getContentSize().width * panel->getScaleX() / 3,
		panel->getContentSize().height * panel->getScaleY() * 3 / 4));
	current_level_layout->setPosition(Vec2(-panel->getContentSize().width * panel->getScaleX() / 2,
		current_level_layout->getContentSize().height / 2));

	Label* level_label = Label::createWithTTF("Level " + Value(level + 1).asString(), "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	level_label->setColor(Color3B::BLACK);
	level_label->setPosition(Vec2(current_level_layout->getContentSize().width / 2,
		-level_label->getContentSize().height / 2));
	current_level_layout->addChild(level_label, 1, "level_label");

	Sprite* attack = Sprite::create("res/buttons/attack.png");
	Sprite* speed = Sprite::create("res/buttons/speed.png");
	Sprite* range_i = Sprite::create("res/buttons/range.png");
	current_level_layout->addChild(attack, 1, "attack");
	current_level_layout->addChild(speed, 1, "speed");
	current_level_layout->addChild(range_i, 1, "range");

	double size_sprite = panel->getContentSize().width * panel->getScaleX() / 10;

	attack->setScale(size_sprite / attack->getContentSize().width);
	speed->setScale(size_sprite / speed->getContentSize().width);
	range_i->setScale(size_sprite / range_i->getContentSize().width);
	
	attack->setPosition(size_sprite, level_label->getPosition().y -
		level_label->getContentSize().height / 2
		-attack->getContentSize().height*attack->getScaleY() / 2);
	speed->setPosition(size_sprite, attack->getPosition().y - attack->getContentSize().height*
		attack->getScaleY() / 2 - speed->getContentSize().height * speed->getScaleY() * 3 / 4);
	range_i->setPosition(size_sprite, speed->getPosition().y - speed->getContentSize().height*
		speed->getScaleY() / 2 - range_i->getContentSize().height * range_i->getScaleY() * 3 / 4);

	std::string s = Value(damage).asString();
	s.resize(4);
	Label* attack_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	attack_label->setColor(Color3B::BLACK);
	attack_label->setPosition(Vec2(attack->getPosition().x + size_sprite,
		attack->getPosition().y));
	attack_label->setAnchorPoint(Vec2(0.f, 0.5f));
	current_level_layout->addChild(attack_label, 1, "attack_label");

	s = Value(round(range / Cell::getCellWidth() * 100) / 100).asString();
	s.resize(4);
	Label* range_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	range_label->setColor(Color3B::BLACK);
	range_label->setPosition(Vec2(range_i->getPosition().x + size_sprite,
		range_i->getPosition().y));
	range_label->setAnchorPoint(Vec2(0.f, 0.5f));
	current_level_layout->addChild(range_label, 1, "range_label");

	s = Value(attackSpeed).asString();
	s.resize(4);
	Label* speed_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	speed_label->setColor(Color3B::BLACK);
	speed_label->setPosition(Vec2(speed->getPosition().x + size_sprite,
		speed->getPosition().y));
	speed_label->setAnchorPoint(Vec2(0.f, 0.5f));
	current_level_layout->addChild(speed_label, 1, "speed_label");
	
	double position_x = getPosition().x;
	double position_y = getPosition().y + panel->getContentSize().height * panel->getScaleY() / 2 +
		getContentSize().height*getScaleY() / 2;
	if (position_x - panel->getContentSize().width * panel->getScaleX() / 2 < 0) {
		position_x += abs(position_x - panel->getContentSize().width * panel->getScaleX() / 2);
	}
	else if (position_x + panel->getContentSize().width * panel->getScaleX() / 2 > visibleSize.width * 3 / 4) {
		position_x -= abs(position_x + panel->getContentSize().width * panel->getScaleX() / 2 - visibleSize.width * 3 / 4);
	}
	if (position_y + panel->getContentSize().height * panel->getScaleY() / 2 > visibleSize.height) {
		position_y = getPosition().y - panel->getContentSize().height * panel->getScaleY() / 2 -
			getContentSize().height*getScaleY() / 2;
	}
	layout->setPosition(Vec2(position_x, position_y));
	layout->addChild(current_level_layout, 2, "current_level_layout");


	auto nextlevel_button = ui::Button::create("res/buttons/wood_nextlevel_panel.png","", "res/buttons/wood_nextlevel_disabled.png");
	nextlevel_button->setScaleX(panel->getContentSize().width * panel->getScaleX() * .95f / 
		nextlevel_button->getContentSize().width * 3 / 4);
	nextlevel_button->setScaleY(panel->getContentSize().height * panel->getScaleY() /
		nextlevel_button->getContentSize().height);
	nextlevel_button->setPosition(Vec2(panel->getContentSize().width * panel->getScaleX() / 6,
		panel->getContentSize().height * panel->getScaleY()/ 2 - 
		nextlevel_button->getContentSize().height * nextlevel_button->getScaleY() / 2));
	layout->addChild(nextlevel_button, 1, "next_level_button");
	nextlevel_button->addTouchEventListener([&, layout](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			MyGame* game = SceneManager::getInstance()->getGame();
			if ((int)game->getLevel()->getQuantity() >= getSpecConfig()["cost"][level + 1].asInt() &&
				level < (int)getSpecConfig()["cost"].size()) {
				game->getLevel()->decreaseQuantity(getSpecConfig()["cost"][level + 1].asInt());
				upgradeCallback(sender);
				if ((int)game->getLevel()->getQuantity() < getSpecConfig()["cost"][level + 1].asInt()) {
					((ui::Button*)layout->getChildByName("next_level_button"))->
						setEnabled(false);
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
						setColor(Color3B::RED);
				}
				else {
					((ui::Button*)layout->getChildByName("next_level_button"))->
						setEnabled(true);
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
						setColor(Color3B::YELLOW);
				}
				((Label*)layout->getChildByName("next_level_layout")->getChildByName("level_label"))->
					setString("Level " + Value(level + 2).asString());
				if (level + 1 < (int)getSpecConfig()["cost"].size() - 1) {
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("attack_label"))->
						setString(getSpecConfig()["damages"][level + 1].asString());
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("speed_label"))->
						setString(getSpecConfig()["attack_speed"][level + 1].asString());
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("range_label"))->
						setString(getSpecConfig()["range"][level + 1].asString());
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
						setString(getSpecConfig()["cost"][level + 1].asString());
				}
				else if (level >= (int)getSpecConfig()["damages"].size() - 2) {
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
						setString(getSpecConfig()["cost"][level + 1].asString());
					layout->getChildByName("next_level_layout")->getChildByName("range")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("speed")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("attack")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("range_label")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("speed_label")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("attack_label")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("description_label")->setVisible(true);
				}
				else {
					layout->getChildByName("next_level_layout")->setVisible(false);
					((ui::Button*)layout->getChildByName("next_level_button"))->
						setEnabled(false);
					layout->getChildByName("max_level_label")->setVisible(true);
				}
				((Label*)layout->getChildByName("current_level_layout")->getChildByName("level_label"))->
					setString("Level " + Value(level + 1).asString());
				((Label*)layout->getChildByName("current_level_layout")->getChildByName("attack_label"))->
					setString(getSpecConfig()["damages"][level].asString());
				((Label*)layout->getChildByName("current_level_layout")->getChildByName("speed_label"))->
					setString(getSpecConfig()["attack_speed"][level].asString());
				((Label*)layout->getChildByName("current_level_layout")->getChildByName("range_label"))->
					setString(getSpecConfig()["range"][level].asString());
				((Label*)layout->getChildByName("sell_label"))->setString(getSpecConfig()["sell"][level].asString());
			}
		}
	});

	auto nextlevel_layout = ui::Layout::create();
	nextlevel_layout->setContentSize(Size(panel->getContentSize().width * panel->getScaleX() / 3,
		panel->getContentSize().height * panel->getScaleY() * 3 / 4));
	nextlevel_layout->setPosition(Vec2(0, nextlevel_layout->getContentSize().height / 2));

	Label* level_n_label = Label::createWithTTF("Level " + Value(level + 2).asString(), 
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	level_n_label->setColor(Color3B::BLACK);
	level_n_label->setPosition(Vec2(nextlevel_layout->getContentSize().width * nextlevel_layout->getScaleX() / 2, 
		-level_n_label->getContentSize().height / 2));
	nextlevel_layout->addChild(level_n_label, 1, "level_label");

	Sprite* attack_n = Sprite::create("res/buttons/attack.png");
	Sprite* speed_n = Sprite::create("res/buttons/speed.png");
	Sprite* range_ni = Sprite::create("res/buttons/range.png");
	Sprite* cost = Sprite::create("res/buttons/sugar.png");

	nextlevel_layout->addChild(attack_n, 1, "attack");
	nextlevel_layout->addChild(speed_n, 1, "speed");
	nextlevel_layout->addChild(range_ni, 1, "range");
	nextlevel_layout->addChild(cost, 1, "cost");

	attack_n->setScale(size_sprite / attack_n->getContentSize().width);
	speed_n->setScale(size_sprite / speed_n->getContentSize().width);
	range_ni->setScale(size_sprite / range_ni->getContentSize().width);
	cost->setScale(size_sprite / cost->getContentSize().width);

	attack_n->setPosition(size_sprite, level_n_label->getPosition().y - 
		level_n_label->getContentSize().height / 2 -size_sprite / 2);
	speed_n->setPosition(size_sprite, attack_n->getPosition().y - size_sprite / 2 - size_sprite / 2);
	range_ni->setPosition(size_sprite, speed_n->getPosition().y - size_sprite / 2 - size_sprite / 2);

	auto description_label = Label::createWithTTF(getSpecConfig()["last_level_description"].asString(),
		"fonts/LICABOLD.ttf", 20 * visibleSize.width / 1280);
	description_label->setColor(Color3B::BLACK);
	description_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	description_label->setDimensions(nextlevel_layout->getContentSize().width * 2 * 0.85,
		nextlevel_layout->getContentSize().height * 3 / 4);
	description_label->setPosition(nextlevel_layout->getContentSize().width * nextlevel_layout->getScaleX() / 2,
		level_n_label->getPosition().y - level_n_label->getContentSize().height / 2 -
		description_label->getContentSize().height / 2);
	nextlevel_layout->addChild(description_label, 1, "description_label");

	auto upgrade_label = Label::createWithTTF("Upgrade",
		"fonts/LICABOLD.ttf", 30 * visibleSize.width / 1280);
	upgrade_label->setColor(Color3B::YELLOW);
	upgrade_label->enableOutline(Color4B::BLACK, 1);
	upgrade_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	upgrade_label->setPosition(nextlevel_layout->getContentSize().width * nextlevel_layout->getScaleX() / 2,
		range_ni->getPosition().y - range_ni->getContentSize().height * range_ni->getScaleY()/ 2 -
		upgrade_label->getContentSize().height / 2);
	nextlevel_layout->addChild(upgrade_label, 1, "upgrade");

	cost->setPosition(size_sprite, upgrade_label->getPosition().y - upgrade_label->getContentSize().height / 2 - size_sprite / 2);

	s = getSpecConfig()["damages"][level + 1].asString();
	s.resize(4);
	Label* attack_n_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	attack_n_label->setColor(Color3B::BLACK);
	attack_n_label->setPosition(Vec2(attack_n->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		attack_n->getPosition().y));
	attack_n_label->setAnchorPoint(Vec2(0.f, 0.5f));
	nextlevel_layout->addChild(attack_n_label, 1, "attack_label");

	s = Value(round(getSpecConfig()["range"][level + 1].asDouble() / Cell::getCellWidth() * 100) / 100).asString();
	s.resize(4);
	Label* range_n_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	range_n_label->setColor(Color3B::BLACK);
	range_n_label->setPosition(Vec2(range_ni->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		range_ni->getPosition().y));
	range_n_label->setAnchorPoint(Vec2(0.f, 0.5f));
	nextlevel_layout->addChild(range_n_label, 1, "range_label");

	s = getSpecConfig()["attack_speed"][level + 1].asString();
	s.resize(4);
	Label* speed_n_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	speed_n_label->setColor(Color3B::BLACK);
	speed_n_label->setPosition(Vec2(speed_n->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		speed_n->getPosition().y));
	speed_n_label->setAnchorPoint(Vec2(0.f, 0.5f));
	nextlevel_layout->addChild(speed_n_label, 1, "speed_label");

	s = Value(getSpecConfig()["cost"][level + 1].asInt()).asString();
	s.resize(4);
	Label* cost_n_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	cost_n_label->setColor(Color3B::YELLOW);
	cost_n_label->enableOutline(Color4B::BLACK, 1);
	cost_n_label->setPosition(Vec2(cost->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		cost->getPosition().y));
	cost_n_label->setAnchorPoint(Vec2(0.f, 0.5f));
	nextlevel_layout->addChild(cost_n_label, 1, "cost_label");

	layout->addChild(nextlevel_layout, 2, "next_level_layout");

	Label* max_level_label = Label::createWithTTF("Max. Level Reached", "fonts/LICABOLD.ttf", 35 * visibleSize.width / 1280);
	max_level_label->setColor(Color3B::BLACK);
	max_level_label->setAlignment(TextHAlignment::CENTER);
	max_level_label->setDimensions(nextlevel_layout->getContentSize().width, nextlevel_layout->getContentSize().height * 0.6);
	max_level_label->setAnchorPoint(Vec2(0, 0.5));
	max_level_label->setPosition(0,0);
	layout->addChild(max_level_label, 3, "max_level_label");

	if (level < (int)getSpecConfig()["damages"].size()) {
		max_level_label->setVisible(false);
		range_ni->setVisible(true);
		speed_n->setVisible(true);
		attack_n->setVisible(true);
		attack_n_label->setVisible(true);
		range_n_label->setVisible(true);
		speed_n_label->setVisible(true);
		description_label->setVisible(false);
	}
	else {
		nextlevel_layout->setVisible(false);
		nextlevel_button->setEnabled(false);
	}
	if (level >= (int)getSpecConfig()["damages"].size() - 2) {
		range_ni->setVisible(false);
		speed_n->setVisible(false);
		attack_n->setVisible(false);
		attack_n_label->setVisible(false);
		range_n_label->setVisible(false);
		speed_n_label->setVisible(false);
		description_label->setVisible(true);
	}
	MyGame* game = SceneManager::getInstance()->getGame();
	if ((int)game->getLevel()->getQuantity() < getSpecConfig()["cost"][level + 1].asInt()) {
		nextlevel_button->setEnabled(false);
	}

	auto sell = ui::Button::create("res/buttons/wood_sell.png");
	sell->setScaleX(panel->getContentSize().width * panel->getScaleX() / sell->getContentSize().width / 3);
	sell->setScaleY(panel->getContentSize().height * panel->getScaleY() / sell->getContentSize().height / 3);
	sell->setPosition(Vec2( -panel->getContentSize().width * panel->getScaleX() * 0.95 / 3, 
		current_level_layout->getPosition().y + range_i->getPosition().y - size_sprite / 2 -
		sell->getContentSize().height* sell->getScaleY() / 2));
	sell->addTouchEventListener([&](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			MyGame* game = SceneManager::getInstance()->getGame();
			game->getLevel()->increaseQuantity(getSpecConfig()["sell"][level].asInt());
			destroyCallback(sender);
			SceneManager::getInstance()->getGame()->getMenu()->hideTowerInfo();
		}
	});
	layout->addChild(sell, 1);

	auto sell_label = Label::createWithTTF("Sell",
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	sell_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	sell_label->setPosition(sell->getPosition().x, sell->getPosition().y + sell_label->getContentSize().height / 2);
	layout->addChild(sell_label,1);

	auto sell_cost = Label::createWithTTF(getSpecConfig()["cost"][level + 1].asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	sell_cost->setAlignment(cocos2d::TextHAlignment::CENTER);
	sell_cost->setPosition(sell->getPosition().x + sell_cost->getContentSize().width / 2, sell_label->getPosition().y -
		sell_label->getContentSize().height / 2 - size_sprite / 2);
	layout->addChild(sell_cost, 1, "sell_label");

	Sprite* cost_sugar = Sprite::create("res/buttons/sugar.png");
	cost_sugar->setScale(size_sprite / cost_sugar->getContentSize().width);
	cost_sugar->setPosition(sell_cost->getPosition().x - size_sprite,
		sell_cost->getPosition().y);
	layout->addChild(cost_sugar, 1);

	return layout;
}

void Tower::updateInformationLayout(ui::Layout* layout) {
	MyGame* game = SceneManager::getInstance()->getGame();
	if ((int)game->getLevel()->getQuantity() < getSpecConfig()["cost"][level + 1].asInt()) {
		((ui::Button*)layout->getChildByName("next_level_button"))->
			setEnabled(false);
		((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
			setColor(Color3B::RED);
	}
	else {
		if (level + 1 < (int)getSpecConfig()["cost"].size() && !((ui::Button*)layout->getChildByName("next_level_button"))->isEnabled()) {
			((ui::Button*)layout->getChildByName("next_level_button"))->
				setEnabled(true);
		}
		else if(level + 1 >= (int)getSpecConfig()["cost"].size() && (((ui::Button*)layout->getChildByName("next_level_button"))->isEnabled()
			|| layout->getChildByName("next_level_layout")->isVisible())){
			((ui::Button*)layout->getChildByName("next_level_button"))->
				setEnabled(false);
			layout->getChildByName("next_level_layout")->setVisible(false);
			layout->getChildByName("max_level_label")->setVisible(true);
		}
		((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
			setColor(Color3B::YELLOW);
	}
}