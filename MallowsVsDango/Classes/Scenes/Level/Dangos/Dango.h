#pragma once

#include "cocos2d.h"
#include "../../../Config/json.h"
#include "ui/CocosGUI.h"
#include "../Towers/Attacks/Effect.h"
#include <spine/spine-cocos2dx.h>
#include "GUI/DangoInformationPanel.h"

class Cell;
class Tower;
class Attack;

using namespace spine;

class Dango : public cocos2d::Node{
public:
	enum DIRECTION {
		RIGHT,
		UP,
		DOWN,
		LEFT
	};
	enum STATE {
		IDLE,
		ATTACK,
		ATTACK_SPE,
		RELOAD,
		MOVE,
		DYING,
		DEAD
	};
	Dango(std::vector<Cell*> npath, int nlevel);
	virtual ~Dango();
	//static Dango* create(std::string image, std::vector<Cell*> npath, int level);

	static Json::Value getConfig();
	virtual Json::Value getSpecConfig() = 0;
	void initFromConfig();
	void initDangoCharacteristics(Json::Value &config);
	void initSkeleton(Json::Value &config);
	void initHolySugarNumber(Json::Value &config);

	void showLifeBar();
	void hideLifeBar();

	bool isAlive();
	bool isDying();
	bool isDone();
	bool willBeAlive();
	virtual bool shouldAttack();
	double getSpeedReductionRatio();
	double getHitPoints();
	double getGain();
	int getTargetedCell();
	int getNbCellsToPath();
	double getSpeed();
	void updateAnimationDirection(Dango::STATE state, Dango::DIRECTION direction, double gameSpeed);
	virtual std::string getAttackSpeAnimationName();
	int getXP();
	int getHolySugar();
	unsigned int getLevel();

	virtual void update(float dt);
	virtual void updateMove(float dt);
	virtual void updateReload(float dt);
	virtual void updateAttack(float dt);
	virtual void updateIDLE();
	virtual void updateAnimation();
	virtual void skeletonAnimationHandle();
	void pauseAnimation();
	void resumeAnimation();
	void updateDirection(cocos2d::Vec2 direction);
	void updateEffects(float dt);

	void takeDamages(double damages);
	void applyProspectiveDamages(int id_damage);
	int takePDamages(double damages, double delay);
	void removePDamages(int id_damage);
	
	void move(float dt);
	virtual void attack(float dt);
	void addEffect(Effect* effect);
	void changeSpeedAnimation(float speed);

	void addTargetingTower(Tower* tower);
	void addTargetingAttack(Attack* tower);
	int addSpeedModifier(std::pair<double, Effect*> pair);
	int addDamagesModifier(std::pair<double, Effect*> pair);
	void removeSpeedModifier(int id);
	void removeDamageModifier(int id);
	void removeTargetingTower(Tower* tower);
	void removeTargetingAttack(Attack* tower);

	void die();	
	virtual void endDyingAnimation();

protected:
	STATE state;
	DIRECTION cDirection;

	unsigned int targetedCell;
	std::vector<Cell*> path;
	cocos2d::Action* cAction;

	DangoInformationPanel* lifeBar;

	std::string name;
	SkeletonAnimation* skeleton;

	std::map<int, std::pair<double, Effect*>> m_speed;
	std::map<int, std::pair<double, Effect*>> m_damages;
	std::vector<Tower*> targeting_towers;
	std::vector<Attack*> targeting_attacks;
	int modifier_id;

	double speed;
	double hitPoints;
	double maxHitPoints;
	double pDamages;				// prosepctive damages
	std::map<int, double> prospective_damages;
	int id_damages;
	double reload_timer;
	double attack_damages;
	double attack_reloading;
	bool on_ground;
	unsigned int level;
	int xp;
	int holy_sugar;
	bool stay_on_ground;

	std::vector<Effect*> effects;
};

std::string getStringFromState(Dango::STATE state);
std::string getStringFromDirection(Dango::DIRECTION state);
