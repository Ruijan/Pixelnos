#include "Dango.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"
#include "../Level/Wall.h"
#include "../AppDelegate.h"
#include "../SceneManager.h"
#include "../Level/Interface/LevelInterface.h"
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
}

void Dango::initFromConfig() {
	auto config = getSpecConfig();
	initDangoCharacteristics(config);
	initHolySugarNumber(config);
	initSkeleton(config);
	lifeBar = DangoInformationPanel::create(this);
	lifeBar->setVisible(false);
	addChild(lifeBar, 2, "lifeBar");
}

void Dango::initDangoCharacteristics(Json::Value &config)
{
	hitPoints = config["hitpoints"][level].asDouble();
	maxHitPoints = config["hitpoints"][level].asDouble();
	speed = config["speed"][level].asDouble();
	attack_reloading = config["reload"][level].asDouble();
	attack_damages = config["damages"][level].asDouble();
	name = config["names"][level].asString();
	xp = config["xp"][level].asInt();
	stay_on_ground = config["stay_on_ground"].asBool();
}

void Dango::initSkeleton(Json::Value &config)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	skeleton = SkeletonAnimation::createWithJsonFile(config["skeleton"].asString(),
		config["atlas"].asString(), 0.12f * visibleSize.width / 1280);
	skeleton->setCompleteListener([&](spTrackEntry* entry) {
		skeletonAnimationHandle();
	});
	skeleton->setEventListener([this](spTrackEntry* entry, spEvent* event) {
		if (Value(event->data->name).asString() == "up") {
			on_ground = false;
		}
		else if (Value(event->data->name).asString() == "down") {
			on_ground = true;
		}
	});
	skeleton->setSkin("normal_" + Value(level + 1).asString());
	updateAnimation();
	addChild(skeleton, 2);
}

void Dango::initHolySugarNumber(Json::Value &config)
{
	double minimumProbability = config["holy_sugar"]["prob"][level].asDouble();
	double number = config["holy_sugar"]["number"][level].asDouble();
	std::string type_proba = config["holy_sugar"]["type"][level].asString();
	std::random_device randomDevice;  //Will be used to obtain a seed for the random number engine
	std::mt19937 generator(randomDevice()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> uniformDistribution(0.0, 1.0);
	holy_sugar = 0;
	if (type_proba == "full") {
		double proba = uniformDistribution(generator);
		if (proba < minimumProbability) {
			holy_sugar = number;
		}
	}
	if (type_proba == "range") {
		for (int i(0); i < number; ++i) {
			double proba = uniformDistribution(generator);
			if (proba < minimumProbability) {
				++holy_sugar;
			}
		}
	}
}

void Dango::showLifeBar()
{
	lifeBar->setVisible(true);
}

void Dango::hideLifeBar()
{
	if (hitPoints == maxHitPoints) {
		lifeBar->setVisible(false);
	}
}

void Dango::update(float dt) {
	updateEffects(dt);
	lifeBar->update();
	switch (state) {
		case IDLE:
			updateIDLE();
			break;
		case ATTACK:
			updateAttack(dt);
			break;
		case RELOAD:
			updateReload(dt);
			break;
		case MOVE:
			updateMove(dt);
			break;
		default:
			break;
	}
}

void Dango::updateMove(float dt)
{
	if (!on_ground || !stay_on_ground) {
		move(dt);
	}

	if (shouldAttack()) {
		state = ATTACK;
		updateAnimation();
	}
}

void Dango::updateReload(float dt)
{
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
}

void Dango::updateAttack(float dt)
{
	if (shouldAttack()) {
		attack(dt);
		state = RELOAD;
	}
	else {
		state = MOVE;
		updateAnimation();
	}
}

void Dango::updateIDLE()
{
	if (shouldAttack()) {
		state = ATTACK;
		updateAnimation();
	}
	else {
		state = MOVE;
		updateAnimation();
	}
}

void Dango::move(float dt){
	
	double distanceToTargetCell = path[targetedCell]->getPosition().distanceSquared(getPosition());
	double distanceBetweenTwoCells(0);
	double distanceToPreviousCell(0);
	if (targetedCell >= 1){
		distanceBetweenTwoCells = path[targetedCell]->getPosition().distanceSquared(path[targetedCell - 1]->getPosition());
		distanceToPreviousCell = path[targetedCell-1]->getPosition().distanceSquared(getPosition());
	}
	if (targetedCell == 0 || distanceToPreviousCell >= distanceBetweenTwoCells){
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
	float gameSpeed = SceneManager::getInstance()->getGame()->getAcceleration();

	if (state == ATTACK || state == MOVE) {
		updateAnimationDirection(state, cDirection, gameSpeed);
	}
}

std::string getStringFromState(Dango::STATE state) {
	switch (state) {
	case Dango::STATE::ATTACK:
		return "attack";
	case Dango::STATE::ATTACK_SPE:
		return "attack_spe";
	case Dango::STATE::MOVE:
		return "jump";
	case Dango::STATE::RELOAD:
		return "reload";
	case Dango::STATE::DYING:
		return "dying";
	case Dango::STATE::IDLE:
		return "idle";
	case Dango::STATE::DEAD:
		return "dead";
	}
}

std::string getStringFromDirection(Dango::DIRECTION direction) {
	switch (direction) {
	case Dango::DIRECTION::UP:
		return "up";
	case Dango::DIRECTION::LEFT:
		return "side";
	case Dango::DIRECTION::RIGHT:
		return "side";
	case Dango::DIRECTION::DOWN:
		return "down";
	}
}

void Dango::updateAnimationDirection(Dango::STATE state, Dango::DIRECTION direction, double gameSpeed) {
	spTrackEntry* anim;
	double x = this->getScaleX();
	if (direction == Dango::DIRECTION::RIGHT) {
		this->setScaleX(((x > 0) - (x < 0))* x);
	}
	else if (direction == Dango::DIRECTION::LEFT) {
		this->setScaleX(-((x > 0) - (x < 0))*x);
	}
	anim = skeleton->setAnimation(0, getStringFromState(state) + "_" + getStringFromDirection(direction), true);
	skeleton->setTimeScale(anim->animation->duration * (1 - getSpeedReductionRatio()) * gameSpeed);
}

int Dango::getXP() {
	return xp;
}

int Dango::getHolySugar() {
	return holy_sugar;
}

unsigned int Dango::getLevel()
{
	return level;
}

double Dango::getHitPoints(){
	return hitPoints;
}

double Dango::getGain(){
	return getSpecConfig()["gain"][level].asDouble();
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
	showLifeBar();
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
	showLifeBar();
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
	return speed + getSpeedReductionRatio() * speed;
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

void Dango::pauseAnimation() {
	skeleton->pause();
}

void Dango::resumeAnimation() {
	skeleton->resume();
}

int Dango::getNbCellsToPath() {
	return path.size() - targetedCell;
}

double Dango::getSpeedReductionRatio() {
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

void Dango::changeSpeedAnimation(float game_speed) {
	if (state == ATTACK) {
		skeleton->setTimeScale(1.25f * game_speed);
	}
	else if (state == MOVE) {
		skeleton->setTimeScale(skeleton->getCurrent()->animation->duration * getSpeed() * game_speed);
	}
}