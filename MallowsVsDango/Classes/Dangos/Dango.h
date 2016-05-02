#ifndef DANGO_HPP
#define DANGO_HPP

#include "cocos2d.h"
#include "../Config/json.h"
#include "ui/CocosGUI.h"
#include "Towers/Effect.h"

class Cell;
class InterfaceGame;
class Tower;
class Attack;

class Dango : public cocos2d::Sprite{
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
		RELOAD,
		MOVE
	};
	Dango(std::vector<Cell*> npath, double nspeed, double hp, int nlevel,
		double damages, double a_reload, double anim_duration, int nb_frames, std::string nname);
	virtual ~Dango();
	//static Dango* create(std::string image, std::vector<Cell*> npath, int level);

	static Json::Value getConfig();
	virtual Json::Value getSpecConfig() = 0;

	bool isAlive();
	bool isDone();
	bool willBeAlive();
	virtual bool shouldAttack();
	double getHitPoints();
	double getGain();
	int getTargetedCell();
	double getSpeed();

	virtual void update(float dt);
	void updateAnimation();
	void updateDirection(cocos2d::Vec2 direction);

	void takeDamages(double damages);
	void applyProspectiveDamages(int id_damage);
	int takePDamages(double damages, double delay);
	void removePDamages(int id_damage);
	
	void move(float dt);
	virtual void attack(float dt);
	virtual void runAnimation(cocos2d::Animation* anim);
	void addEffect(Effect* effect);

	cocos2d::ui::Layout* getInformationLayout(InterfaceGame* interface_game);
	virtual void updateInformationLayout(cocos2d::ui::Layout* layout);

	void addTargetingTower(Tower* tower);
	void addTargetingAttack(Attack* tower);
	int addSpeedModifier(std::pair<double, Effect*> pair);
	int addDamagesModifier(std::pair<double, Effect*> pair);
	void removeSpeedModifier(int id);
	void removeDamageModifier(int id);
	void removeTargetingTower(Tower* tower);
	void removeTargetingAttack(Attack* tower);
	
protected:
	STATE state;
	DIRECTION cDirection;

	unsigned int targetedCell;
	std::vector<Cell*> path;
	cocos2d::Action* cAction;

	double animation_duration;
	double nb_frames_anim;
	std::string name;

	std::map<int, std::pair<double, Effect*>> m_speed;
	std::map<int, std::pair<double, Effect*>> m_damages;
	std::vector<Tower*> targeting_towers;
	std::vector<Attack*> targeting_attacks;
	int modifier_id;

	double speed;
	double hitPoints;
	double pDamages;				// prosepctive damages
	std::map<int, double> prospective_damages;
	int id_damages;
	double reload_timer;
	double attack_damages;
	double attack_reloading;
	int level;
	
	std::vector<Effect*> effects;
};
#endif
