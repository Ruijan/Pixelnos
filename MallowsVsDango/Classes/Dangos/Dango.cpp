#include "Dango.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"
#include "../Level/Wall.h"
#include "../AppDelegate.h"
#include "../SceneManager.h"
#include "../Level/InterfaceGame.h"
#include "../Towers/Tower.h"
#include "../Towers/Attack.h"
#include <random>
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

USING_NS_CC;

Dango::Dango(std::vector<Cell*> npath, int nlevel) : path(npath), targetedCell(0), level(nlevel), 
	cAction(nullptr), cDirection(RIGHT), pDamages(0.0), 
	state(IDLE), reload_timer(0), modifier_id(0), id_damages(0), on_ground(false){
}

Dango::~Dango() { 
	for (auto& tower : targeting_towers) {
		tower->removeTarget(this);
	}
	for (auto& attack : targeting_attacks) {
		attack->removeTarget(this);
	}
	state = DEAD;
	removeAllChildren();
	//std::cerr << "Dango Destroyed ! confirmed !" << std::endl;
}

void Dango::initFromConfig() {
	auto config = getSpecConfig();
	hitPoints = config["level"][level]["hitpoints"].asDouble();
	speed = config["level"][level]["speed"].asDouble();
	attack_reloading = config["level"][level]["reload"].asDouble();
	attack_damages = config["level"][level]["damages"].asDouble();
	animation_duration = config["level"][level]["animation_attack_time"].asDouble();
	nb_frames_anim = config["level"][level]["animation_attack_size"].asInt();
	name = config["level"][level]["name"].asString();
	xp = config["level"][level]["xp"].asInt();
	double proba = config["level"][level]["holy_sugar"]["proba"].asDouble();
	double number = config["level"][level]["holy_sugar"]["number"].asDouble();
	std::string type_proba = config["level"][level]["holy_sugar"]["type"].asString();
	holy_sugar = 0;
	if (type_proba == "full") {
		double proba = rand() % 1000 / 1000;
		if (proba > proba) {
			holy_sugar = number;
		}
	}
	if (type_proba == "range") {
		for (int i(0); i < number; ++i) {
			double proba = rand() % 1000 / 1000;
			if (proba > proba) {
				++holy_sugar;
			}
		}
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	skeleton = SkeletonAnimation::createWithFile(config["skeleton"].asString(),
		config["atlas"].asString(), 0.12f * visibleSize.width / 1280);
	//skeleton->setScale(Cell::getCellWidth() / skeleton->getContentSize().width);

	skeleton->setStartListener([this](int trackIndex) {
		spTrackEntry* entry = spAnimationState_getCurrent(skeleton->getState(), trackIndex);
		const char* animationName = (entry && entry->animation) ? entry->animation->name : 0;
		//log("%d start: %s", trackIndex, animationName);
	});
	skeleton->setEndListener([this](int trackIndex) {

	});
	skeleton->setCompleteListener([this](int trackIndex, int loopCount) {
		skeletonAnimationHandle();
	});
	skeleton->setEventListener([this](int trackIndex, spEvent* event) {
		/*if (Value(event->data->name).asString() == "up") {
			on_ground = false;
		}
		else if (Value(event->data->name).asString() == "down") {
			on_ground = true;
		}*/
		//log("%d event: %s, %d, %f, %s", trackIndex, event->data->name, event->intValue, event->floatValue, event->stringValue);
	});
	skeleton->setSkin("normal_" + Value(level + 1).asString());
	updateAnimation();
	/*auto anim = skeleton->setAnimation(0,"jump_side",true);
	skeleton->setTimeScale(1 / (getSpeed() * anim->animation->duration));*/
	//skeleton->setPosition(Vec2(0, -Cell::getCellHeight() * 3 / 10));
	addChild(skeleton,2);
}

void Dango::update(float dt) {
	updateEffects(dt);

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
			if (!on_ground) {
				move(dt);
			}
			
			if (shouldAttack()) {
				state = ATTACK;
				updateAnimation();
			}
			break;
		default:
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
		setPosition(previousPos + direction * getSpeed() * Cell::getCellWidth() * dt);
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
	double x = this->getScaleX();
	spTrackEntry* anim;
	if (state == ATTACK) {
		switch (cDirection) {
		case UP:
			
			anim = skeleton->setAnimation(0, "attack_up", false);
			skeleton->setTimeScale(1.25f);
			break;
		case RIGHT:
			skeleton->setTimeScale(1.25f);
			this->setScaleX(((x > 0) - (x < 0))* x);
			anim = skeleton->setAnimation(0, "attack_side", false);
			break;
		case LEFT:
			skeleton->setTimeScale(1.25f);
			this->setScaleX(-((x > 0) - (x < 0))*x);
			anim = skeleton->setAnimation(0, "attack_side", false);
			break;
		case DOWN:
			skeleton->setTimeScale(1.25f);
			anim = skeleton->setAnimation(0, "attack_down", false);
			break;
		}
	}
	else if(state == MOVE) {
		switch (cDirection) {
		case UP:
			anim = skeleton->setAnimation(0, "jump_up", true);
			break;
		case RIGHT:
			this->setScaleX(((x > 0) - (x < 0))* x);
			anim = skeleton->setAnimation(0, "jump_side", true);
			break;
		case LEFT:
			this->setScaleX(-((x > 0) - (x < 0))*x);
			anim = skeleton->setAnimation(0, "jump_side", true);
			break;
		case DOWN:
			anim = skeleton->setAnimation(0, "jump_down", true);
			break;
		}
		skeleton->setTimeScale(anim->animation->duration * getSpeed());
	}
}

int Dango::getXP() {
	return xp;
}

int Dango::getHolySugar() {
	return holy_sugar;
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
		total_dmg += modifier.second.first;
	}
	double final_damages = damages + damages * total_dmg;
	hitPoints -= final_damages;
	if(hitPoints < 0){
		hitPoints = 0;
	}
	if (hitPoints == 0) {
		state = DYING;
		die();
	}
}

void Dango::applyProspectiveDamages(int id_damage) {
	runAction(Sequence::create(Hide::create(), DelayTime::create(0.1f), Show::create(), nullptr));
	hitPoints -= prospective_damages[id_damage];
	removePDamages(id_damage);
	if (hitPoints < 0) {
		hitPoints = 0;
	}
	if (hitPoints == 0) {
		state = DYING;
		die();
	}
	/*log("take prospective damages id : %i, size: %i", id_damage, prospective_damages.size());
	for (auto id : prospective_damages){
		log("-> damages id : %i", id);
	}*/
	
}

int Dango::takePDamages(double damages, double delay){
	double total_dmg(0);
	for (auto modifier : m_damages) {
		if (!modifier.second.second->willBeDone(delay)) {
			total_dmg += modifier.second.first;
		}
	}
	double final_damages = damages + damages * total_dmg;
	prospective_damages[id_damages] = final_damages;
	//log("add prospective damages id : %i, size: %i", id_damages, prospective_damages.size());
	++id_damages;
	return id_damages - 1;
}

void Dango::removePDamages(int id_damage) {
	prospective_damages.erase(prospective_damages.find(id_damage));
}

bool Dango::shouldAttack() {
	return (!path[targetedCell]->isFree());
}


bool Dango::isAlive(){
	return state != DEAD;
	//return hitPoints > 0;
}
bool Dango::isDying() {
	return state == DYING;
}

void Dango::die() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	skeleton->setAnimation(0, "death_normal", false);
	auto star = Sprite::create("res/levels/star.png");
	star->setScale(Cell::getCellWidth() / 5 / star->getContentSize().width);
	star->setPosition(_position);
	SceneManager::getInstance()->getGame()->getMenu()->getChildByName("reward_layout")->addChild(star,2,"star");
	auto element = SceneManager::getInstance()->getGame()->getMenu()->getChildByName("label_information")->getChildByName("sugar");
	int posx = element->getPosition().x + element->getContentSize().width * (0.5 - element->getAnchorPoint().x);
	int posy = element->getPosition().y + element->getContentSize().height * (0.5 - element->getAnchorPoint().y);
	posx += SceneManager::getInstance()->getGame()->getMenu()->getChildByName("label_information")->getPosition().x;
	posy += SceneManager::getInstance()->getGame()->getMenu()->getChildByName("label_information")->getPosition().y;
	int gain = getGain();
	auto autoRemove = CallFunc::create([star, gain, this]() {
		SceneManager::getInstance()->getGame()->getLevel()->increaseQuantity(gain);
		SceneManager::getInstance()->getGame()->getMenu()->getChildByName("reward_layout")->removeChild(star);
	});
	star->runAction(Sequence::create(Spawn::createWithTwoActions(
		EaseBackOut::create(MoveBy::create(1.f, Vec2(0, posy - _position.y))),
		MoveBy::create(1.f, Vec2(posx - _position.x, 0))), autoRemove, nullptr));
}

bool Dango::willBeAlive(){
	double total_dmg(0);
	for (auto dmg : prospective_damages) {
		total_dmg += dmg.second;
	}
	return (hitPoints - total_dmg > 0);
}

bool Dango::isDone(){
	double distance = path[targetedCell]->getPosition().distanceSquared(getPosition());
	return (targetedCell == path.size() - 1 && distance < 10);
}

Json::Value Dango::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::DANGO);
}

int Dango::getTargetedCell(){
	return targetedCell;
}

double Dango::getSpeed(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	return speed + getSpeedRedtuctionRatio() * speed;
}

void Dango::addEffect(Effect* effect) {
	effects.push_back(effect);
	if (effect->getAnimationName() != "") {
		skeleton->setSkin(effect->getAnimationName() + "_" + Value(level + 1).asString());
		effect->setVisible(false);
	}
	addChild(effect, 3);
}

int Dango::addSpeedModifier(std::pair<double, Effect*> speed_modifier) {
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
	bool change_anim(true);
	if (m_speed[id].second->getAnimationName() != "") {
		for (auto modifier : m_speed) {
			if (modifier.second.second->getAnimationName() != "" && modifier.first != id) {
				change_anim = false;
			}
		}
	}
	if (change_anim && state != DEAD) {
		skeleton->setSkin("normal_" + Value(level + 1).asString());
	}
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

int Dango::addDamagesModifier(std::pair<double, Effect*> dmg_modifier) {
	m_damages[modifier_id] = dmg_modifier;
	++modifier_id;
	return modifier_id - 1;
}

ui::Layout* Dango::getInformationLayout(InterfaceGame* interface_game) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto layout = ui::Layout::create();
	layout->setPosition(Vec2(-Cell::getCellWidth() / 2, -Cell::getCellWidth() / 2));
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
	loading_bar->setScaleX(Cell::getCellWidth() / 2 / loading_bar->getContentSize().width);
	loading_bar->setScaleY(Cell::getCellWidth() / 10 / loading_bar->getContentSize().height);
	layout->addChild(loading_bar, 2, "loading_bar");

	Sprite* loading_background = Sprite::create("res/buttons/loaderBackground.png");
	loading_background->setScaleX(Cell::getCellWidth() / 2 / loading_background->getContentSize().width);
	loading_background->setScaleY(Cell::getCellWidth() / 10 / loading_background->getContentSize().height);
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

void Dango::pauseAnimation() {
	skeleton->pause();
}

void Dango::resumeAnimation() {
	skeleton->resume();
}

int Dango::getNbCellsToPath() {
	return path.size() - targetedCell;
}

double Dango::getSpeedRedtuctionRatio() {
	double speed_ratio(0);
	for (auto modifier : m_speed) {
		if(modifier.second.first < speed_ratio){
			speed_ratio = modifier.second.first;
		}
	}
	if (speed_ratio < -0.9) {
		speed_ratio = -0.9;
	}
	return speed_ratio;
}

void Dango::updateEffects(float dt) {
	for (auto& effect : effects) {
		effect->update(dt);
		if (effect->isDone()) {
			removeChild(effect, 1);
			//delete effect;
			effect = nullptr;
		}
	}
	effects.erase(std::remove(effects.begin(), effects.end(), nullptr), effects.end());
}

void Dango::skeletonAnimationHandle() {
	std::string name = skeleton->getCurrent()->animation->name;
	if (Value(name).asString() == "death_normal") {
		endDyingAnimation();
	}
}

void Dango::endDyingAnimation() {
	state = DEAD;
}