#ifndef TOWER_HPP
#define TOWER_HPP

#include "cocos2d.h"
#include "../Config/json.h"
#include "ui/CocosGUI.h"
#include <spine/spine-cocos2dx.h>


class Cell;
class Dango;
class InterfaceGame;

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
		BLINKING_DOWN 
	};
	enum TowerType{
		ARCHER,
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
	void initFromConfig();

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
	double getCost();
	Dango* getTarget();
	double getDamage();
	double getAttackSpeed();
	int getLevel();
	Tower::State getState();
	void setState(Tower::State state);
	void setTarget(Dango* dango);
	void displayRange(bool disp);
	cocos2d::ui::Layout* getInformationLayout(InterfaceGame* interface_game);
	static TowerType getTowerTypeFromString(std::string type);
	cocos2d::Vector<cocos2d::SpriteFrame*> getAnimation(Tower::State animState);
	static SkeletonAnimation* getSkeletonAnimationFromName(std::string name);
	static cocos2d::Vector<cocos2d::SpriteFrame*> getAnimationFromName(std::string name, Tower::State animState);
	virtual void removeTarget(Dango* dango);
	
	// Updates
	virtual void update(float dt);
	virtual void updateDisplay(float dt);
	virtual void updateInformationLayout(cocos2d::ui::Layout* layout);
	virtual void updateEnrageLayout();
	virtual void handleEnrageMode();
	virtual void handleEndEnrageAnimation() = 0;
	virtual void chooseTarget(std::vector<Dango*> targets);
	virtual void givePDamages(double damage);
	virtual void reload();
	
	// Callbacks
	void destroyCallback(Ref* sender);
	void builtCallback(Ref* sender);
	void upgradeCallback(Ref* sender);
	virtual void startLimit() = 0;

	// Config
	static Json::Value getConfig();
	virtual const Json::Value getSpecConfig() = 0;

protected:
	//State attribute
	State state;
	bool fixed;
	bool selected;
	bool destroy;
	
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
	void updateDirection();
};


#endif
