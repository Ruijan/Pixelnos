#include "Level.h"
#include "../AppDelegate.h"
#include "../Config/json.h"
#include "../Lib/Functions.h"
#include "Wall.h"
#include <fstream>

USING_NS_CC;

Level* Level::create(unsigned int nLevel){
	Level* level = new Level(nLevel);
	if (level->init()){
		level->autorelease();
		return level;
	}
	
	CC_SAFE_DELETE(level);
	return NULL;
}

Level::Level(unsigned int nLevel) : id(nLevel), size(14,12), sugar(60), life(3),

paused(false), zGround(0), experience(0){}

bool Level::init()
{
	if (!Layer::init())
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();

	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["levels"][id];
	
	std::vector<Node*> elements;
	sugar = config["sugar"].asDouble();
	experience = config["exp"].asInt();
	generator = new DangoGenerator();
	auto fileUtils = FileUtils::getInstance();

	std::string level_config = fileUtils->getStringFromFile(config["path_level"].asString());
	Json::Reader reader;
	Json::Value root;
	bool parsingConfigSuccessful = reader.parse(level_config, root, false);
	if (!parsingConfigSuccessful) {
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
		log(error.c_str());
	}
	else {
		addChild(Layer::create(), 0, "backgrounds");
		addChild(Layer::create(), 0, "objects");
		getChildByName("backgrounds")->setPosition(visibleSize.width * 3 / 8, visibleSize.height / 2);
		getChildByName("objects")->setPosition(visibleSize.width  * 3 / 8, visibleSize.height / 2);

		double ratio1 = sqrt((16.0 / 9.0) / (visibleSize.width / visibleSize.height));
		double ratio2 = sqrt((16.0 / 9.0) / (root["resolution"]["width"].asDouble() / root["resolution"]["height"].asDouble()));
		double ratio = (visibleSize.width * ratio1) / (root["resolution"]["width"].asDouble() * ratio2);
		double min_width_ratio = 4.0 / 3.0;
		double min_height_ratio = 16.0 / 9.0;

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
		for (unsigned int i(0); i < root["objects"].size(); ++i) {
			std::string name = root["objects"][i]["image_name"].asString();
			Sprite* object = Sprite::createWithSpriteFrameName(root["objects"][i]["image_name"].asString());
			object->setRotation(root["objects"][i]["rotation"].asDouble());
			object->setPosition(root["objects"][i]["position"][0].asDouble() * ratio,
				root["objects"][i]["position"][1].asDouble() * ratio);
			object->setScaleX(root["objects"][i]["scale"][0].asDouble() * ratio);
			object->setScaleY(root["objects"][i]["scale"][1].asDouble() * ratio);
			object->setLocalZOrder(root["objects"][i]["zorder"].asInt());
			object->setFlippedX(root["objects"][i]["flipped"][0].asBool());
			object->setFlippedY(root["objects"][i]["flipped"][1].asBool());
			getChildByName("objects")->addChild(object);
			elements.push_back(object);
		}
		int nb_cells_width = 12;
		double ratio3 = visibleSize.width / visibleSize.height;
		double size_cell = (3.0 / 4.0) * visibleSize.width * sqrt(min_width_ratio / ratio3) / nb_cells_width;
		int nb_cells_height = visibleSize.height * sqrt(ratio3 / min_height_ratio) / size_cell;
		int nb_cells_maxwidth = 16;
		int nb_cells_maxheight = 12;
		size.width = 12;
		size.height = nb_cells_height;
		double offset_x = (12 % 2) * size_cell / 2.0;
		double offset_y = (nb_cells_height % 2) * size_cell / 2.0;
		
		for (int i(0); i < nb_cells_maxwidth; ++i) {
			std::vector<Cell*> row;
			for (int j(0); j < nb_cells_maxheight; ++j) {
				Cell* cell = Cell::create();
				row.push_back(cell);
				cell->setPosition(Vec2((i - nb_cells_maxwidth / 2.0 + 0.5) * Cell::getCellWidth() + visibleSize.width * 3 / 8,
					(nb_cells_maxheight - j - 1 + 0.5 - nb_cells_maxheight / 2.0) * Cell::getCellHeight() + visibleSize.height / 2));
				if (!((AppDelegate*)Application::getInstance())->getConfigClass()->isAlwaysGridEnabled()) {
					cell->setVisible(false);
				}
				
				if (abs(i + 0.5 - nb_cells_maxwidth / 2.0) >= nb_cells_width / 2.0 || abs(j + 0.5 - nb_cells_maxheight / 2.0) >= nb_cells_height / 2.0) {
					cell->setOffLimit(true);
				}
				addChild(cell,4);
			}
			cells.push_back(row);
		}
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
		for (int i(0); i < root["nbwaves"].asInt(); ++i) {
			generator->addWave();
			for (unsigned int j(0); j < root["dangosChain"][i].size(); ++j) {
				int enemy_level = Value(root["dangosChain"][i][j].asString().substr(
					root["dangosChain"][i][j].asString().size() - 1,
					root["dangosChain"][i][j].asString().size())).asInt();
				std::string enemy_name = root["dangosChain"][i][j].asString().substr(0, root["dangosChain"][i][j].asString().size() - 1);
				double enemy_timer = root["dangosTime"][i][j].asDouble();
				int current_wave = i;
				generator->addStep(root["dangosChain"][i][j].asString(), root["dangosTime"][i][j].asDouble(),
					root["dangosPath"][i][j].asDouble(), i);
			}
		}
	}
	initWalls();

	std::sort(elements.begin(), elements.end(), sortZOrder);
	int i = 1;
	for (auto& element : elements) {
		element->setLocalZOrder(i);
		++i;
	}
	zGround = i;

	return true;
}

void Level::initWalls() {
	for (auto& path : paths) {
		Wall* wall = Wall::create();
		path[path.size() - 2]->setObject(wall);
		wall->setPosition(path[path.size() - 2]->getPosition());
		wall->setPosition(wall->getPositionX(), wall->getPositionY() + Cell::getCellHeight() / 4);
		wall->setScale(Cell::getCellWidth() / wall->getChildren().at(0)->getContentSize().width);
		addChild(wall);
		walls.push_back(wall);
	}
}

Level::~Level()
{
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

		// update generator
		generator->update(dt, this);
		if (!generator->isDone() && generator->isWaveDone() && dangos.empty()) {
			generator->nextWave();
		}

		// update dangos
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
				del = true;
				sugar += dango->getGain();
			}
			if (del) {
				/*for (auto& tower : turrets) {
					if (tower->getTarget() == dango) {
						tower->setTarget(nullptr);
					}
				}
				for (auto& attack : attacks) {
					if (attack->getTarget() == dango) {
						attack->setTarget(nullptr);
					}
				}*/
				if (generator->isDone() && dangos.size() == 1) {
					SceneManager::getInstance()->getGame()->getMenu()->startRewarding(dango->getPosition());
				}
				if (SceneManager::getInstance()->getGame()->getMenu()->getCurrentDango() == dango) {
					SceneManager::getInstance()->getGame()->getMenu()->handleDeadDango();
				}
				removeChild(dango);
				dango = nullptr;
			}
		}

		// update towers
		updateTowers(dt);

		// update bullets
		for (auto& attack : attacks) {
			attack->update(dt);
			if (attack->isDone()) {
				removeChild(attack);
				attack = nullptr;
			}
		}

		//update wall
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

		removeElements();
	}	
}

void Level::updateTowers(float dt) {
	for (auto& tower : turrets) {
		if (!tower->hasToBeDestroyed()) {
			if (tower->isFixed()) {
				tower->chooseTarget(dangos);
				tower->update(dt);
			}
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

void Level::removeElements() {
	attacks.erase(std::remove(attacks.begin(), attacks.end(), nullptr), attacks.end());
	turrets.erase(std::remove(turrets.begin(), turrets.end(), nullptr), turrets.end());
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

Quantity Level::getLife(){
	return life;
}

unsigned int Level::getLevelId(){
	return id;
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
		return true;
	}
}

void Level::pause(){
	Node::pause();
	for(auto child: getChildren()){
		child->pause();
	}
	paused = true;
}

void Level::resume(){
	Node::resume();
	for(auto child: getChildren()){
		child->resume();
	}
	paused = false;
}
void Level::addTurret(Tower* tower){
	turrets.push_back(tower);
	addChild(tower);
	tower->setLocalZOrder(zGround);
}

std::vector<Dango*> Level::getEnemies() {
	return dangos;
}

std::vector<Dango*> Level::getEnemiesInRange(cocos2d::Vec2 position, double range) {
	std::vector<Dango*> enemies_in_range;
	for (auto& dango : dangos) {
		double distance = dango->getPosition().distance(position);
		if (distance < range) {
			enemies_in_range.push_back(dango);
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
	double position1_height = sprite1->getPosition().y - sprite1->getContentSize().height*
		(1 - sprite1->getAnchorPoint().y) * sprite1->getScaleY();
	double position2_height = sprite2->getPosition().y - sprite2->getContentSize().height*
		(1 - sprite2->getAnchorPoint().y) * sprite2->getScaleY();
	if (position1_height > position2_height) {
		return true;
	}
	else if (position1_height == position2_height) {
		double position1_width = sprite1->getPosition().x + sprite1->getContentSize().width*
			(1 - sprite1->getAnchorPoint().x) * sprite1->getScaleX();
		double position2_width = sprite2->getPosition().x + sprite2->getContentSize().width*
			(1 - sprite2->getAnchorPoint().x) * sprite2->getScaleX();
		return !(position1_width <= position2_width);
	}
	else {
		return false;
	}
}

void Level::reorder(){
	std::vector<Node*> elements;
	for(auto& tower : turrets){
		if (tower != nullptr)
			elements.push_back(tower);
	}
	for(auto& dango : dangos){
		if (dango != nullptr)
			elements.push_back(dango);
	}
	for (auto& wall : walls) {
		if (wall != nullptr) {
			elements.push_back(wall);
		}
	}
	std::stable_sort (elements.begin(), elements.end(), sortZOrder);
	int i = zGround;
	for(auto& element : elements){
		element->setLocalZOrder(i);
		++i;
	}
	for(auto& attack : attacks){
		attack->setLocalZOrder(i);
		++i;
	}
}

void Level::reset(){
	for (auto& dango : dangos){
		removeChild(dango);
	}
	dangos.clear();
	for (auto& tower : turrets){
		removeChild(tower);
	}
	turrets.clear();
	for (auto& attack : attacks){
		removeChild(attack);
	}
	attacks.clear();

	sugar = ((AppDelegate*)Application::getInstance())->getConfig()["levels"][id]["sugar"].asDouble();
	life = 3;
	paused = false;
	generator->reset();
	for (auto& tcell : cells){
		for (auto& cell : tcell){
			cell->setObject(nullptr);
		}
	}
	for (auto& wall : walls) {
		if (wall != nullptr) {
			removeChild(wall);
		}
	}
	initWalls();
}

Tower* Level::touchingTower(cocos2d::Vec2 position){
	for (auto& tower : turrets){
		Vec2 pointInSprite = position - tower->getPosition() * getScale();
		double scale1 = tower->getScale();
		double scale2 = getScale();
		pointInSprite.x += tower->getSpriteFrame()->getRect().size.width * tower->getScale() / 2;
		pointInSprite.y += tower->getSpriteFrame()->getRect().size.height * tower->getScale() / 2;
		Rect itemRect = Rect(0, 0, tower->getSpriteFrame()->getRect().size.width * tower->getScale(),
			tower->getSpriteFrame()->getRect().size.height * tower->getScale());

		if (itemRect.containsPoint(pointInSprite)){
			return tower;
		}
	}
	return nullptr;
}

Dango* Level::touchingDango(cocos2d::Vec2 position) {
	for (auto& dango : dangos) {
		Vec2 pointInSprite = position - dango->getPosition() * getScale();
		pointInSprite.x += dango->getSpriteFrame()->getRect().size.width * dango->getScaleX() / 2;
		pointInSprite.y += dango->getSpriteFrame()->getRect().size.height * dango->getScaleY() / 2;
		Rect itemRect = Rect(0, 0, dango->getSpriteFrame()->getRect().size.width * dango->getScaleX(),
			dango->getSpriteFrame()->getRect().size.height * dango->getScaleY());

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