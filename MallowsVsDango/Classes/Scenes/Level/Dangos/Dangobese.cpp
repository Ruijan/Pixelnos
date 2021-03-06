#include "Dangobese.h"
#include "../Cell.h"
#include "../../../AppDelegate.h"
#include "../Towers/Attacks/ThrowableAttacks/WaterBall.h"

USING_NS_CC;

Dangobese::Dangobese(std::vector<Cell*> npath, int nlevel) :
Dango(npath, nlevel){
	auto config = getSpecConfig();
	attack_spe_reload_time = config["attack_spe"]["reload_spe"][nlevel].asDouble();
	attack_spe_duration = config["attack_spe"]["attack_duration"][nlevel].asDouble();
	attack_spe_reload_timer = 0;
	attack_spe_timer = 0;
}

Dangobese::~Dangobese() {
	//std::cerr << "Dangobese Destroyed ! confirmed !" << std::endl;
}

Dangobese* Dangobese::create(std::vector<Cell*> npath, int nlevel)
{
	Dangobese* pSprite = new Dangobese(npath, nlevel);

	/*if (pSprite->initWithFile(getConfig()["level"][nlevel]["image"].asString()))
	{*/
		pSprite->initFromConfig();
		pSprite->autorelease();

		return pSprite;
	//}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Dangobese::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::DANGO)["dangobese"];
}
Json::Value Dangobese::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::DANGO)["dangobese"];
}

void Dangobese::attackSpe(float dt) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	std::vector<Attack*> attacks = ((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getAttacks();
	for (auto& attack : attacks) {
		if (!isAffectedByAttack(attack)) {
			double distance = attack->getPosition().distance(getPosition());
			if (distance < 150 * visibleSize.width / 960 &&
				!((WaterBall*)attack)->isForceApplied()) {
				attack->setEnabled(false);
				attack->setTarget(this);
				((WaterBall*)attack)->setForceApplied(true);
			}
		}
	}
}

bool Dangobese::shouldAttackSpe() {
	if (attack_spe_reload_timer > attack_spe_reload_time) {
		std::vector<Attack*> attacks = ((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->getAttacks();
		for (auto& attack : attacks) {
			if (!isAffectedByAttack(attack)) {
				double distance = sqrt(pow(attack->getPosition().x - getPosition().x, 2) +
					pow(attack->getPosition().y - getPosition().y, 2));
				if (distance < 150 && !((WaterBall*)attack)->isForceApplied()) {
					return true;
				}
			}
		}
	}
	return false;
}

std::string Dangobese::getAttackSpeAnimationName() {
	return "aspirate";
}

void Dangobese::update(float dt) {
	updateEffects(dt);
	Size visibleSize = Director::getInstance()->getVisibleSize();

	reload_timer += dt;
	if (attack_spe_done) {
		attack_spe_reload_timer += dt;
	}
	
	switch (state) {
	case IDLE:
		if (shouldAttack() && reload_timer > attack_reloading) {
			state = ATTACK;
			updateAnimation();
			reload_timer = 0;
		}
		else if (path[targetedCell]->isFree()) {
			state = MOVE;
			updateAnimation();
		}
		break;
	case ATTACK:
		updateAttack(dt);
		break;
	case ATTACK_SPE:
		if (shouldAttackSpe() && attack_spe_done) {
			attackSpe(dt);
			startAttackSpeAnimation();
			attack_spe_reload_timer = 0;
			attack_spe_timer = 0;
			attack_spe_done = false;
		}
		else if(!shouldAttackSpe() && attack_spe_done) {
			state = MOVE;
			updateAnimation();
		}
		else if (!attack_spe_done) {
			if (attack_spe_timer > attack_spe_duration) {
				state = MOVE;
				updateAnimation();
				attack_spe_done = true;
			}
			else {
				attackSpe(dt);
				attack_spe_timer += dt;
			}
		}
		break;
	case MOVE:
		if (path[targetedCell]->isFree()) {
			if (!on_ground || !stay_on_ground) {
				move(dt);
			}
			if (shouldAttackSpe()) {
				state = ATTACK_SPE;
				updateAnimation();
			}
			else if (shouldAttack() && reload_timer > attack_reloading) {
				state = ATTACK;
				updateAnimation();
				reload_timer = 0;
			}
		}
		else {
			state = IDLE;
		}
		break;
	}
}

bool Dangobese::isAffectedByAttack(Attack* attack) {
	return attack->affectEnemy(this);
}

void Dangobese::startAttackSpeAnimation() {
	skeleton->setAnimation(0, "aspirate", true);
}