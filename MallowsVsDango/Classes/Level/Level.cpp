#include "Level.h"
#include "../AppDelegate.h"
#include "../Config/json.h"
#include "../Lib/Functions.h"
#include "../SceneManager.h"
#include "../Scenes/Skills.h"
#include "../Dangos/Dango.h"
#include "../Scenes/MyGame.h"
#include "../Towers/TowerFactory.h"
#include "../Config/Exceptions/JsonContentException.h"
#include "Wall.h"
#include <fstream>

USING_NS_CC;

Level* Level::create(unsigned int nLevel, unsigned int nWorld){
	Level* level = new Level(nLevel, nWorld);
	if (level->init()){
		level->autorelease();
		return level;
	}
	
	CC_SAFE_DELETE(level);
	return NULL;
}

Level::Level(unsigned int nLevel, unsigned int nWorld) : id(nLevel), id_world(nWorld), size(14,12), sugar(60), life(3), usedSugar(0),
paused(false), zGround(0), experience(0){}

bool Level::init()
{
	if (!Layer::init())
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();

	Json::Value currentLevelConfig = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::LEVEL)["worlds"][id_world]["levels"][id];
	Json::Value save_file = ((AppDelegate*)Application::getInstance())->getSave();
	Json::Value levelElementsConfig = tryGettingLevelElementsConfig(currentLevelConfig);

	initLevelValues(currentLevelConfig, save_file);
	
	addChild(Layer::create(), 0, "backgrounds");
	addChild(Layer::create(), 0, "objects");
	getChildByName("backgrounds")->setPosition(visibleSize.width * 3 / 8, visibleSize.height / 2);
	getChildByName("objects")->setPosition(visibleSize.width  * 3 / 8, visibleSize.height / 2);

	double ratio1 = sqrt((16.0 / 9.0) / (visibleSize.width / visibleSize.height));
	double ratio2 = sqrt((16.0 / 9.0) / (levelElementsConfig["resolution"]["width"].asDouble() / levelElementsConfig["resolution"]["height"].asDouble()));
	double ratio = (visibleSize.width * ratio1) / (levelElementsConfig["resolution"]["width"].asDouble() * ratio2);
	double min_width_ratio = 4.0 / 3.0;
	double min_height_ratio = 16.0 / 9.0;

	initBackgrounds(levelElementsConfig, ratio);
	initCells(min_width_ratio, min_height_ratio, visibleSize);
	initPaths(levelElementsConfig, ratio, visibleSize);
	initObjects(levelElementsConfig, ratio, visibleSize);
	initLockedCells(levelElementsConfig, ratio, visibleSize);
	initGenerator(levelElementsConfig);
	initWalls();

	return true;
}

void Level::initLevelValues(Json::Value &currentLevelConfig, Json::Value &save_file)
{
	for (unsigned int i(0); i < currentLevelConfig["towers"].size(); ++i) {
		tower_xp[currentLevelConfig["towers"].getMemberNames()[i]] = 0;
	}

	sugar = currentLevelConfig["sugar"].asDouble();
	sugar += Skills::getSavedSkillFromID(3)["bought"].asBool() ? Skills::getSkillFromID(3)["bonus"].asInt() : 0;
	sugar += Skills::getSavedSkillFromID(9)["bought"].asBool() ? Skills::getSkillFromID(9)["bonus"].asInt() : 0;

	experience = currentLevelConfig["exp"].asInt();
	if (save_file["c_level"].asInt() < (int)id + 1) {
		holySugar = currentLevelConfig["holy_sugar"].asInt();
	}
	else {
		holySugar = 0;
	}
}

Json::Value tryGettingLevelElementsConfig(Json::Value& allLevelsConfig) {
	Json::Reader reader;
	Json::Value levelConfig;
	auto fileUtils = FileUtils::getInstance();
	std::string level_config("");
	if (allLevelsConfig["finalized"].asBool()) {
		level_config += "res/";
	}
	else {
		level_config += FileUtils::getInstance()->getWritablePath() + "Levels/";
	}
	level_config = fileUtils->getStringFromFile(level_config + allLevelsConfig["path_level"].asString());
	
	bool parsingConfigSuccessful = reader.parse(level_config, levelConfig, false);
	if (!parsingConfigSuccessful) {
		throw new JsonContentException(reader.getFormattedErrorMessages());
	}
	return levelConfig;
}

void Level::initObjects(Json::Value &root, double ratio, cocos2d::Size &visibleSize)
{
	std::vector<Node*> elements;
	Json::Value objects_json = root["objects"];
	for (auto& object_json : objects_json) {
		std::string name = object_json["image_name"].asString();

		Sprite* object = Sprite::createWithSpriteFrameName(object_json["image_name"].asString());
		object->setRotation(object_json["rotation"].asDouble());
		object->setPosition(object_json["position"][0].asDouble() * ratio,
			object_json["position"][1].asDouble() * ratio);
		object->setScaleX(object_json["scale"][0].asDouble() * ratio);
		object->setScaleY(object_json["scale"][1].asDouble() * ratio);
		object->setLocalZOrder(object_json["zorder"].asInt());
		object->setFlippedX(object_json["flipped"][0].asBool());
		object->setFlippedY(object_json["flipped"][1].asBool());

		if (name.find("path") == std::string::npos) {
			objects.push_back(object);
			object->setPosition(object->getPosition() + Vec2(visibleSize.width * 3 / 8, visibleSize.height / 2));
			addChild(object);
		}
		else {
			getChildByName("backgrounds")->addChild(object);
		}
		elements.push_back(object);
	}
	std::sort(elements.begin(), elements.end(), sortZOrder);
	int i = 1;
	for (auto& element : elements) {
		element->setLocalZOrder(i);
		++i;
	}
	zGround = i;
}

void Level::initLockedCells(Json::Value &root, double ratio, cocos2d::Size &visibleSize)
{
	Json::Value locked_cell_json = root["locked_cell"];
	for (auto& locked_cell : locked_cell_json) {
		Vec2 cell_pos = Vec2(locked_cell["pos"][0].asFloat() * ratio + visibleSize.width * 3 / 8,
			locked_cell["pos"][1].asFloat() * ratio + visibleSize.height / 2);
		Cell* cell = getNearestCell(cell_pos);
		cell->setOffLimit(true);
	}
}

void Level::initGenerator(Json::Value &root)
{
	generator = new DangoGenerator();
	for (int i(0); i < root["nbwaves"].asInt(); ++i) {
		generator->addWave();
		for (unsigned int j(0); j < root["dangosChain"][i].size(); ++j) {
			int enemy_level = Value(root["dangosChain"][i][j].asString().substr(
				root["dangosChain"][i][j].asString().size() - 1,
				root["dangosChain"][i][j].asString().size())).asInt();
			generator->addStep(root["dangosChain"][i][j].asString(), root["dangosTime"][i][j].asDouble(),
				root["dangosPath"][i][j].asDouble(), i);
		}
	}
}

void Level::initPaths(Json::Value &root, double ratio, cocos2d::Size &visibleSize)
{
	for (unsigned int i(0); i < root["paths"].size(); ++i) {
		std::vector<Cell*> path;
		for (unsigned int j(0); j < root["paths"][i]["path"].size(); ++j) {
			Vec2 path_pos = Vec2(root["paths"][i]["path"][j][0].asFloat() * ratio + visibleSize.width * 3 / 8,
				root["paths"][i]["path"][j][1].asFloat() * ratio + visibleSize.height / 2);
			Cell* cell = getNearestCell(path_pos);
			cell->setPath(true);
			path.push_back(cell);
		}
		paths.push_back(path);
	}
}

void Level::initCells(int min_width_ratio, int min_height_ratio, cocos2d::Size &visibleSize)
{
	int nb_cells_width = 12;
	double ratio3 = visibleSize.width / visibleSize.height;
	double size_cell = (3.0 / 4.0) * visibleSize.width * sqrt(min_width_ratio / ratio3) / nb_cells_width;
	int nb_cells_height = visibleSize.height * sqrt(ratio3 / min_height_ratio) / size_cell;
	int nb_cells_maxwidth = 16;
	int nb_cells_maxheight = 12;
	size.width = 12;
	size.height = nb_cells_height;
	for (int i(0); i < nb_cells_maxwidth; ++i) {
		std::vector<Cell*> row;
		for (int j(0); j < nb_cells_maxheight; ++j) {
			Cell* cell = Cell::create();
			row.push_back(cell);
			cell->setPosition(Vec2((i - nb_cells_maxwidth / 2.0 + 0.5) * Cell::getCellWidth() + visibleSize.width * 3 / 8,
				(nb_cells_maxheight - j - 1 + 0.5 - nb_cells_maxheight / 2.0) * Cell::getCellHeight() + visibleSize.height / 2));
			if (!((AppDelegate*)Application::getInstance())->getConfigClass()->getSettings()->isAlwaysGridEnabled()) {
				cell->setVisible(false);
			}

			if (abs(i + 0.5 - nb_cells_maxwidth / 2.0) >= nb_cells_width / 2.0 || abs(j + 0.5 - nb_cells_maxheight / 2.0) >= nb_cells_height / 2.0) {
				cell->setOffLimit(true);
			}
			addChild(cell, 4);
		}
		cells.push_back(row);
	}
}

void Level::initBackgrounds(Json::Value &root, double ratio)
{
	for (unsigned int i(0); i < root["backgrounds"].size(); ++i) {
		Sprite* background = Sprite::create(root["backgrounds"][i]["image_name"].asString());
		background->setRotation(root["backgrounds"][i]["rotation"].asDouble());
		background->setPosition(root["backgrounds"][i]["position"][0].asDouble() * ratio,
			root["backgrounds"][i]["position"][1].asDouble() * ratio);
		background->setScaleX(root["backgrounds"][i]["scale"][0].asDouble() * ratio);
		background->setScaleY(root["backgrounds"][i]["scale"][1].asDouble() * ratio);
		background->setLocalZOrder(root["backgrounds"][i]["zorder"].asInt());
		background->setFlippedX(root["backgrounds"][i]["flipped"][0].asBool());
		background->setFlippedY(root["backgrounds"][i]["flipped"][1].asBool());
		getChildByName("backgrounds")->addChild(background);
	}
}

void Level::incrementXPTower(std::string name, int amount) {
	tower_xp[name] += amount;
	for (auto& tower : towers) {
		if (tower->isSameType(TowerFactory::getTowerTypeFromString(name))) {
			tower->incrementXP(amount);
		}
	}
}

int Level::getTowerXP(std::string name) {
	return tower_xp[name];
}

int Level::getHolySugar() {
	return holySugar;
}

float Level::getProgress() {
	return generator->getProgress();
}

void Level::initWalls() {
	if (Skills::getSavedSkillFromID(1)["bought"].asBool()) {
		for (auto& path : paths) {
			Wall* wall = Wall::create(2);
			path[path.size() - 3]->setObject(wall);
			wall->setPosition(path[path.size() - 3]->getPosition());
			wall->setPosition(wall->getPositionX(), wall->getPositionY() + Cell::getCellHeight() / 4);
			wall->setScale(Cell::getCellWidth() / wall->getChildren().at(0)->getContentSize().width);
			addChild(wall);
			walls.push_back(wall);
		}
	}
}

Level::~Level()
{
	reset();
	log("deletion of level");
	removeAllChildren();
	log("All children deleted");
	delete generator;
	log("deleted generator");
}
void Level::update(float dt)
{
	if (!paused) {
		// Reorder dangos and towers in Z plan
		reorder();

		updateDangoGenerator(dt);
		updateDangos(dt);
		updateTowers(dt);
		updateBullets(dt);
		updateWall();
		removeDeletedElements();
	}	
}

void Level::updateDangoGenerator(float dt)
{
	generator->update(dt, this);
	if (!generator->isDone() && generator->isWaveDone() && dangos.empty()) {
		generator->nextWave();
	}
}

void Level::updateWall()
{
	for (auto& wall : walls) {
		if (wall != nullptr && wall->isDestroyed()) {
			for (auto& path : paths) {
				for (auto& cell : path) {
					if (cell->getObject() == wall) {
						cell->setObject(nullptr);
					}
				}
			}
			removeChild(wall);
			wall = nullptr;
		}
	}
}

void Level::updateBullets(float dt)
{
	for (auto& attack : attacks) {
		attack->update(dt);
		if (attack->isDone()) {
			incrementXPTower(attack->getType() , attack->getRewardedXP());
			removeChild(attack);
			attack = nullptr;
		}
	}
}

void Level::updateDangos(float dt)
{
	int i(0);
	for (auto& dango : dangos) {
		++i;
		dango->update(dt);
		bool del = false;
		if (dango->isDone()) {
			del = true;
			if (life > 0) {
				life -= 1;
			}
		}
		if (!dango->isAlive()) {
			increaseQuantity(dango->getGain());
			del = true;
		}
		if (del) {
			if (dango->getHolySugar() > 0) {
				SceneManager::getInstance()->getGame()->getMenu()->generateHolySugar(dango->getPosition());
				holySugar += dango->getHolySugar();
			}
			if (isLevelFinished()) {
				SceneManager::getInstance()->getGame()->getMenu()->startRewarding(dango->getPosition());
			}
			if (SceneManager::getInstance()->getGame()->getMenu()->getCurrentDango() == dango) {
				SceneManager::getInstance()->getGame()->getMenu()->handleDeadDango();
			}
			removeChild(dango);
			dango = nullptr;
		}
	}
}

bool Level::isLevelFinished()
{
	return generator->isDone() && dangos.size() == 1 && life > 0;
}

void Level::updateTowers(float dt) {
	for (auto& tower : towers) {
		if (!tower->hasToBeDestroyed()) {
			tower->update(dt);
		}
		else {
			for (unsigned int i(0); i < cells.size(); ++i) {
				for (unsigned int j(0); j < cells[i].size(); j++) {
					if (cells[i][j]->getObject() == tower) {
						cells[i][j]->setObject(nullptr);
					}
				}
			}
			removeChild(tower);
			tower = nullptr;
		}
	}
}

void Level::removeDeletedElements() {
	attacks.erase(std::remove(attacks.begin(), attacks.end(), nullptr), attacks.end());
	towers.erase(std::remove(towers.begin(), towers.end(), nullptr), towers.end());
	dangos.erase(std::remove(dangos.begin(), dangos.end(), nullptr), dangos.end());
}

bool Level::isPaused(){
	return paused;
}
int Level::getTotalExperience(){
	return experience;
}

Quantity Level::getQuantity(){
	return sugar;
}

Quantity Level::getUsedQuantity() {
	return usedSugar;
}

Quantity Level::getLife(){
	return life;
}

unsigned int Level::getLevelId(){
	return id;
}

unsigned int Level::getWorldId() {
	return id_world;
}

void Level::increaseQuantity(Quantity add){
	sugar += add;
}
bool Level::decreaseQuantity(Quantity removed){
	if (sugar - removed < 0){
		return false;
	}
	else{
		sugar -= removed;
		usedSugar += removed;
		return true;
	}
}

void Level::pause(){
	Node::pause();
	for(auto child: getChildren()){
		child->pause();
	}
	for (auto tower : towers) {
		tower->pauseAnimation();
	}
	for (auto dango : dangos) {
		dango->pauseAnimation();
	}
	paused = true;
}

void Level::resume(){
	Node::resume();
	for(auto child: getChildren()){
		child->resume();
	}
	for (auto tower : towers) {
		tower->resumeAnimation();
	}
	for (auto dango : dangos) {
		dango->resumeAnimation();
	}
	paused = false;
}

void Level::addTurret(Tower* tower){
	towers.push_back(tower);
	addChild(tower);
	tower->setLocalZOrder(zGround);
}

std::vector<Dango*> Level::getEnemies() {
	return dangos;
}

std::vector<Dango*> Level::getEnemiesInRange(cocos2d::Vec2 position, double range) {
	std::vector<Dango*> enemies_in_range;
	for (auto& dango : dangos) {
		if (dango != nullptr) {
			double distance = dango->getPosition().distance(position);
			if (distance < range) {
				enemies_in_range.push_back(dango);
			}
		}
	}
	return enemies_in_range;
}

Cell* Level::getNearestCell(cocos2d::Vec2 position){
	Cell* nearestCell = nullptr;
	double lowestDistance = -5;
	for(unsigned int i(0); i < cells.size(); ++i){
		for(unsigned int j(0); j < cells[i].size(); j++){
			double dist = position.distanceSquared(cells[i][j]->getPosition());
			if(lowestDistance < 0 || dist < lowestDistance){
				nearestCell = cells[i][j];
				lowestDistance = dist;
			}
		}
	}
	return nearestCell;
}

Vec2 Level::getNearestPositionInGrid(cocos2d::Vec2 position) {
	Vec2 nearestCell = Vec2();
	double lowestDistance = -5;
	for (unsigned int i(0); i < cells.size(); ++i) {
		for (unsigned int j(0); j < cells[i].size(); j++) {
			double dist = position.distanceSquared(cells[i][j]->getPosition());
			if (lowestDistance < 0 || dist < lowestDistance) {
				nearestCell = Vec2(i,j);
				lowestDistance = dist;
			}
		}
	}
	return nearestCell;
}

std::vector<Cell*> Level::getPath(int path){
	if (path < (int)paths.size()) {
		return paths[path];
	}
	else {
		log("Path index is larger than the number of path.");
		return paths[0];
	}
}

void Level::addDango(Dango* dango){
	dangos.push_back(dango);
	dango->setVisible(true);
	addChild(dango);
}

void Level::addAttack(Attack* bullet){
	attacks.push_back(bullet);
	addChild(bullet);
}

bool Level::hasLost(){
	return (life == 0);
}

bool sortZOrder(Node* sprite1, Node* sprite2){
	double position1_height = getMaxYPositionFromSprite(sprite1);
	double position2_height = getMaxYPositionFromSprite(sprite2);
	if (position1_height > position2_height) {
		return true;
	}
	else if (position1_height == position2_height) {
		double position1_width = getMaxXPositionFromSprite(sprite1);
		double position2_width = getMaxXPositionFromSprite(sprite2);
		return !(position1_width <= position2_width);
	}
	else {
		return false;
	}
}

float getMaxXPositionFromSprite(cocos2d::Node * sprite)
{
	return sprite->getPosition().x + sprite->getContentSize().width*
		(1 - sprite->getAnchorPoint().x) * sprite->getScaleX();
}

float getMaxYPositionFromSprite(cocos2d::Node * sprite)
{
	return sprite->getPosition().y - sprite->getContentSize().height*
		(1 - sprite->getAnchorPoint().y) * sprite->getScaleY();
}

void Level::reorder(){
	std::vector<Node*> elements = createElementsArray();
	std::stable_sort (elements.begin(), elements.end(), sortZOrder);
	int i = zGround;
	for(auto& element : elements){
		element->setLocalZOrder(i);
		++i;
	}
}

std::vector<Node*> Level::createElementsArray() {
	std::vector<Node*> elements;
	for (auto& tower : towers) {
		if (tower != nullptr)
			elements.push_back(tower);
	}
	for (auto& dango : dangos) {
		if (dango != nullptr)
			elements.push_back(dango);
	}
	for (auto& wall : walls) {
		if (wall != nullptr) {
			elements.push_back(wall);
		}
	}
	for (auto& object : objects) {
		if (object != nullptr) {
			elements.push_back(object);
		}
	}
	for (auto& attack : attacks) {
		if (attack != nullptr) {
			elements.push_back(attack);
		}
	}
	return elements;
}

void Level::reset(){
	removeLevelChildren();

	sugar = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::LEVEL)["worlds"][id_world]["levels"][id]["sugar"].asDouble();
	usedSugar = 0;
	life = 3;
	paused = false;
	generator->reset();
	resetCells();
	initWalls();
}

void Level::resetCells()
{
	for (auto& tcell : cells) {
		for (auto& cell : tcell) {
			cell->setObject(nullptr);
		}
	}
}

void Level::removeLevelChildren()
{
	for (auto& dango : dangos) {
		removeChild(dango);
	}
	dangos.clear();
	for (auto& tower : towers) {
		removeChild(tower);
	}
	towers.clear();
	for (auto& attack : attacks) {
		removeChild(attack);
	}
	attacks.clear();
	for (auto& wall : walls) {
		if (wall != nullptr) {
			removeChild(wall);
		}
	}
	walls.clear();
}

Tower* Level::touchingTower(cocos2d::Vec2 position){
	for (auto& tower : towers){
		Vec2 pointInSprite = position - tower->getPosition() * getScale();
		double scale1 = tower->getScale();
		double scale2 = getScale();
		pointInSprite.x += Cell::getCellWidth() / 2;
		pointInSprite.y += Cell::getCellWidth() / 2;
		Rect itemRect = Rect(0, 0, Cell::getCellWidth(),
			Cell::getCellHeight());

		if (itemRect.containsPoint(pointInSprite)){
			return tower;
		}
	}
	return nullptr;
}

Dango* Level::touchingDango(cocos2d::Vec2 position) {
	for (auto& dango : dangos) {
		Vec2 pointInSprite = position - dango->getPosition() * getScale();
		pointInSprite.x += Cell::getCellWidth() / 2;
		pointInSprite.y += Cell::getCellHeight() / 2;
		Rect itemRect = Rect(0, 0, Cell::getCellWidth(),
			Cell::getCellHeight());

		if (itemRect.containsPoint(pointInSprite)) {
			return dango;
		}
	}
	return nullptr;
}

std::vector<Attack*> Level::getAttacks() {
	return attacks;
}

void Level::showGrid(bool show) {
	for (auto& row : cells) {
		for (auto& cell : row) {
			cell->setVisible(show);
		}
	}
}

std::vector<Tower*>& Level::getTowers() {
	return towers;
}

std::vector<std::vector<Cell*>>& Level::getPaths() {
	return paths;
}

std::vector<Wall*>& Level::getWalls() {
	return walls;
}

Dango* Level::getLastEnemy() {
	if (dangos.size() != 0) {
		return dangos.back();
	}
	else {
		return nullptr;
	}
}

void Level::setGameSpeed(float game_speed) {
	for (auto& dango : dangos) {
		dango->changeSpeedAnimation(game_speed);
	}
	for (auto& tower : towers) {
		tower->changeSpeedAnimation(game_speed);
	}
}