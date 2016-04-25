#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "cocos2d.h"
#include "Cell.h"
#include "../Dangos/Dango.h"
#include "../Towers/Tower.h"
#include "../Towers/Attack.h"
#include "DangoGenerator.h"
#include "Wall.h"
#include <iostream>


/**
@brief    The current level(game level or start screen or save screen) of the application.

Like a specific screen with all the caracteristics. It's an abstract class.
*/

bool sortZOrder(cocos2d::Node* sprite1, cocos2d::Node* sprite2);
typedef unsigned int Quantity;

class Level : public cocos2d::Layer
{
	
public:
	Level(unsigned int nLevel);
	virtual ~Level();
	static Level* create(unsigned int nLevel);
	virtual bool init();
	void initWalls();

	virtual void update(float dt);
	void updateTowers(float dt);
	void removeElements();

	bool isPaused();
	Quantity getQuantity();
	Quantity getLife();
	int getTotalExperience();
	unsigned int getLevelId();
	void increaseQuantity(Quantity add);
	bool decreaseQuantity(Quantity removed);
	virtual void pause();
	virtual void resume();
	void addTurret(Tower* turret);
	Cell* getNearestCell(cocos2d::Vec2 position);
	Tower* touchingTower(cocos2d::Vec2 position);
	Dango* touchingDango(cocos2d::Vec2 position);

	std::vector<Dango*> getEnemiesInRange(cocos2d::Vec2 position, double range);
	std::vector<Dango*> getEnemies();
	std::vector<Cell*> getPath(int path);
	void addDango(Dango* dango);
	void addAttack(Attack* bullet);
	bool hasLost();
	void reset();
	std::vector<Attack*> getAttacks();

	void showGrid(bool show);

protected:
	unsigned int id;
	cocos2d::Size size;

	std::vector<Dango*> dangos;
	std::vector<Tower*> turrets;
	std::vector<Attack*> attacks;
	std::vector<Wall*> walls;

	DangoGenerator* generator;

	std::vector<std::vector<Cell*>> cells;
	std::vector<std::vector<Cell*>> paths;

	bool paused;
	int zGround;
	Quantity sugar;
	Quantity life;
	int experience;
	
	void reorder();
};

#endif
