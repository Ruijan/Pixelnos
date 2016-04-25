#include "Dango.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"
#include "../Level/Wall.h"
#include "../AppDelegate.h"
#include "../InterfaceGame.h"
#include "../Towers/Tower.h"
#include "../Towers/Attack.h"

USING_NS_CC;

Dango::Dango(std::vector<Cell*> npath, double nspeed, double hp, int nlevel, 
	double damages, double a_reload) : path(npath), targetedCell(0), speed(nspeed),
	hitPoints(hp), level(nlevel), cAction(nullptr), cDirection(RIGHT), pDamages(0.0), 
	state(IDLE), reload_timer(0), attack_damages(damages), attack_reloading(a_reload),
	modifier_id(0){
}

Dango::~Dango() { 
	removeAllChildren();
	for (auto& tower : targeting_towers) {
		tower->removeTarget(this);
	}
	for (auto& attack : targeting_attacks) {
		attack->removeTarget(this);
	}
	//std::cerr << "Dango Destroyed ! confirmed !" << std::endl;
}

void Dango::update(float dt) {
	for (auto& effect : effects){
		effect->update(dt);
		if (effect->isDone()) {
			removeChild(effect, 1);
			//delete effect;
			effect = nullptr;
		}
	}
	effects.erase(std::remove(effects.begin(), effects.end(), nullptr), effects.end());

	switch (state) {
		case IDLE:
			if (shouldAttack()) {
				state = ATTACK;
				updateAnimation();
			}
			else {
				state = MOVE;
				updateAnimation();
			}
			break;
		case ATTACK:
			if (shouldAttack()) {
				attack(dt);
				state = RELOAD;
			}
			else {
				state = MOVE;
				updateAnimation();
			}
			break;
		case RELOAD:
			reload_timer += dt;
			if (reload_timer > attack_reloading) {
				reload_timer = 0;
				if (shouldAttack()) {
					state = ATTACK;
					updateAnimation();
				}
				else {
					state = MOVE;
					updateAnimation();
				}
			}
			break;
		case MOVE:
			move(dt);
			if (shouldAttack()) {
				state = ATTACK;
				updateAnimation();
			}
			break;
	}
}

void Dango::move(float dt){
	
	double distance = path[targetedCell]->getPosition().distanceSquared(getPosition());
	double distance2(0);
	double distance3(0);
	if (targetedCell >= 1){
		distance2 = path[targetedCell]->getPosition().distanceSquared(path[targetedCell - 1]->getPosition());
		distance3 = path[targetedCell-1]->getPosition().distanceSquared(getPosition());
	}
	if (targetedCell == 0 || distance3 >= distance2){
		if (targetedCell + 1 < path.size()){
			++targetedCell;
			Vec2 direction = path[targetedCell]->getPosition() - path[targetedCell - 1]->getPosition();
			direction.normalize();
			updateDirection(direction);
		}
		else{
			setPosition(path[targetedCell]->getPosition());
		}
	}
	else{
		Vec2 previousPos = getPosition();
		Vec2 direction = path[targetedCell]->getPosition() - path[targetedCell - 1]->getPosition();
		direction.normalize();
		updateDirection(direction);
		setPosition(previousPos + direction * getSpeed() * dt);
	}
	
}

void Dango::updateDirection(Vec2 direction) {
	DIRECTION nDirection = cDirection;
	if (direction.x > direction.y && direction.x > 0) {
		nDirection = RIGHT;
	}
	else if (abs(direction.x) > abs(direction.y) && direction.x < 0) {
		nDirection = LEFT;
	}
	else if (abs(direction.x) < abs(direction.y) && direction.y < 0) {
		nDirection = DOWN;
	}
	else if (abs(direction.x) < abs(direction.y) && direction.y > 0) {
		nDirection = UP;
	}
	if (nDirection != cDirection) {
		cDirection = nDirection;
		updateAnimation();
	}
}

void Dango::attack(float dt) {
	((Wall*)(path[targetedCell]->getObject()))->takeDamages(attack_damages);
}

void Dango::updateAnimation(){
	if(cAction != nullptr){
		stopAction(cAction);
	}
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	std::string preString(getSpecConfig()["level"][level]["name"].asString());
	double x = this->getScaleX();
	if (state == ATTACK) {
		switch (cDirection) {
		case UP:
			preString += "_attack_";
			break;
		case RIGHT:
			this->setScaleX(((x > 0) - (x < 0))* x);
			preString += "_attack_";
			break;
		case LEFT:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString += "_attack_";
			break;
		case DOWN:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString += "_attack_";
			break;
		}
	}
	else {
		switch (cDirection) {
		case UP:
			preString += "_ju_";
			break;
		case RIGHT:
			this->setScaleX(((x > 0) - (x < 0))* x);
			preString += "_j_";
			break;
		case LEFT:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString += "_j_";
			break;
		case DOWN:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			preString += "_jd_";
			break;
		}
	}
	cocos2d::Vector<SpriteFrame*> animFrames;
	char str[100] = { 0 };
	for (int i = 0; i < 24; ++i){
		sprintf(str, "%s%03d.png", preString.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	double delay = getSpecConfig()["level"][level]["animation_duration"].asDouble() / getSpecConfig()["level"][level]["nb_images_animation"].asDouble();
	Animation* animation = Animation::createWithSpriteFrames(animFrames, delay);
	// Cell::getCellWidth() / getSpeed() / 24 * getSpecConfig()["cellperanim"].asFloat()
	runAnimation(animation);
}

double Dango::getHitPoints(){
	return hitPoints;
}

double Dango::getGain(){
	return getSpecConfig()["level"][level]["gain"].asDouble();
}

void Dango::takeDamages(double damages){
	runAction(Sequence::create(Hide::create(), DelayTime::create(0.1f), Show::create(), nullptr));
	double total_dmg(0);
	for (auto modifier : m_damages) {
		total_dmg += modifier.second;
	}
	double final_damages = damages + damages * total_dmg;
	hitPoints -= final_damages;
	pDamages -= damages;
	if(hitPoints < 0){
		hitPoints = 0;
	}
	if (pDamages < 0) {
		log("We have a problem, the prospective damages should not be below zero");
	}
}

void Dango::takePDamages(double damages){
	pDamages += damages;
}

void Dango::removePDamages(double damages) {
	pDamages -= damages;
	if (pDamages < 0) {
		log("prospectives damages negative");
		pDamages = 0;
	}
}

bool Dango::shouldAttack() {
	return (!path[targetedCell]->isFree());
}


bool Dango::isAlive(){
	return hitPoints > 0;
}

bool Dango::willBeAlive(){
	return (hitPoints - pDamages > 0);
}

bool Dango::isDone(){
	double distance = path[targetedCell]->getPosition().distanceSquared(getPosition());
	return (targetedCell == path.size() - 1 && distance < 10);
}

Json::Value Dango::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"];
}

int Dango::getTargetedCell(){
	return targetedCell;
}

double Dango::getSpeed(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	double total_speed(0);
	for (auto modifier : m_speed) {
		total_speed += modifier.second;
	}
	return speed * visibleSize.width / 960.0 + total_speed * speed * visibleSize.width / 960.0;
}

void Dango::runAnimation(Animation* anim) {
	if (state == MOVE) {
		cAction = runAction(RepeatForever::create(Animate::create(anim)));
	}
	else if (state == ATTACK) {
		cAction = runAction(Animate::create(anim));
	}
}

void Dango::addEffect(Effect* effect) {
	effects.push_back(effect);
	addChild(effect,3);
}

int Dango::addSpeedModifier(double speed_modifier) {
	m_speed[modifier_id] = speed_modifier;	
	++modifier_id;
	return modifier_id - 1;
}

void Dango::addTargetingTower(Tower* tower) {
	targeting_towers.push_back(tower);
}

void Dango::addTargetingAttack(Attack* attack) {
	targeting_attacks.push_back(attack);
}

void Dango::removeSpeedModifier(int id) {
	m_speed.erase(m_speed.find(id));
	//m_speed.erase(std::remove(m_speed.begin(), m_speed.end(), id), m_speed.end());
}

void Dango::removeDamageModifier(int id) {
	m_damages.erase(m_damages.find(id));
	//m_damages.erase(std::remove(m_damages.begin(), m_damages.end(), id), m_damages.end());
}

void Dango::removeTargetingTower(Tower* tower) {
	targeting_towers.erase(std::remove(targeting_towers.begin(), targeting_towers.end(), tower), targeting_towers.end());
}

void Dango::removeTargetingAttack(Attack* tower) {
	targeting_attacks.erase(std::remove(targeting_attacks.begin(), targeting_attacks.end(), tower), targeting_attacks.end());
}

int Dango::addDamagesModifier(double dmg_modifier) {
	m_damages[modifier_id] = dmg_modifier;
	++modifier_id;
	return modifier_id - 1;
}

ui::Layout* Dango::getInformationLayout(InterfaceGame* interface_game) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto layout = ui::Layout::create();
	/*
	auto panel = ui::Button::create("res/buttons/wood_information_panel.png");
	panel->setScaleX(visibleSize.width / 5 / panel->getContentSize().width);
	panel->setScaleY(visibleSize.width / 8 / panel->getContentSize().width);
	panel->setZoomScale(0);
	layout->addChild(panel, 0, "panel");

	Label* dango_label = Label::createWithTTF(getSpecConfig()["name"].asString(), "fonts/LICABOLD.ttf", 35 * visibleSize.width / 1280);
	dango_label->setColor(Color3B::BLACK);
	dango_label->setPosition(Vec2(0,
		panel->getContentSize().height * panel->getScaleY() * 0.90/2));
	dango_label->setAnchorPoint(Vec2(0.5, 1));
	layout->addChild(dango_label, 1, "level_label");

	Sprite* speed_mv = Sprite::create("res/buttons/speed.png");
	Sprite* attack = Sprite::create("res/buttons/attack.png");
	Sprite* speed_attack = Sprite::create("res/buttons/speed.png");
	Sprite* life = Sprite::create("res/buttons/life.png");
	layout->addChild(speed_mv, 1, "speed_mv");
	layout->addChild(attack, 1, "attack");
	layout->addChild(life, 1, "range");

	double size_sprite = panel->getContentSize().width * panel->getScaleX() / 12;
	

	speed_mv->setScale(size_sprite / speed_mv->getContentSize().width);
	attack->setScale(size_sprite / attack->getContentSize().width);
	speed_attack->setScale(size_sprite / speed_attack->getContentSize().width);
	life->setScale(size_sprite / life->getContentSize().width);

	std::string s = Value(getSpeed() / Cell::getCellWidth()).asString();
	s.resize(4);
	Label* speed_mv_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 20 * visibleSize.width / 1280);
	speed_mv_label->setColor(Color3B::BLACK);
	speed_mv_label->setAnchorPoint(Vec2(0.f, 0.5f));
	layout->addChild(speed_mv_label, 1, "speed_mv_label");

	s = Value(attack_damages).asString();
	s.resize(4);
	Label* attack_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 20 * visibleSize.width / 1280);
	attack_label->setColor(Color3B::BLACK);
	attack_label->setAnchorPoint(Vec2(0.f, 0.5f));
	layout->addChild(attack_label, 1, "attack_label");

	Label* life_label = Label::createWithTTF(Value((int)hitPoints).asString() + "/" + 
		getSpecConfig()["level"][level]["hitpoints"].asString(),
		"fonts/LICABOLD.ttf", 20 * visibleSize.width / 1280);
	life_label->setColor(Color3B::BLACK);
	life_label->setAnchorPoint(Vec2(0.f, 0.5f));
	layout->addChild(life_label, 1, "life_label");

	double total_width = 3 * size_sprite + speed_mv_label->getContentSize().width +
		attack_label->getContentSize().width + life_label->getContentSize().width +
		3 / 4 * size_sprite;

	speed_mv->setPosition(-total_width / 2,
		-panel->getContentSize().height * panel->getScaleY() / 4);
	speed_mv_label->setPosition(Vec2(speed_mv->getPosition().x + size_sprite * 3 / 4,
		speed_mv->getPosition().y));
	attack->setPosition(speed_mv_label->getPosition().x + speed_mv_label->getContentSize().width +
		attack->getContentSize().width * attack->getScale() * 3 / 4,
		-panel->getContentSize().height * panel->getScaleY() / 4);
	attack_label->setPosition(Vec2(attack->getPosition().x + size_sprite * 3 / 4,
		attack->getPosition().y));
	life->setPosition(attack_label->getPosition().x + attack_label->getContentSize().width +
		life->getContentSize().width * life->getScale() * 3 / 4,
		-panel->getContentSize().height * panel->getScaleY() / 4);
	life_label->setPosition(Vec2(life->getPosition().x + size_sprite * 3 / 4,
		life->getPosition().y));
	*/

	ui::LoadingBar* loading_bar = ui::LoadingBar::create("res/buttons/sliderProgress.png");
	loading_bar->setPercent((double)hitPoints / getSpecConfig()["level"][level]["hitpoints"].asDouble() * 100);
	loading_bar->setScale(visibleSize.width / 15 / loading_bar->getContentSize().width);
	layout->addChild(loading_bar, 2, "loading_bar");

	Sprite* loading_background = Sprite::create("res/buttons/loaderBackground.png");
	loading_background->setScale(visibleSize.width / 15 / loading_background->getContentSize().width);
	layout->addChild(loading_background, 1, "loading_background");

	double position_x = getPosition().x;
	double position_y = getPosition().y + getContentSize().height * getScaleY();
	if (position_x - loading_background->getContentSize().width * loading_background->getScaleX() / 2 < 0) {
		position_x += abs(position_x - loading_background->getContentSize().width * loading_background->getScaleX() / 2);
	}
	else if (position_x + loading_background->getContentSize().width * loading_background->getScaleX() / 2 > visibleSize.width * 3 / 4) {
		position_x -= abs(position_x + loading_background->getContentSize().width * loading_background->getScaleX() / 2 - visibleSize.width * 3 / 4);
	}
	if (position_y + loading_background->getContentSize().height * loading_background->getScaleY() / 2 > visibleSize.height) {
		position_y = getPosition().y - loading_background->getContentSize().height * loading_background->getScaleY() / 2 -
			getContentSize().height*getScaleY() / 2;
	}
	layout->setPosition(Vec2(position_x, position_y));

	return layout;
}

void Dango::updateInformationLayout(cocos2d::ui::Layout* layout) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto loading_background = layout->getChildByName("loading_background");
	auto loading_bar = (ui::LoadingBar*)layout->getChildByName("loading_bar");
	loading_bar->setPercent((double)hitPoints / getSpecConfig()["level"][level]["hitpoints"].asDouble() * 100);

	double position_x = getPosition().x;
	double position_y = getPosition().y + getContentSize().height * getScaleY();
	if (position_x - loading_background->getContentSize().width * loading_background->getScaleX() / 2 < 0) {
		position_x += abs(position_x - loading_background->getContentSize().width * loading_background->getScaleX() / 2);
	}
	else if (position_x + loading_background->getContentSize().width * loading_background->getScaleX() / 2 > visibleSize.width * 3 / 4) {
		position_x -= abs(position_x + loading_background->getContentSize().width * loading_background->getScaleX() / 2 - visibleSize.width * 3 / 4);
	}
	if (position_y + loading_background->getContentSize().height * loading_background->getScaleY() / 2 > visibleSize.height) {
		position_y = getPosition().y - loading_background->getContentSize().height * loading_background->getScaleY() / 2 -
			getContentSize().height*getScaleY() / 2;
	}
	layout->setPosition(Vec2(position_x, position_y));
}