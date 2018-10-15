#include "Tower.h"
#include "../Level/Cell.h"
#include "../Dangos/Dango.h"
#include "../Towers/Attack.h"
#include "../AppDelegate.h"
#include "../Scenes/Skills.h"
#include "../Config/Config.h"
#include <math.h>

USING_NS_CC;

Tower::Tower() :
	state(State::IDLE), fixed(false), destroy(false), target(nullptr), timer(0), timerIDLE(0), level(0),
	nb_attacks(0), direction(DOWN)
{}

Tower::~Tower() {
	if (target != nullptr) {
		target->removeTargetingTower(this);
	}
}

void Tower::initFromConfig(Config* configClass) {
	auto config = getSpecConfig();
	auto save = ((AppDelegate*)Application::getInstance())->getConfigClass()->getSaveValues();
	settings = configClass->getTowerSettings(getType());
	animation_duration = config["animation_attack_time"].asDouble();
	nb_frames_anim = config["animation_attack_size"].asInt();
	name = config["name"].asString();
	nb_max_attacks_limit = config["limit"].asInt();
	xp = save["towers"][config["name"].asString()]["exp"].asInt() + 
		(SceneManager::getInstance())->getGame()->getLevel()->getTowerXP(config["name"].asString());
	level_max = save["towers"][config["name"].asString()]["max_level"].asInt();
	limit_enabled = Skills::getSavedSkillFromID(config["limit_skill_id"].asInt())["bought"].asBool();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	skeleton = SkeletonAnimation::createWithJsonFile(config["skeleton"].asString(),
		config["atlas"].asString(),  Cell::getCellWidth() * 0.8 / config["animation_size"]["width"].asDouble());

	skeleton->setStartListener([this](spTrackEntry* entry) {
		const char* animationName = (entry && entry->animation) ? entry->animation->name : 0;
		//log("%d start: %s", trackIndex, animationName);
	});
	skeleton->setEndListener([this](spTrackEntry* entry) {});
	skeleton->setCompleteListener([this](spTrackEntry* entry) {
		std::string name = entry->animation->name;
		if (name == "blink" || name == "hello") {
			skeleton->setAnimation(0, "still", false);
			skeleton->addAnimation(0, "blink", false);
		}
		if (state == LIMIT_BURSTING) {
			handleEndEnrageAnimation();
		}
	});
	skeleton->setEventListener([this](spTrackEntry* entry, spEvent* event) {
		if (Value(event->data->name).asString() == "throw_bomb") {
			if (state != LIMIT_BURSTING) {
				this->state = State::RELOADING;
			}
			timer = 0;
			timerIDLE = 0;
			if (target != nullptr) {
				attack();
			}
		}
		//log("%d event: %s, %d, %f, %s", trackIndex, event->data->name, event->intValue, event->floatValue, event->stringValue);
	});

	skeleton->setAnimation(0, "still", false);
	skeleton->addAnimation(0, "blink", false);
	skeleton->setSkin("normal_" + Value(level + 1).asString());
	skeleton->setPosition(Vec2(0, -Cell::getCellHeight() * 3 / 10));

	addChild(skeleton);
}

void Tower::initDebug(){
	autorelease();

	loadingCircle = DrawNode::create();
	loadingCircle->setVisible(false);
	addChild(loadingCircle);

	Sprite* range = Sprite::create("res/turret/range.png");
	range->setScale(getRange() / range->getContentSize().width * 2);
	addChild(range, 0, "range");

	/*DrawNode *circle = DrawNode::create();
	circle->drawCircle(Vec2(0, 0), getRange() / getScaleX(),
		M_PI_4/2, 60, true, 1, 1, Color4F(1, 0, 0, 0.5));
	circle->drawCircle(Vec2(0, 0), getRange() / getScaleX(),
		7 * M_PI_4 / 2, 60, true, 1, 1, Color4F(1, 1, 0, 0.5));
	circle->drawCircle(Vec2(0, 0), getRange() / getScaleX(),
		9 * M_PI_4 / 2, 60, true, 1, 1, Color4F(1, 1, 1, 0.5));
	circle->drawCircle(Vec2(0, 0), getRange() / getScaleX(),
		15 * M_PI_4 / 2, 60, true, 1, 1, Color4F(0, 0, 0, 0.5));
	addChild(circle);*/


	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	auto label_state = Label::createWithTTF("IDLE", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	label_state->enableOutline(Color4B::BLACK, 2);
	label_state->setVisible(false);
	addChild(label_state,3,"label_state");
}

void Tower::initEnragePanel() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto enrage_panel = ui::Layout::create();
	enrage_panel->setPosition(Vec2(0, Cell::getCellWidth() / getScaleX()));

	auto bubble = Sprite::create("res/buttons/speech_fury_red.png");
	bubble->setScale(2.25 * Cell::getCellWidth() / getScaleX() / bubble->getContentSize().width);
	bubble->setAnchorPoint(Vec2(0.5f, 0.f));
	enrage_panel->addChild(bubble, 0, "bubble");

	ui::Button* enrage = ui::Button::create("res/buttons/button_wood_empty.png");
	enrage->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			if (((ui::Button*)getChildByName("enrage_panel")->
				getChildByName("enrage_button"))->isEnabled()) {
				startLimit();
				getChildByName("enrage_panel")->stopAllActions();
				getChildByName("enrage_panel")->setPosition(Vec2(
					0, Cell::getCellWidth() / getScaleX()));
				getChildByName("enrage_panel")->setScale(1.0f);
				((ui::Button*)getChildByName("enrage_panel")->
					getChildByName("enrage_button"))->setEnabled(false);

				auto scale_to = EaseBackOut::create(ScaleTo::create(0.2f, 0.0001f));
				auto scale_to_button = TargetedAction::create(getChildByName("enrage_panel")->
					getChildByName("enrage_button"), EaseBackOut::create(ScaleTo::create(0.1f, 0.0001f)));
				auto shake_it_right = MoveBy::create(0.03f,
					Vec2(Cell::getCellWidth() / getScaleX() / 8, 0));
				auto shake_it_left = shake_it_right->reverse();
				auto scale_by = ScaleBy::create(0.24f, 1.1f);
				auto shake_sequence = Repeat::create(Sequence::create(shake_it_right, shake_it_left,
					shake_it_left, shake_it_right, nullptr), 3);
				auto shake = Spawn::create(shake_sequence,
					Sequence::create(scale_by, scale_by->reverse(), nullptr), nullptr);
				getChildByName("enrage_panel")->runAction(Sequence::create(scale_to_button, shake, scale_to, Hide::create(), nullptr));
			}
		}
	});
	enrage->setScale(bubble->getContentSize().width * bubble->getScaleX() * 0.7 / 
		enrage->getContentSize().width);
	enrage->setPosition(Vec2(bubble->getPosition().x, bubble->getPosition().y + 
		bubble->getContentSize().height * bubble->getScale() * 0.6));
	enrage_panel->addChild(enrage, 1, "enrage_button");

	Label* enrage_label = Label::createWithTTF("ENRAGE !", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	enrage_label->setColor(Color3B::YELLOW);
	enrage_label->enableOutline(Color4B::BLACK, 1);
	enrage_label->setPosition(enrage->getPosition());
	enrage_panel->addChild(enrage_label, 1, "enrage_label");
	addChild(enrage_panel, 2, "enrage_panel");
	enrage_panel->setScale(0.0001f);
	enrage_panel->setVisible(false);
}

void Tower::incrementXP(int amount) {
	xp += amount;
	if (xp >= settings->getXPLevels()[level_max + 1]) {
		xp -= settings->getXPLevels()[level_max + 1];
		++level_max;
	}
}

void Tower::destroyCallback(Ref* sender){
	destroy = true;
	SceneManager::getInstance()->getGame()->getMenu()->removeTower();
}

void Tower::builtCallback(Ref* sender){
	fixed = true;
	skeleton->setAnimation(0, "hello", false);
}

void Tower::upgradeCallback(Ref* sender){
	if(level < (int)settings->getMaxExistingLevel()){
		++level;
		if (target != nullptr) {
			stopAttacking();
		}
		setIDLEState();
		skeleton->setSkin("normal_" + Value(level + 1).asString());
		startAnimation();
		getChildByName("range")->runAction(ScaleTo::create(0.25f,getRange() / getChildByName("range")->getContentSize().width * 2));
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
	double bestScore(1000);
	bool chosen = false;
	for (auto& cTarget : targets) {
		if (cTarget != nullptr) {
			int first = cTarget->getNbCellsToPath();
			if (first < bestScore && isPotentialTarget(cTarget)){
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

bool Tower::isPotentialTarget(Dango* cTarget) {
	double dist = cTarget->getPosition().distanceSquared(this->getPosition());
	double minDist = pow(getRange(), 2);
	return  dist <= minDist && cTarget->willBeAlive();
}

void Tower::givePDamages(double damage){
	if (target != nullptr) {
		attacked_enemies[target] = target->takePDamages(damage, animation_duration);
	}
}

double Tower::getRange(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	return settings->getRange(level) * visibleSize.width / 960.0;
}

double Tower::getNormalizedRange()
{
	return round(settings->getRange(level) / Cell::getCellWidth() * 100) / 100;
}

double Tower::getNormalizedRangeFromRange(double cRange)
{
	return round(cRange / Cell::getCellWidth() * 100) / 100;
}

double Tower::getCost(){
	return settings->getCost(level);
}

double Tower::getDamage(){
	return settings->getDamage(level);
}

double Tower::getAttackSpeed(){
	return 	settings->getAttackSpeed(level);
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
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	for (unsigned int i = 0; i <= nb_frames_anim; ++i){
		std::string frameName = name + "_" + action + "_movement_%03d.png";
		sprintf(str, frameName.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	return animFrames;
}

SkeletonAnimation* Tower::createSkeletonAnimationFromName(std::string name) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto config = getConfig()[name];
	SkeletonAnimation* animated_skeleton = SkeletonAnimation::createWithJsonFile(config["skeleton"].asString(),
		config["atlas"].asString(), 1.f);
	animated_skeleton->setCompleteListener([animated_skeleton](spTrackEntry* entry) {
		std::string name = entry->animation->name;
		if (name == "hello") {
			initAnimatedSkeleton(animated_skeleton);
		}
	});

	initAnimatedSkeleton(animated_skeleton);
	animated_skeleton->setSkin("normal_1");
	return animated_skeleton;
}

void initAnimatedSkeleton(spine::SkeletonAnimation * animated_skeleton)
{
	animated_skeleton->setAnimation(0, "still", false);
	animated_skeleton->addAnimation(0, "blink", false);
	animated_skeleton->addAnimation(0, "still", false);
	animated_skeleton->addAnimation(0, "hello", false);
}

void Tower::handleEnrageMode() {
	if (target != nullptr) {
		if (isLimitReached() && ((AppDelegate*)Application::getInstance())->getConfigClass()->getSettings()->isLimitEnabled() && limit_enabled) {
			startLimit();
		}
		else if (isLimitReached() && !getChildByName("enrage_panel")->isVisible() && limit_enabled) {
			getChildByName("enrage_panel")->setVisible(true);
			getChildByName("enrage_panel")->stopAllActions();
			getChildByName("enrage_panel")->setPosition(Vec2(0, Cell::getCellWidth() / getScaleX()));
			getChildByName("enrage_panel")->setScale(0.0001f);
			((ui::Button*)getChildByName("enrage_panel")->
				getChildByName("enrage_button"))->setEnabled(true);

			auto scale_to = EaseBackOut::create(ScaleTo::create(0.1f, 1.f));
			double scale = getChildByName("enrage_panel")->
				getChildByName("bubble")->getContentSize().width * getChildByName("enrage_panel")->
				getChildByName("bubble")->getScaleX() * 0.7 /
				getChildByName("enrage_panel")->
				getChildByName("enrage_button")->getContentSize().width;
			auto scale_to_button = TargetedAction::create(getChildByName("enrage_panel")->
				getChildByName("enrage_button"), EaseBackOut::create(ScaleTo::create(0.1f, scale)));
			auto callbackEnrage = CallFunc::create([&]() {
				auto shake_it_right = MoveBy::create(0.05f,
					Vec2(Cell::getCellWidth() / getScaleX() / 8, 0));
				auto shake_it_left = shake_it_right->reverse();
				auto scale_by = ScaleBy::create(0.3f, 1.05f);
				auto shake_sequence = Repeat::create(Sequence::create(shake_it_right, shake_it_left,
					shake_it_left, shake_it_right, nullptr), 3);
				auto shake = RepeatForever::create(Sequence::create(DelayTime::create(1.f),
					Spawn::create(shake_sequence,
						Sequence::create(scale_by, scale_by->reverse(), nullptr), nullptr),
					nullptr));
				getChildByName("enrage_panel")->runAction(shake);
			});
			getChildByName("enrage_panel")->runAction(Sequence::create(scale_to_button, scale_to, callbackEnrage, nullptr));
		}
	}
}

void Tower::update(float dt) {
	updateDisplay(dt);
	if (isLimitReached() && getChildByName("enrage_panel")->isVisible() && limit_enabled) {
		updateEnrageLayout();
	}
	if(fixed){
		updateState(dt);
	}
}

void Tower::updateState(float dt)
{
	timer += dt;
	switch (state) {
	case State::BLOCKED:
		break;
	case State::IDLE:
		updateIDLE();
		break;
	case State::AWARE:
		updateAware(dt);
		break;
	case State::ATTACKING:
		updateAttacking(dt);
		break;
	case State::RELOADING:
		updateReloading();
		break;
	case State::LIMIT_BURSTING:
		break;
	default:
		setIDLEState();
		break;
	}
}

void Tower::updateReloading()
{
	chooseTarget(((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getEnemies());
	reload();
	handleEnrageMode();
}

void Tower::updateAttacking(float dt)
{
	timerIDLE += dt;
	if (timerIDLE > 2) {
		log("ERROR WITH TIMER !!!");
	}
}

void Tower::updateAware(float dt)
{
	timerIDLE += dt;
	chooseTarget(((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getEnemies());
	if (target != nullptr) {
		handleEnrageMode();
		if (state != LIMIT_BURSTING) {
			state = State::ATTACKING;
			((Label*)getChildByName("label_state"))->setString("ATTACKING");
			givePDamages(settings->getDamage(level));
			startAnimation();
			timerIDLE = 0;
		}
	}
	if (timerIDLE > 2) {
		setIDLEState();
		startAnimation();
	}
}

void Tower::setIDLEState()
{
	state = State::IDLE;
	((Label*)getChildByName("label_state"))->setString("IDLE");
	timerIDLE = 0;
}

void Tower::updateIDLE()
{
	chooseTarget(((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getEnemies());
	if (target != nullptr) {
		state = State::AWARE;
		((Label*)getChildByName("label_state"))->setString("AWARE");
		handleEnrageMode();
	}
}

void Tower::updateEnrageLayout() {
	if (target == nullptr && ((ui::Button*)getChildByName("enrage_panel")->
		getChildByName("enrage_button"))->isEnabled()) {
		((ui::Button*)getChildByName("enrage_panel")->
			getChildByName("enrage_button"))->setEnabled(false);
	}
	else if (target != nullptr && !((ui::Button*)getChildByName("enrage_panel")->
		getChildByName("enrage_button"))->isEnabled()) {
		((ui::Button*)getChildByName("enrage_panel")->
			getChildByName("enrage_button"))->setEnabled(true);
	}
}

void Tower::updateDisplay(float dt){
	if(!fixed){
		updateOpacity(dt);
	}
	else{
		skeleton->setOpacity(255);
	}
}

void Tower::updateOpacity(float dt)
{
	float opacity = skeleton->getOpacity();
	if (state == BLINKING_UP) {
		if (opacity < 250) {
			opacity = skeleton->getOpacity() + 510 * dt < 250 ? skeleton->getOpacity() + 510 * dt : 250;
			skeleton->setOpacity(opacity);
		}
		else {
			state = BLINKING_DOWN;
		}
	}
	else if (state == BLINKING_DOWN) {
		if (opacity > 50) {
			opacity = skeleton->getOpacity() + 510 * dt >50 ? skeleton->getOpacity() - 510 * dt : 50;
			skeleton->setOpacity(opacity);
		}
		else {
			state = BLINKING_UP;
		}
	}
	else {
		state = BLINKING_UP;
	}
}

bool Tower::isLimitReached() {
	return nb_attacks >= nb_max_attacks_limit;
}

bool Tower::hasToBeDestroyed(){
	return destroy;
}

void Tower::displayRange(bool disp){
	getChildByName("range")->setVisible(disp);
	loadingCircle->setVisible(disp);
}

bool Tower::isSameType(Tower::TowerType type)
{
	return type == getType();
}

void Tower::startAnimation(float speed){
	std::string action("");
	skeleton->clearTracks();
	float game_speed = SceneManager::getInstance()->getGame()->getAcceleration();
	switch (state) {
	case IDLE:
		skeleton->setTimeScale(1.f * speed * game_speed);
		skeleton->setSkin("normal_" + Value(level + 1).asString());
		skeleton->setAnimation(0, "still", false);
		skeleton->setAnimation(1, "blink", false);
		break;
	case ATTACKING:
		skeleton->setSkin("normal_" + Value(level + 1).asString());
		skeleton->setTimeScale(1.5f * speed * game_speed);
		setOrientedAnimation();
		break;
	case LIMIT_BURSTING:
		skeleton->setSkin("enraged_" + Value(level + 1).asString());
		skeleton->setTimeScale(1.5f * speed * game_speed);
		setOrientedAnimation();
		break;
	default:
		std::cerr << "No animation found for this state.";
		std::cerr << "Steady state animation used instead." << std::endl;
		action = "steady";
		break;
	};
	
}

void Tower::setOrientedAnimation()
{
	updateDirection();
	switch (direction) {
	case DOWN:
		skeleton->setAnimation(0, "attackfront", false);
		break;
	case UP:
		skeleton->setAnimation(0, "attacktop", false);
		break;
	case RIGHT:
		skeleton->setAnimation(0, "attackright", false);
		break;
	case LEFT:
		skeleton->setAnimation(0, "attackleft", false);
		break;
	default:
		skeleton->setAnimation(0, "attackfront", false);
		break;
	}
}

const Json::Value& Tower::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TOWER);
}

void Tower::reload(){
	double attackSpeed = settings->getAttackSpeed(level);
	if (timer > attackSpeed){
		state = State::AWARE;
		((Label*)getChildByName("label_state"))->setString("AWARE");
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

Tower::State Tower::getState(){
	return state;
}

int Tower::getLevel(){
	return level;
}

void Tower::setSelected(bool select){
	selected = select;
}

void Tower::updateDirection() {
	Vec2 vec = target->getPosition() - getPosition();
	vec.normalize();
	float angle = vec.getAngle();
	if (abs(angle) <= M_PI_4) {
		direction = RIGHT;
	}
	else if (abs(angle) >= 3 * M_PI_4) {
		direction = LEFT;
	}
	else if (angle < -M_PI_4 && angle > -3 * M_PI_4) {
		direction = DOWN;
	}
	else if (angle > M_PI_4 && angle < 3 * M_PI_4) {
		direction = UP;
	}
}

void Tower::pauseAnimation() {
	skeleton->pause();
}

void Tower::resumeAnimation() {
	skeleton->resume();
}

std::string Tower::getName() {
	return name;
}

void Tower::blockTower() {
	state = BLOCKED;
	((Label*)getChildByName("label_state"))->setString("BLOCKED");
}

void Tower::freeTower() {
	setIDLEState();
}

bool Tower::isTowerBlocked() {
	return state == BLOCKED;
}

void Tower::stopAttacking() {
	if (target != nullptr) {
		if (attacked_enemies.find(target) != attacked_enemies.end()) {
			target->removePDamages(attacked_enemies[target]);
		}
		target->removeTargetingTower(this);
		target = nullptr;
	}
}

void Tower::changeSpeedAnimation(float game_speed) {
	switch (state) {
	case IDLE:
		skeleton->setTimeScale(1.f * game_speed);
		break;
	case ATTACKING:
		skeleton->setTimeScale(1.5f * game_speed);
		break;
	case LIMIT_BURSTING:
		skeleton->setTimeScale(1.5f * game_speed);
		break;
	default:
		std::cerr << "No animation found for this state.";
		std::cerr << "Steady state animation used instead." << std::endl;
		break;
	};
}

TowerSettings * Tower::getTowerSettings()
{
	return settings;
}

int Tower::getMaxLevel()
{
	return level_max;
}

int Tower::getCurrentXP(){
	return xp;
}
