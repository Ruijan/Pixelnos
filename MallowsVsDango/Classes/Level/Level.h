#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "cocos2d.h"
#include "Cell.h"
#include "../Dangos/Dango.h"
#include "../Towers/Tower.h"
#include "../Towers/Attacks/Attack.h"
#include "DangoGenerator.h"
#include "Wall.h"
#include <iostream>


/**
@brief    The current level(game level or start screen or save screen) of the application.

Like a specific screen with all the caracteristics. It's an abstract class.
*/

bool sortZOrder(cocos2d::Node* sprite1, cocos2d::Node* sprite2);
float getMaxXPositionFromSprite(cocos2d::Node * sprite);
float getMaxYPositionFromSprite(cocos2d::Node * sprite);
Json::Value tryGettingLevelElementsConfig(Json::Value& allLevelsConfig);
typedef unsigned int Quantity;

class Level : public cocos2d::Layer
{
	
public:
	Level(unsigned int nLevel, unsigned int nWorld);
	virtual ~Level();
	static Level* create(unsigned int nLevel, unsigned int nWorld);
	virtual bool init();
	void initLevelValues(Json::Value &currentLevelConfig, Json::Value &save_file);
	void initObjects(Json::Value &root, double ratio, cocos2d::Size &visibleSize);
	void initLockedCells(Json::Value &root, double ratio, cocos2d::Size &visibleSize);
	void initPaths(Json::Value &root, double ratio, cocos2d::Size &visibleSize);
	void initCells(int min_width_ratio, int min_height_ratio, cocos2d::Size &visibleSize);
	void initBackgrounds(Json::Value &root, double ratio);
	void initWalls();

	virtual void update(float dt);
	void updateDangoGenerator(float dt);
	void updateWall();
	void updateBullets(float dt);
	void updateDangos(float dt);
	bool isLevelFinished();
	void updateTowers(float dt);
	void removeDeletedElements();

	bool isPaused();
	std::vector<Tower*>& getTowers();
	std::vector<Wall*>& getWalls();
	std::vector<std::vector<Cell*>>& getPaths();
	Quantity getQuantity();
	Quantity getUsedQuantity();
	Quantity getLife();
	int getTotalExperience();
	unsigned int getLevelId();
	unsigned int getWorldId();
	float getProgress();
	void increaseQuantity(Quantity add);
	bool decreaseQuantity(Quantity removed);
	virtual void pause();
	virtual void resume();
	void addTurret(Tower* turret);
	Cell* getNearestCell(cocos2d::Vec2 position);
	cocos2d::Vec2 getNearestPositionInGrid(cocos2d::Vec2 position);
	Tower* touchingTower(cocos2d::Vec2 position);
	Dango* touchingDango(cocos2d::Vec2 position);

	void incrementXPTower(std::string name, int amount);
	int getTowerXP(std::string name);
	int getHolySugar();


	std::vector<Dango*> getEnemiesInRange(cocos2d::Vec2 position, double range);
	std::vector<Dango*> getEnemies();
	Dango* getLastEnemy();
	std::vector<Cell*> getPath(int path);
	void addDango(Dango* dango);
	void addAttack(Attack* bullet);
	bool hasLost();
	void reset();
	void resetCells();
	void removeLevelChildren();
	std::vector<Attack*> getAttacks();
	void setGameSpeed(float game_speed);

	void showGrid(bool show);

protected:
	unsigned int id;
	unsigned int id_world;
	cocos2d::Size size;

	std::vector<Dango*> dangos;
	std::vector<Tower*> towers;
	std::vector<Attack*> attacks;
	std::vector<Wall*> walls;
	std::vector<cocos2d::Sprite*> objects;
	std::map<std::string, int> tower_xp;

	DangoGenerator* generator;

	std::vector<std::vector<Cell*>> cells;
	std::vector<std::vector<Cell*>> paths;

	bool paused;
	int zGround;
	Quantity sugar;
	Quantity usedSugar;
	Quantity life;
	int experience;
	int holySugar;
	
	void reorder();
	std::vector<Node*> createElementsArray();
};

#endif
