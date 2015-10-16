#include "Level.h"
#include "../AppDelegate.h"
#include "Config/json.h"
#include <fstream>

USING_NS_CC;

Level* Level::create(cocos2d::Size nsize){
	Level* level = new Level(nsize);
	if (level->init()){
		level->autorelease();
		return level;
	}
	
	CC_SAFE_DELETE(level);
	return NULL;
}

Level::Level(Size nsize) : size(nsize), sugar(60), life(3), paused(false), loaded(false) {}

bool Level::init()
{
	if (!Layer::init())
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::vector<std::vector<std::string>> table_map = readMapFromCSV("Resources/res/map/level1_map.csv");
	std::vector<std::vector<std::string>> table_path = readMapFromCSV("Resources/res/map/level1_path.csv");
	
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("res/tiles/ground.plist", "res/tiles/ground.png");
	
	size.width = table_map[0].size();
	size.height = table_map.size();
	//Load images into cache
	//Director::getInstance()->getTextureCache()->addImage("res/tiles/tile.png","tile.png");
	srand(time(NULL));
	
	std::ifstream ifs;
	std::string filename = "res/tiles/ground.json";
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful(false);
	if(CC_TARGET_PLATFORM == CC_PLATFORM_LINUX){
		filename = "Resources/" + filename;
	}
	if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID){
		std::string document = "{\"towers\":{\"archer\":{\"name\":\"archer\",\"description\":\"Archer:Thismallowisalongdistanceattacker.\",\"attack_speed\":1.5,\"damages\":1,\"range\":120,\"cost\":30,\"image\":\"res/turret/tower.png\",\"image_menu\":\"res/turret/tower_menu.png\",\"image_menu_disable\":\"res/turret/tower_menu_disable.png\",\"animation\":\"res/turret/animations/archer.png\",\"animation_size\":15,\"animation_steady_size\":3},\"grill\":{\"name\":\"grill\",\"description\":\"FlameThrower:Selectatargetandburnitovertime.\",\"attack_speed\":2.5,\"damages\":1,\"range\":80,\"cost\":50,\"image\":\"res/turret/grill.png\",\"image_menu\":\"res/turret/grill_menu.png\",\"image_menu_disable\":\"res/turret/grill_menu_disable.png\",\"animation\":\"res/turret/animations/grill.png\",\"animation_size\":1,\"animation_steady_size\":4}},\"dangos\":{\"dango\":{\"speed\":100,\"hitpoints\":2}},\"levels\":{\"level1\":{\"generator\":\"res/level1_generator.json\",\"sugar\":40}}}";
		parsingSuccessful = reader.parse(document, root, false);
	}
	else{
		ifs.open(filename, std::ifstream::binary);
		parsingSuccessful = reader.parse(ifs, root, false);
	}
	if (!parsingSuccessful)
	{
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
	}
	
	
	for (int i(0); i < size.width ; ++i){
		std::vector<Cell*> row;
		for (int j(0); j < size.height; ++j){
			std::string::size_type sz;
			int i_dec = std::stoi (table_map[j][i],&sz,10);
			std::string filename = root["frames"][i_dec]["filename"].asString();
			//std::string filename = "grass8.png";
			
			Cell* cell = Cell::create(filename);
			if (table_path[j][i] == "s"){
				start = Point(j,i);
			}
			else if (table_path[j][i] == "e"){
				end = Point(j,i);
			}
			row.push_back(cell);
			cell->setPosition(Vec2((i + 0.5) * Cell::getCellWidth() , (size.height - j - 1 + 0.5) * Cell::getCellHeight())); 
			/*if(i == 4)
			{
				cell->setPosition(Vec2( 0.5 * Cell::getCellHeight() , (size.height - j - 1 + 0.5) * Cell::getCellHeight())); 
			}*/
			
			addChild(cell);
		}
		cells.push_back(row);
	}
	createPath(table_path);

	double factor = visibleSize.height / (size.height * Cell::getCellHeight());
	setAnchorPoint(Vec2(0, 0));
	setPosition(Vec2(0, 0));
	
	generator = DangoGenerator::createWithFilename(((AppDelegate*)Application::getInstance())->getConfig()["levels"]["level1"]["generator"].asString());
	sugar = ((AppDelegate*)Application::getInstance())->getConfig()["levels"]["level1"]["sugar"].asDouble();
	loaded = true;
	return true;
}

Level::~Level()
{
	removeAllChildren();
}
void Level::update(float dt)
{
	if (!paused){
		// Reorder dangos and towers in Z plan
		reorder();
		
		// update generator
		generator->update(dt, this);
		if (!generator->isDone() && generator->isWaveDone() && dangos.empty()){
			generator->nextWave();
		}

		// update dangos
		int i(0);
		for (auto& dango : dangos){
			++i;
			dango->update(dt);
			bool del = false;
			if (dango->isDone()){
				del = true;
				life -= 1;
				if (life < 0){
					life = 0;
				}
			}
			if (!dango->isAlive()){
				del = true;
				sugar += dango->getGain();
			}
			if (del){
				for(auto& tower : turrets){
					if(tower->getTarget() == dango){
						tower->setTarget(nullptr);
					}
				}
				for(auto& bullet : bullets){
					if(bullet->getTarget() == dango){
						bullet->setTarget(nullptr);
					}
				}
				removeChild(dango);
				dango = nullptr;
				std::cerr << "Dango Destroyed !" << dangos.size()<< std::endl;
			}
		}
		

		// update towers
		for (auto& tower : turrets){
			if (!tower->hasToBeDestroyed()){
				if (tower->isFixed()){
					tower->chooseTarget(dangos);
					tower->update(dt);
				}
			}
			else{
				for (unsigned int i(0); i < cells.size(); ++i){
					for (unsigned int j(0); j < cells[i].size(); j++){
						if (cells[i][j]->getObject() == tower){
							cells[i][j]->setObject(nullptr);
						}
					}
				}
				removeChild(tower);
				std::cerr << "Tower Destroyed !" << std::endl;
				tower = nullptr;
			}
		}
		
		
		for (auto& bullet: bullets){
			bullet->update(dt);
			if(bullet->hasTouched()){
				removeChild(bullet);
				bullet = nullptr;
				std::cerr << "Bullet Destroyed !" << std::endl;
			}
		}
		bullets.erase(std::remove(bullets.begin(), bullets.end(), nullptr), bullets.end());
		turrets.erase(std::remove(turrets.begin(), turrets.end(), nullptr), turrets.end());
		dangos.erase(std::remove(dangos.begin(), dangos.end(), nullptr), dangos.end());
	}
}


void Level::createPath(std::vector<std::vector<std::string>> table){
	bool end_reached = false;
	Point ccell = start;
	path.push_back(cells[ccell.y][ccell.x]);
	while (!end_reached){
		for (int i(-1); i < 2; ++i){
			for (int j(-1); j < 2; ++j){
				if (!((i == -1 && j == -1) || (i == -1 && j == 1) || (i == 1 && j == 1) || (i == 1 && j == -1) || (i == 0 && j == 0))){
					if (ccell.x + i > 0 && ccell.x + i < size.height && ccell.y + j > 0 && ccell.y + j < size.width){
						if (table[ccell.x + i][ccell.y + j] == "0" && (path.size() < 3 || 
							(cells[ccell.y + j][ccell.x + i] != path[path.size() - 2] && 
							cells[ccell.y + j][ccell.x + i] != path[path.size() - 3]))){
							
							path.push_back(cells[ccell.y + j][ccell.x + i]);
							cells[ccell.y + j][ccell.x + i]->setPath(true);
							ccell = Point(ccell.x + i, ccell.y + j);
						}
						if (table[ccell.x + i][ccell.y + j] == "e"){
							end_reached = true;
							path.push_back(cells[ccell.y + j][ccell.x + i]);
							cells[ccell.y + j][ccell.x + i]->setPath(true);
						}
					}
				}
			}
		}
	}
	/*for (unsigned int i(0); i < path.size(); ++i){
		path[i]->setVisible(false);
	}*/
}

void Level::setSize(cocos2d::Size nsize){
	size = nsize;
}
bool Level::isLoaded(){ 
	return loaded; 
}

Quantity Level::getQuantity(){
	return sugar;
}

Quantity Level::getLife(){
	return life;
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

std::vector<Cell*> Level::getPath(){
	return path;
}

void Level::addDango(Dango* dango){
	dangos.push_back(dango);
	addChild(dango);
}

void Level::addBullet(Bullet* bullet){
	bullets.push_back(bullet);
	addChild(bullet);
}

bool Level::hasLost(){
	return (life == 0);
}

bool sortZOrder(Sprite* sprite1, Sprite* sprite2){
	if(sprite1->getPosition().y > sprite2->getPosition().y){
		return true;
	}
	else if(sprite1->getPosition().y == sprite2->getPosition().y){
		return !sprite1->getPosition().x < sprite2->getPosition().x;
	}
	else{
		return false;
	}
}

void Level::reorder(){
	std::vector<Sprite*> elements;
	for(auto& tower : turrets){
		elements.push_back(tower);
	}
	for(auto& dango : dangos){
		elements.push_back(dango);
	}
	std::sort (elements.begin(), elements.end(), sortZOrder);
	int i = 2;
	for(auto& element : elements){
		element->setLocalZOrder(i);
		++i;
	}
	for(auto& bullet : bullets){
		bullet->setLocalZOrder(i);
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
	sugar = ((AppDelegate*)Application::getInstance())->getConfig()["levels"]["level1"]["sugar"].asDouble();
	life = 3;
	loaded = true;
	paused = false;
	generator->reset();
	for (auto& tcell : cells){
		for (auto& cell : tcell){
			cell->setObject(nullptr);
		}
	}
}

bool Level::hasWon(){
	return (generator->isDone() && dangos.empty());
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

std::vector<std::string> readPathFromCSV(std::string filename){
	std::ifstream myfile(filename);
	std::vector<std::string> table_map;
	std::string line;
	if(myfile.is_open()){
		while ( getline (myfile,line) ){
			table_map.push_back(line);
		}
		myfile.close();
	}
	else std::cout << "Unable to open file";
  return table_map;
	
}


std::vector<std::vector<std::string>> readMapFromCSV(std::string filename){
	std::ifstream myfile(filename);
	std::vector<std::vector<std::string>> table_map;
	std::string line;
	if(myfile.is_open()){
		while ( getline (myfile,line) ){
			std::vector<std::string> elems;
			split(line, ',', elems);
			table_map.push_back(elems);
		}
		myfile.close();
	}
	else std::cout << "Unable to open file";
  return table_map;
	
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
