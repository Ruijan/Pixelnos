#ifndef TOWER_HPP
#define TOWER_HPP

#include "cocos2d.h"
#include "../Config/json.h"

#include "ui/CocosGUI.h"
#include <spine/spine-cocos2dx.h>
#include "../Config/Settings/TowerSettings.h"

class Cell;
class Dango;
class LevelInterface;
class Config;

using namespace spine;

class Tower : public cocos2d::Node{

public:
	enum State{
		IDLE = 0,
		AWARE,
		ATTACKING,
		RELOADING,
		LIMIT_BURSTING,
		BLINKING_UP,
		BLINKING_DOWN,
		BLOCKED
	};
	enum TowerType{
		BOMBER,
		CUTTER,
		SAUCER
	};
	static std::vector<TowerType> getAllTowerTypes();

	enum Direction {
		RIGHT,
		UP,
		DOWN,
		LEFT
	};

	Tower();
	virtual ~Tower();
	void initDebug();
	void initEnragePanel();
	void initFromConfig(Config* configClass);

	void pauseAnimation();
	void resumeAnimation();

	// Setters & Getters
	bool isFixed();
	void setFixed(bool f);
	bool isSelected();
	void setSelected(bool select);
	bool isLimitReached();
	bool hasToBeDestroyed();
	double getRange();
	double getNormalizedRange();
	double getNormalizedRangeFromRange(double range);
	double getCost();
	Dango* getTarget();
	double getDamage();
	double getAttackSpeed();
	std::string getName();
	int getLevel();
	Tower::State getState();
	void setState(Tower::State state);
	void setTarget(Dango* dango);
	void displayRange(bool disp);
	bool isSameType(std::string type);
	static TowerType getTowerTypeFromString(std::string type);
	cocos2d::Vector<cocos2d::SpriteFrame*> getAnimation(Tower::State animState);
	static SkeletonAnimation* getSkeletonAnimationFromName(std::string name);
	static cocos2d::Vector<cocos2d::SpriteFrame*> getAnimationFromName(std::string name, Tower::State animState);
	virtual void removeTarget(Dango* dango);
	void incrementXP(int amount);
	void blockTower(bool block);
	bool isTowerBlocked();
	virtual void stopAttacking();
	void changeSpeedAnimation(float speed);

	TowerSettings* getTowerSettings();
	int getMaxLevel();
	int getCurrentXP();
	
	// Updates
	virtual void update(float dt);
	void updateState(float dt);
	void updateReloading();
	void updateAttacking(float dt);
	void updateBlocked();
	void updateAware(float dt);
	void updateIDLE();
	virtual void updateDisplay(float dt);
	virtual void updateEnrageLayout();
	virtual void handleEnrageMode();
	virtual void handleEndEnrageAnimation() = 0;
	virtual void chooseTarget(std::vector<Dango*> targets);
	virtual bool isPotentialTarget(Dango *);
	virtual void givePDamages(double damage);
	virtual void reload();
	
	// Callbacks
	void destroyCallback(Ref* sender);
	void builtCallback(Ref* sender);
	void upgradeCallback(Ref* sender);
	virtual void startLimit() = 0;

	// Config
	static const Json::Value& getConfig();
	virtual const Json::Value& getSpecConfig() = 0;



protected:
	virtual TowerType getType() = 0;
protected:
	//State attribute
	State state;
	bool fixed;
	bool selected;
	bool destroy;
	bool limit_enabled;
	bool blocked;

	TowerSettings* settings;
	
	Dango* target;
	std::map<Dango*, int> attacked_enemies;
	Direction direction;
	
	// Characteristics
	double cost;
	double attack_speed;
	double damage;
	double range;
	double timer;
	double timerIDLE;
	int level;
	int level_max;
	int xp;

	int nb_attacks;
	int nb_max_attacks_limit;
	
	//
	cocos2d::DrawNode* loadingCircle;

	// parameters for animation
	double animation_duration;
	double nb_frames_anim;
	std::string name;
	SkeletonAnimation* skeleton;
	cocos2d::Sprite* image;
	bool spritesheet;
	
	//methods
	virtual void attack() = 0;
	void startAnimation(float speed = 1);
	void setOrientedAnimation();
	void updateDirection();
};


#endif
