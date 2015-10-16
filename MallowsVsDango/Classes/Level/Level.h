#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "cocos2d.h"
#include "Cell.h"
#include "../Dangos/Dango.h"
#include "../Towers/Tower.h"
#include "../Towers/Bullet.h"
#include "DangoGenerator.h"
#include <iostream>


/**
@brief    The current level(game level or start screen or save screen) of the application.

Like a specific screen with all the caracteristics. It's an abstract class.
*/
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::vector<std::string>> readMapFromCSV(std::string filename);
std::vector<std::string> readPathFromCSV(std::string filename);
typedef unsigned int Quantity;

class Level : public cocos2d::Layer
{
	
public:
	Level(cocos2d::Size nsize);
	virtual ~Level();
	static Level* create(cocos2d::Size nsize);
	virtual bool init();

	virtual void update(float dt);

	void setSize(cocos2d::Size nsize);
	bool isFinished();
	bool isLoaded();
	Quantity getQuantity();
	Quantity getLife();
	void increaseQuantity(Quantity add);
	bool decreaseQuantity(Quantity removed);
	virtual void pause();
	virtual void resume();
	void addTurret(Tower* turret);
	Cell* getNearestCell(cocos2d::Vec2 position);
	Tower* touchingTower(cocos2d::Vec2 position);
	std::vector<Cell*> getPath();
	void addDango(Dango* dango);
	void addBullet(Bullet* bullet);
	bool hasLost();
	bool hasWon();
	void reset();
	

	//CREATE_FUNC(Level);

protected:
	std::vector<Dango*> dangos;
	std::vector<Tower*> turrets;
	std::vector<Bullet*> bullets;
	std::vector<std::vector<Cell*>> cells;
	cocos2d::Size size;
	std::vector<Cell*> path;
	bool paused;
	bool loaded;
	cocos2d::Point start;
	cocos2d::Point end;
	Quantity sugar;
	Quantity life;
	DangoGenerator* generator;	

	void createPath(std::vector<std::vector<std::string>> );
	void reorder();
};

#endif
