#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "cocos2d.h"
#include "Cell.h"
#include "../Dangos/Dango.h"
#include "../Towers/Tower.h"
#include "../Towers/Bullet.h"
#include "DangoGenerator.h"
#include "../Dialogue.h"
#include <iostream>


/**
@brief    The current level(game level or start screen or save screen) of the application.

Like a specific screen with all the caracteristics. It's an abstract class.
*/
std::vector<std::vector<std::string>> readMapFromCSV(std::string filename);
std::vector<std::string> readPathFromCSV(std::string filename);
bool sortZOrder(cocos2d::Sprite* sprite1, cocos2d::Sprite* sprite2);
typedef unsigned int Quantity;

class Level : public cocos2d::Layer
{
	
public:
	enum State{
		INTRO,
		TITLE,
		STARTING,
		RUNNING,
		ENDING,
		DONE
	};
	Level(int nLevel);
	virtual ~Level();
	static Level* create(int nLevel);
	virtual bool init();

	virtual void update(float dt);

	void setSize(cocos2d::Size nsize);
	bool isFinishing();
	bool isLoaded();
	bool isPaused();
	bool isCellInPath(Cell* cell);
	Quantity getQuantity();
	Quantity getLife();
	int getTotalExperience();
	int getLevelId();
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
	int id;
	std::vector<Dango*> dangos;
	std::vector<Tower*> turrets;
	std::vector<Bullet*> bullets;
	Dialogue* introDialogue;
	std::vector<std::vector<Cell*>> cells;
	cocos2d::Size size;
	std::vector<Cell*> path;
	bool paused;
	bool loaded;
	cocos2d::Point start;
	cocos2d::Point end;
	int zGround;
	Quantity sugar;
	Quantity life;
	DangoGenerator* generator;	
	State state;
	double timer;
	int experience;
	

	void createPath(std::vector<std::vector<std::string>> );
	void reorder();
};

#endif
