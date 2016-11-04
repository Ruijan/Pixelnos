#include "Tower.h"
#include "../Level/Cell.h"
#include "../Dangos/Dango.h"
#include "../Towers/Attack.h"
#include "../AppDelegate.h"
#include "../Level/InterfaceGame.h"
#include <math.h>

USING_NS_CC;

Tower::Tower() :
	state(State::IDLE), fixed(false), destroy(false), target(nullptr), timer(0), timerIDLE(0), level(0),
	nb_attacks(0), spritesheet(false), direction(DOWN), blocked(false)
{}

Tower::~Tower() {
	if (target != nullptr) {
		target->removeTargetingTower(this);
	}
}

void Tower::initFromConfig() {
	auto config = getSpecConfig();
	auto save = ((AppDelegate*)Application::getInstance())->getConfigClass()->getSaveValues();
	for (unsigned int i(0); i < config["cost"].size(); ++i) {
		costs.push_back(config["cost"][i].asInt());
		sells.push_back(config["sell"][i].asInt());
		damages.push_back(config["damages"][i].asDouble());
		ranges.push_back(config["range"][i].asDouble());
		attack_speeds.push_back(config["attack_speed"][i].asDouble());
		xp_levels.push_back(config["xp_level"][i].asDouble());
	}
	
	cost = costs[0];
	attack_speed = attack_speeds[0];
	damage = damages[0];
	range = ranges[0];
	animation_duration = config["animation_attack_time"].asDouble();
	nb_frames_anim = config["animation_attack_size"].asInt();
	name = config["name"].asString();
	nb_max_attacks_limit = config["limit"].asInt();
	xp = save["towers"][config["name"].asString()]["exp"].asInt() + 
		(SceneManager::getInstance())->getGame()->getLevel()->getTowerXP(config["name"].asString());
	level_max = save["towers"][config["name"].asString()]["max_level"].asInt();
	limit_enabled = ((AppDelegate*)Application::getInstance())->getConfigClass()->
		findSkill(config["limit_skill_id"].asInt())["bought"].asBool();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	skeleton = SkeletonAnimation::createWithFile(config["skeleton"].asString(),
		config["atlas"].asString(),  Cell::getCellWidth() * 0.8 / config["animation_size"]["width"].asDouble());

	skeleton->setStartListener([this](int trackIndex) {
		spTrackEntry* entry = spAnimationState_getCurrent(skeleton->getState(), trackIndex);
		const char* animationName = (entry && entry->animation) ? entry->animation->name : 0;
		//log("%d start: %s", trackIndex, animationName);
	});
	skeleton->setEndListener([this](int trackIndex) {

	});
	skeleton->setCompleteListener([this](int trackIndex, int loopCount) {
		std::string name = skeleton->getCurrent()->animation->name;
		if (Value(skeleton->getCurrent()->animation->name).asString() == "blink" ||
			Value(skeleton->getCurrent()->animation->name).asString() == "hello") {
			skeleton->clearTracks();
			skeleton->setAnimation(0, "still", false);
			skeleton->addAnimation(0, "blink", false);
		}
		if (state == LIMIT_BURSTING) {
			handleEndEnrageAnimation();
		}
	});
	skeleton->setEventListener([this](int trackIndex, spEvent* event) {
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

	/*auto clipper = ClippingNode::create();
	clipper->setContentSize(Size(Cell::getCellWidth(), Cell::getCellWidth()));
	clipper->setAnchorPoint(Vec2(0.5, 0.5));
	clipper->setAlphaThreshold(0.05f);
	addChild(clipper);

	auto stencil = Sprite::create("res/turret/splash.png");
	stencil->setPosition(clipper->getContentSize().width, clipper->getContentSize().height);
	clipper->setStencil(skeleton);
	clipper->addChild(stencil);*/
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
	

	/*auto label_state = Label::createWithTTF("IDLE", "fonts/LICABOLD.ttf", 30.f * visibleSize.width / 1280);
	label_state->enableOutline(Color4B::BLACK, 2);
	addChild(label_state,3,"label_state");*/
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
	if (xp >= xp_levels[level_max + 1]) {
		xp -= xp_levels[level_max + 1];
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
	if(level < (int)costs.size()){
		++level;
		range = ranges[level];
		damage = damages[level];
		attack_speed = attack_speeds[level];
		if (target != nullptr) {
			stopAttacking();
		}
		state = IDLE;
		timerIDLE = 0;
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

	for(auto& cTarget : targets){
		if(cTarget != nullptr){
			int first = cTarget->getNbCellsToPath();
			double dist = cTarget->getPosition().distanceSquared(this->getPosition());
			double minDist = pow(getRange(), 2);
			if (first < bestScore && dist <= minDist && cTarget->willBeAlive()){
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
void Tower::givePDamages(double damage){
	if (target != nullptr) {
		attacked_enemies[target] = target->takePDamages(damage, animation_duration);
	}
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
	return attack_speed;
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
	
	int animation_size = Tower::getConfig()[name]["animation_"+ action +"_size"].asInt();
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cocos2d::Vector<SpriteFrame*> animFrames;

	char str[100] = { 0 };
	for (int i(0); i <= animation_size; ++i)
	{
		std::string frameName =  name+"_"+action+"_movement_%03d.png";
		sprintf(str, frameName.c_str(), i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	return animFrames;
}

SkeletonAnimation* Tower::getSkeletonAnimationFromName(std::string name) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto config = getConfig()[name];
	SkeletonAnimation* animated_skeleton = SkeletonAnimation::createWithFile(config["skeleton"].asString(),
		config["atlas"].asString(), 1.f);
	animated_skeleton->setCompleteListener([animated_skeleton](int trackIndex, int loopCount) {
		std::string name = animated_skeleton->getCurrent()->animation->name;
		if (Value(animated_skeleton->getCurrent()->animation->name).asString() == "hello") {
			animated_skeleton->clearTracks();
			animated_skeleton->setAnimation(0, "still", false);
			animated_skeleton->addAnimation(0, "blink", false);
			animated_skeleton->addAnimation(0, "still", false);
			animated_skeleton->addAnimation(0, "hello", false);
			//animated_skeleton->setSkin()
		}
	});

	animated_skeleton->setAnimation(0, "still", false);
	animated_skeleton->addAnimation(0, "blink", false);
	animated_skeleton->addAnimation(0, "still", false);
	animated_skeleton->addAnimation(0, "hello", false);
	animated_skeleton->setSkin("normal_1");
	return animated_skeleton;
}

void Tower::handleEnrageMode() {
	if (target != nullptr) {
		if (isLimitReached() && ((AppDelegate*)Application::getInstance())->getConfigClass()->isLimitEnabled() && limit_enabled) {
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
	// display update
	
	updateDisplay(dt);
	if (isLimitReached() && getChildByName("enrage_panel")->isVisible() && limit_enabled) {
		updateEnrageLayout();
	}
	
	if(fixed){
		timer += dt;
		switch(state){
			case State::BLOCKED:
				if (!blocked) {
					state = IDLE;
				}
				break;
			case State::IDLE:
				if (blocked) {
					state = BLOCKED;
				}
				else {
					chooseTarget(((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getEnemies());
					if (target != nullptr) {
						state = State::AWARE;
						//((Label*)getChildByName("label_state"))->setString("AWARE");
						handleEnrageMode();
					}
				}
				break;
			case State::AWARE:
				if (blocked) {
					state = BLOCKED;
				}
				else {
					timerIDLE += dt;
					chooseTarget(((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getEnemies());
					if (target != nullptr) {
						handleEnrageMode();
						/*cocos2d::Vector<SpriteFrame*> animFrames = getAnimation(state);
						double delay = animation_duration / nb_frames_anim;
						auto callbackAttack = CallFunc::create([&]() {
							state = State::RELOADING;
							//((Label*)getChildByName("label_state"))->setString("RELOADING");
							/*std::string frameName = name + "_attack_movement_000.png";
							SpriteFrameCache* cache = SpriteFrameCache::getInstance();
							setSpriteFrame(cache->getSpriteFrameByName(frameName.c_str()));
							timer = 0;
							timerIDLE = 0;
							if (target != nullptr) {
								attack();
							}
						});*/
						//runAction(Sequence::create(Animate::create(Animation::createWithSpriteFrames(animFrames, delay)), callbackAttack, nullptr));
						//((Label*)getChildByName("label_state"))->setString("ATTACKING");
						if (state != LIMIT_BURSTING) {
							state = State::ATTACKING;
							givePDamages(damage);
							startAnimation();
							timerIDLE = 0;
						}
					}
				}
				if(timerIDLE > 2){
					state = State::IDLE;
					//((Label*)getChildByName("label_state"))->setString("IDLE");
					timerIDLE = 0;
					startAnimation();
				}
				break;
			case State::ATTACKING:
				timerIDLE += dt;
				if(timerIDLE > 2){
					log("ERROR WITH TIMER !!!");
				}
				break;
			case State::RELOADING:
				chooseTarget(((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getEnemies());
				reload();
				handleEnrageMode();
				break;
			case State::LIMIT_BURSTING:
				break;
			default:
				state = IDLE;
				//((Label*)getChildByName("label_state"))->setString("IDLE");
				break;
		}
	}
	else{
		
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
		float opacity = skeleton->getOpacity();
		if (state == BLINKING_UP){
			if(opacity < 250){
				opacity = skeleton->getOpacity() + 510 * dt < 250 ? skeleton->getOpacity() + 510 * dt : 250;
				skeleton->setOpacity(opacity);
			}
			else{
				state = BLINKING_DOWN;
			}
		}
		else if(state == BLINKING_DOWN){
			if(opacity > 50){
				opacity = skeleton->getOpacity() + 510 * dt >50 ? skeleton->getOpacity() - 510 * dt : 50;
				skeleton->setOpacity(opacity);
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
		skeleton->setOpacity(255);
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

void Tower::startAnimation(float speed){
	/*
	cocos2d::Vector<SpriteFrame*> animFrames = getAnimation(state);
	double delay = animation_duration / nb_frames_anim / speed;
	Animation* current_animation = Animation::createWithSpriteFrames(animFrames, delay);
	runAction(Animate::create(current_animation));
	*/
	std::string action("");
	skeleton->clearTracks();

	switch (state) {
	case IDLE:
		skeleton->setTimeScale(1.f * speed);
		skeleton->setSkin("normal_" + Value(level + 1).asString());
		skeleton->setAnimation(0, "still", false);
		skeleton->setAnimation(1, "blink", false);
		break;
	case ATTACKING:
		skeleton->setTimeScale(1.5f * speed);
		skeleton->setSkin("normal_" + Value(level + 1).asString());
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
		break;
	case LIMIT_BURSTING:
		skeleton->setTimeScale(1.5f * speed);
		skeleton->setSkin("enraged_" + Value(level + 1).asString());
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
		break;
	default:
		std::cerr << "No animation found for this state.";
		std::cerr << "Steady state animation used instead." << std::endl;
		action = "steady";
		break;
	};
	
}

const Json::Value& Tower::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()["towers"];
}

void Tower::reload(){
	
	if (timer > attack_speed){
		state = State::AWARE;
		//((Label*)getChildByName("label_state"))->setString("AWARE");
	}
	else{
		double iniAngle = M_PI_2;
		loadingCircle->clear();
		loadingCircle->drawSolidCircle(Vec2(0, 0), 0.11 * 50 / getScaleX(),
			0, 60, 1, 1, Color4F(0, 0, 0, 1));
		loadingCircle->drawSolidCircle(Vec2(0, 0), 0.1 * timer / attack_speed * 50 / getScaleX(),
			0, 60, 1, 1, Color4F(1 - timer / attack_speed, timer / attack_speed, 0, 1));		
	}
}

Tower::TowerType Tower::getTowerTypeFromString(std::string type){
	if (!strcmp(type.c_str(), "bomber")){
		return Tower::TowerType::BOMBER;
	}
	else if (!strcmp(type.c_str(), "cutter")){
		return Tower::TowerType::CUTTER;
	}
	else if (!strcmp(type.c_str(), "saucer")) {
		return Tower::TowerType::SAUCER;
	}
	else{
		return Tower::TowerType::BOMBER;
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
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();
	const auto config = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()["buttons"];
	const auto spec_config = getSpecConfig();
	auto layout = ui::Layout::create();

	auto panel = ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setScaleX(visibleSize.width / 4 / panel->getContentSize().width);
	panel->setScaleY(visibleSize.width * 0.25 / panel->getContentSize().width);
	panel->setZoomScale(0);
	layout->addChild(panel, 0);

	auto current_level_layout = ui::Layout::create();
	current_level_layout->setContentSize(Size(panel->getContentSize().width * panel->getScaleX() / 3,
		panel->getContentSize().height * panel->getScaleY() * 3 / 4));
	current_level_layout->setPosition(Vec2(-panel->getContentSize().width * panel->getScaleX() * 0.3,
		current_level_layout->getContentSize().height / 2));

	Label* level_label = Label::createWithTTF(config["level"][language].asString() + " " + Value(level + 1).asString(), 
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	level_label->setColor(Color3B::BLACK);
	level_label->setPosition(Vec2(0,
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
	
	attack->setPosition(-size_sprite / 2, level_label->getPosition().y -
		level_label->getContentSize().height / 2
		-attack->getContentSize().height*attack->getScaleY() / 2);
	speed->setPosition(-size_sprite / 2, attack->getPosition().y - attack->getContentSize().height*
		attack->getScaleY() / 2 - speed->getContentSize().height * speed->getScaleY() * 3 / 4);
	range_i->setPosition(-size_sprite / 2, speed->getPosition().y - speed->getContentSize().height*
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

	s = Value(attack_speed).asString();
	s.resize(4);
	Label* speed_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	speed_label->setColor(Color3B::BLACK);
	speed_label->setPosition(Vec2(speed->getPosition().x + size_sprite,
		speed->getPosition().y));
	speed_label->setAnchorPoint(Vec2(0.f, 0.5f));
	current_level_layout->addChild(speed_label, 1, "speed_label");
	
	double position_x = getPosition().x;
	double position_y = getPosition().y + panel->getContentSize().height * panel->getScaleY() / 2 +
		Cell::getCellHeight() / 2;
	if (position_x - panel->getContentSize().width * panel->getScaleX() / 2 < 0) {
		position_x += abs(position_x - panel->getContentSize().width * panel->getScaleX() / 2);
	}
	else if (position_x + panel->getContentSize().width * panel->getScaleX() / 2 > visibleSize.width * 3 / 4) {
		position_x -= abs(position_x + panel->getContentSize().width * panel->getScaleX() / 2 - visibleSize.width * 3 / 4);
	}
	if (position_y + panel->getContentSize().height * panel->getScaleY() / 2 > visibleSize.height) {
		position_y = getPosition().y - panel->getContentSize().height * panel->getScaleY() / 2 -
			Cell::getCellHeight() / 2;
	}
	layout->setPosition(Vec2(position_x, position_y));
	layout->addChild(current_level_layout, 2, "current_level_layout");

	auto nextlevel_button = ui::Button::create("res/buttons/upgrade.png");
	nextlevel_button->setScaleX(panel->getContentSize().width * panel->getScaleX() * .95f / 
		nextlevel_button->getContentSize().width * 0.55f);
	nextlevel_button->setScaleY(panel->getContentSize().height * panel->getScaleY() /
		nextlevel_button->getContentSize().height * 0.85);
	nextlevel_button->setPosition(Vec2(panel->getContentSize().width * panel->getScaleX() / 6,
		0));
	layout->addChild(nextlevel_button, 1, "next_level_button");
	nextlevel_button->addTouchEventListener([&, layout](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			MyGame* game = SceneManager::getInstance()->getGame();
			std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();

			auto cost_size = costs.size();
			if ((int)game->getLevel()->getQuantity() >= costs[level + 1] &&
				level < (int)cost_size) 
			{
				Json::Value action;
				action["tower_name"] = name;
				action["time"] = (int)time(0);
				Vec2 turret_position = game->getLevel()->getNearestPositionInGrid(getPosition());
				action["position"]["x"] = turret_position.x;
				action["position"]["y"] = turret_position.y;
				action["action"] = "upgrade_tower";
				game->addActionToTracker(action);
				game->getLevel()->decreaseQuantity(costs[level + 1]);
				upgradeCallback(sender);
				
				if ((int)game->getLevel()->getQuantity() < costs[level + 1]) {
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
					setString(((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()
						["buttons"]["level"][language].asString() + " " + Value(level + 2).asString());
				if (level < (int)cost_size - 2) {
					std::string s("");
					s = Value(damages[level + 1]).asString();
					s.resize(4);
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("attack_label"))->
						setString(s);
					s = Value(attack_speeds[level + 1]).asString();
					s.resize(4);
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("speed_label"))->
						setString(s);
					s = Value(round(ranges[level + 1] / Cell::getCellWidth() * 100) / 100).asString();
					s.resize(4);
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("range_label"))->
						setString(s);
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
						setString(Value(costs[level + 1]).asString());
				}
				else if (level == (int)cost_size - 2) {
					((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
						setString(Value(costs[level + 1]).asString());
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
				if (level >= level_max && level < (int)cost_size - 1) {
					layout->getChildByName("next_level_layout")->getChildByName("range")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("speed")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("attack")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("range_label")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("speed_label")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("attack_label")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("description_label")->setVisible(false);
					layout->getChildByName("next_level_layout")->getChildByName("locked_label")->setVisible(true);
					((ui::Button*)layout->getChildByName("next_level_button"))->setEnabled(false);
				}
				std::string s("");
				((Label*)layout->getChildByName("current_level_layout")->getChildByName("level_label"))->
					setString(((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues()
						["buttons"]["level"][language].asString()+ " " + Value(level + 1).asString());
				s = Value(damages[level]).asString();
				s.resize(4);
				((Label*)layout->getChildByName("current_level_layout")->getChildByName("attack_label"))->
					setString(s);
				s = Value(attack_speeds[level]).asString();
				s.resize(4);
				((Label*)layout->getChildByName("current_level_layout")->getChildByName("speed_label"))->
					setString(s);
				s = Value(round(ranges[level] / Cell::getCellWidth() * 100) / 100).asString();
				s.resize(4);
				((Label*)layout->getChildByName("current_level_layout")->getChildByName("range_label"))->
					setString(s);
				((Label*)layout->getChildByName("sell_label"))->setString(Value(sells[level]).asString());
			}
		}
	});
	
	

	auto nextlevel_layout = ui::Layout::create();
	nextlevel_layout->setContentSize(Size(panel->getContentSize().width * panel->getScaleX() / 3,
		panel->getContentSize().height * panel->getScaleY() * 3 / 4));
	nextlevel_layout->setPosition(Vec2(0, nextlevel_layout->getContentSize().height / 2));

	Label* level_n_label = Label::createWithTTF(config["level"][language].asString()+ " " + Value(level + 2).asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	level_n_label->setColor(Color3B::BLACK);
	level_n_label->setPosition(Vec2(nextlevel_layout->getContentSize().width * nextlevel_layout->getScaleX() / 2, 
		-level_n_label->getContentSize().height / 2));
	nextlevel_layout->addChild(level_n_label, 1, "level_label");

	s = Value(xp_levels[level + 1]).asString();
	int dot_pos = s.find('.');
	s = s.substr(0, dot_pos);
	Label* locked_label = Label::createWithTTF(config["locked"][language].asString() + "\n"+ Value(xp).asString() + "/" +
		s,
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	locked_label->setColor(Color3B::BLACK);
	locked_label->setPosition(level_n_label->getPosition() + Vec2(0, -level_n_label->getContentSize().width / 2
		-locked_label->getContentSize().height / 2));
	locked_label->setDimensions(nextlevel_layout->getContentSize().width, level_n_label->getContentSize().height * 2);
	locked_label->setHorizontalAlignment(TextHAlignment::CENTER);
	nextlevel_layout->addChild(locked_label, 1, "locked_label");

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

	auto description_label = Label::createWithTTF(spec_config["last_level_description_" +
		((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage()].asString(),
		"fonts/LICABOLD.ttf", 20 * visibleSize.width / 1280);
	description_label->setColor(Color3B::BLACK);
	description_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	description_label->setDimensions(nextlevel_layout->getContentSize().width * 2 * 0.85,
		nextlevel_layout->getContentSize().height * 3 / 4);
	description_label->setPosition(nextlevel_layout->getContentSize().width * nextlevel_layout->getScaleX() / 2,
		level_n_label->getPosition().y - level_n_label->getContentSize().height / 2 -
		description_label->getContentSize().height / 2);
	nextlevel_layout->addChild(description_label, 1, "description_label");

	auto upgrade_label = Label::createWithTTF(config["upgrade"][language].asString(),
		"fonts/LICABOLD.ttf", 30 * visibleSize.width / 1280);
	upgrade_label->setColor(Color3B::YELLOW);
	upgrade_label->enableOutline(Color4B::BLACK, 1);
	upgrade_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	upgrade_label->setPosition(nextlevel_layout->getContentSize().width * nextlevel_layout->getScaleX() / 2,
		range_ni->getPosition().y - range_ni->getContentSize().height * range_ni->getScaleY()/ 2 -
		upgrade_label->getContentSize().height / 2);
	nextlevel_layout->addChild(upgrade_label, 1, "upgrade");

	cost->setPosition(size_sprite, upgrade_label->getPosition().y - upgrade_label->getContentSize().height / 2 - size_sprite / 2);

	s = Value(damages[level + 1]).asString();
	s.resize(4);
	Label* attack_n_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	attack_n_label->setColor(Color3B::BLACK);
	attack_n_label->setPosition(Vec2(attack_n->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		attack_n->getPosition().y));
	attack_n_label->setAnchorPoint(Vec2(0.f, 0.5f));
	nextlevel_layout->addChild(attack_n_label, 1, "attack_label");

	s = Value(round(ranges[level + 1] / Cell::getCellWidth() * 100) / 100).asString();
	s.resize(4);
	Label* range_n_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	range_n_label->setColor(Color3B::BLACK);
	range_n_label->setPosition(Vec2(range_ni->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		range_ni->getPosition().y));
	range_n_label->setAnchorPoint(Vec2(0.f, 0.5f));
	nextlevel_layout->addChild(range_n_label, 1, "range_label");

	s = Value(attack_speeds[level + 1]).asString();
	s.resize(4);
	Label* speed_n_label = Label::createWithTTF(s, "fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	speed_n_label->setColor(Color3B::BLACK);
	speed_n_label->setPosition(Vec2(speed_n->getPosition().x + attack_n->getContentSize().width * attack_n->getScale(),
		speed_n->getPosition().y));
	speed_n_label->setAnchorPoint(Vec2(0.f, 0.5f));
	nextlevel_layout->addChild(speed_n_label, 1, "speed_label");

	s = Value(costs[level + 1]).asString();
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

	if (level < level_max) {
		locked_label->setVisible(false);
		if (level < (int)damages.size()) {
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
		if (level == damages.size() - 2) {
			range_ni->setVisible(false);
			speed_n->setVisible(false);
			attack_n->setVisible(false);
			attack_n_label->setVisible(false);
			range_n_label->setVisible(false);
			speed_n_label->setVisible(false);
			description_label->setVisible(true);
		}
		MyGame* game = SceneManager::getInstance()->getGame();
		if ((int)game->getLevel()->getQuantity() < costs[level + 1]) {
			nextlevel_button->setEnabled(false);
		}
	}
	else {
		max_level_label->setVisible(false);
		range_ni->setVisible(false);
		speed_n->setVisible(false);
		attack_n->setVisible(false);
		attack_n_label->setVisible(false);
		range_n_label->setVisible(false);
		speed_n_label->setVisible(false);
		description_label->setVisible(false);
		locked_label->setVisible(true);
		nextlevel_button->setEnabled(false);
	}
	auto sell = ui::Button::create("res/buttons/sell.png");
	sell->setScaleX(panel->getContentSize().width * panel->getScaleX() / sell->getContentSize().width * 0.3);
	sell->setScaleY(panel->getContentSize().height * panel->getScaleY() / sell->getContentSize().height * 0.3);
	sell->setPosition(Vec2( -panel->getContentSize().width * panel->getScaleX() * 0.3, 
		current_level_layout->getPosition().y + range_i->getPosition().y - size_sprite / 2 -
		sell->getContentSize().height* sell->getScaleY() / 2));
	sell->addTouchEventListener([&](Ref *sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
			MyGame* game = SceneManager::getInstance()->getGame();
			game->getLevel()->increaseQuantity(sells[level]);
			destroyCallback(sender);
			SceneManager::getInstance()->getGame()->getMenu()->hideTowerInfo();
			Json::Value action;
			action["tower_name"] = name;
			action["time"] = (int)time(0);
			Vec2 turret_position = game->getLevel()->getNearestPositionInGrid(getPosition());
			action["position"]["x"] = turret_position.x;
			action["position"]["y"] = turret_position.y;
			action["action"] = "sell_tower";
			game->addActionToTracker(action);
		}
	});
	layout->addChild(sell, 1);

	auto sell_label = Label::createWithTTF(config["sell"][language].asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	sell_label->enableOutline(Color4B::BLACK, 2);
	sell_label->setAlignment(cocos2d::TextHAlignment::CENTER);
	sell_label->setPosition(sell->getPosition().x, sell->getPosition().y + sell_label->getContentSize().height / 2);
	layout->addChild(sell_label,1);

	auto sell_cost = Label::createWithTTF(Value(sells[level]).asString(),
		"fonts/LICABOLD.ttf", 25 * visibleSize.width / 1280);
	sell_cost->enableOutline(Color4B::BLACK, 2);
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
	const auto config = getSpecConfig();
	if (level < level_max) {
		if (layout->getChildByName("next_level_layout")->getChildByName("locked_label")->isVisible()) {
			layout->getChildByName("next_level_layout")->getChildByName("locked_label")->setVisible(false);

			if (level < (int)config["damages"].size()) {
				layout->getChildByName("next_level_layout")->getChildByName("range")->setVisible(true);
				layout->getChildByName("next_level_layout")->getChildByName("speed")->setVisible(true);
				layout->getChildByName("next_level_layout")->getChildByName("attack")->setVisible(true);
				layout->getChildByName("next_level_layout")->getChildByName("range_label")->setVisible(true);
				layout->getChildByName("next_level_layout")->getChildByName("speed_label")->setVisible(true);
				layout->getChildByName("next_level_layout")->getChildByName("attack_label")->setVisible(true);
				layout->getChildByName("next_level_layout")->getChildByName("description_label")->setVisible(false);
			}
			else {
				layout->getChildByName("next_level_layout")->setVisible(false);
				((ui::Button*)layout->getChildByName("next_level_button"))->setEnabled(false);
			}
			if (level == (int)config["damages"].size() - 2) {
				layout->getChildByName("next_level_layout")->getChildByName("range")->setVisible(false);
				layout->getChildByName("next_level_layout")->getChildByName("speed")->setVisible(false);
				layout->getChildByName("next_level_layout")->getChildByName("attack")->setVisible(false);
				layout->getChildByName("next_level_layout")->getChildByName("range_label")->setVisible(false);
				layout->getChildByName("next_level_layout")->getChildByName("speed_label")->setVisible(false);
				layout->getChildByName("next_level_layout")->getChildByName("attack_label")->setVisible(false);
				layout->getChildByName("next_level_layout")->getChildByName("description_label")->setVisible(true);
			}
		}
		if ((int)game->getLevel()->getQuantity() < config["cost"][level + 1].asInt()) {
			((ui::Button*)layout->getChildByName("next_level_button"))->
				setEnabled(false);
			((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
				setColor(Color3B::RED);
		}
		else {
			if (level + 1 < (int)config["cost"].size() && !((ui::Button*)layout->getChildByName("next_level_button"))->isEnabled()) {
				((ui::Button*)layout->getChildByName("next_level_button"))->
					setEnabled(true);
			}
			else if (level + 1 >= (int)config["cost"].size() && (((ui::Button*)layout->getChildByName("next_level_button"))->isEnabled()
				|| layout->getChildByName("next_level_layout")->isVisible())) {
				((ui::Button*)layout->getChildByName("next_level_button"))->
					setEnabled(false);
				layout->getChildByName("next_level_layout")->setVisible(false);
				layout->getChildByName("max_level_label")->setVisible(true);
			}
			((Label*)layout->getChildByName("next_level_layout")->getChildByName("cost_label"))->
				setColor(Color3B::YELLOW);
		}
	}
	else {
		if (!layout->getChildByName("next_level_layout")->getChildByName("locked_label")->isVisible()) {
			layout->getChildByName("next_level_layout")->getChildByName("locked_label")->setVisible(true);
			((ui::Button*)layout->getChildByName("next_level_button"))->setEnabled(false);
		}
		std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();
		std::string s = Value(xp_levels[level + 1]).asString();
		int dot_pos = s.find('.');
		s = s.substr(0, dot_pos);
		if (((Label*)layout->getChildByName("next_level_layout")->getChildByName("locked_label"))->getString() !=
			((AppDelegate*)Application::getInstance())->getConfig()
			["buttons"]["locked"][language].asString() + "\n" + Value(xp).asString() + "/" + s) {
			((Label*)layout->getChildByName("next_level_layout")->getChildByName("locked_label"))->setString(
				((AppDelegate*)Application::getInstance())->getConfig()
				["buttons"]["locked"][language].asString() + "\n" + Value(xp).asString() + "/" + s);
		}
	}
}

void Tower::updateDirection() {
	Vec2 vec = target->getPosition() - getPosition();
	float angle = vec.getAngle();
	if (abs(angle) <= M_PI_4 / 2) {
		direction = RIGHT;
	}
	else if (abs(angle) >= 7 * M_PI_4 / 2) {
		direction = LEFT;
	}
	else if (angle < -M_PI_4 / 2 && angle > -7 * M_PI_4 / 2) {
		direction = DOWN;
	}
	else if (angle > M_PI_4 / 2 && angle < 7 * M_PI_4 / 2) {
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

void Tower::blockTower(bool block) {
	blocked = block;
}

bool Tower::isTowerBlocked() {
	return blocked;
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