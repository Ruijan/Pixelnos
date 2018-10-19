#ifndef TOWER_HPP
#define TOWER_HPP

#include "cocos2d.h"
#include "../Config/json.h"

#include "ui/CocosGUI.h"
#include <spine/spine-cocos2dx.h>
#include "../Config/Settings/TowerSettings.h"

class Cell;
class Dango;
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
	double getDamage();
	double getAttackSpeed();
	std::string getName();
	unsigned int getLevel();
	Tower::State getState();
	void displayRange(bool disp);
	bool isSameType(Tower::TowerType type);
	cocos2d::Vector<cocos2d::SpriteFrame*> getAnimation(Tower::State animState);
	static SkeletonAnimation* createSkeletonAnimationFromName(std::string name);
	virtual void removeTarget(Dango* dango);
	void incrementXP(int amount);
	void blockTower();
	void freeTower();
	bool isTowerBlocked();
	virtual void stopAttacking();
	void changeSpeedAnimation(float speed);

	TowerSettings* getTowerSettings();
	unsigned int getMaxLevel();
	unsigned int getCurrentXP();
	
	// Updates
	virtual void update(float dt);
	void updateState(float dt);
	void updateReloading();
	void updateAttacking(float dt);
	void updateAware(float dt);
	void setIDLEState();
	void updateIDLE();
	virtual void updateDisplay(float dt);
	void updateOpacity(float dt);
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

	TowerSettings* settings;
	
	Dango* target;
	std::map<Dango*, int> attacked_enemies;
	Direction direction;
	
	// Characteristics
	double timer;
	double timerIDLE;
	unsigned int level;
	unsigned int level_max;
	int xp;

	int nb_attacks;
	int nb_max_attacks_limit;
	
	cocos2d::DrawNode* loadingCircle;

	// parameters for animation
	double animation_duration;
	double nb_frames_anim;
	std::string name;
	SkeletonAnimation* skeleton;
	cocos2d::Sprite* image;
	
	//methods
	virtual void attack() = 0;
	void startAnimation(float speed = 1);
	void setOrientedAnimation();
	void updateDirection();
};
void initAnimatedSkeleton(spine::SkeletonAnimation * animated_skeleton);


#endif
