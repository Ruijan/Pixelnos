#include "Dangobese.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"
#include "../Towers/Attack.h"

USING_NS_CC;

Dangobese::Dangobese(std::vector<Cell*> npath, int nlevel) :
Dango(npath, nlevel){
}

Dangobese::~Dangobese() {
	//std::cerr << "Dangobese Destroyed ! confirmed !" << std::endl;
}

Dangobese* Dangobese::create(std::vector<Cell*> npath, int nlevel)
{
	Dangobese* pSprite = new Dangobese(npath, nlevel);

	/*if (pSprite->initWithFile(getConfig()["level"][nlevel]["image"].asString()))
	{*/
		pSprite->setScale(Cell::getCellWidth() / pSprite->getContentSize().width);
		pSprite->autorelease();

		pSprite->setAnchorPoint(Point(0.5,0.25));
		pSprite->updateAnimation();

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
			double speed = 500;
			double time_to_reach = distance / speed;
			if (distance < 150 * visibleSize.width / 960 &&
				time_to_reach < attack_duration - attack_spe_reload_timer &&
				!((WaterBall*)attack)->isForceApplied()) {
				Vec2 direction = Vec2(attack->getPosition().x - getPosition().x,
					attack->getPosition().y - getPosition().y).getNormalized();
				Vec2 npos = attack->getPosition() - direction * speed * dt * visibleSize.width / 960;
				attack->setPosition(npos);
				attack->setEnabled(false);
				((WaterBall*)attack)->setForceApplied(true);
				if (distance < 20 * visibleSize.width / 960) {
					attack->setHasToBeDeleted(true);
				}
			}
		}
	}
}

bool Dangobese::shouldAttackSpe() {
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
	return false;
}

void Dangobese::updateAnimation() {
	Dango::updateAnimation();
	// Add the case where the state of the system is ATTACK_SPE
}

void Dangobese::update(float dt) {
	updateEffects(dt);
	Size visibleSize = Director::getInstance()->getVisibleSize();

	reload_timer += dt;
	attack_spe_reload_timer += dt;
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
		if (shouldAttack()) {
			attack(dt);
			state = RELOAD;
		}
		else {
			state = MOVE;
			updateAnimation();
		}
		break;
	case ATTACK_SPE:
		if (shouldAttackSpe()) {
			attackSpe(dt);
			state = MOVE;
			attack_spe_reload_timer = 0;
		}
		else {
			state = MOVE;
			updateAnimation();
		}
		break;
	case MOVE:
		if (path[targetedCell]->isFree()) {
			move(dt);
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
