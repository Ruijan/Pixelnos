#include "Level.h"
#include "../AppDelegate.h"
#include "../Config/json.h"
#include "../Lib/Functions.h"
#include <fstream>

USING_NS_CC;

Level* Level::create(int nLevel){
	Level* level = new Level(nLevel);
	if (level->init()){
		level->autorelease();
		return level;
	}
	
	CC_SAFE_DELETE(level);
	return NULL;
}

Level::Level(int nLevel) : id(nLevel), size(14,12), sugar(60), life(3), 
paused(false), state(INTRO), timer(0),zGround(0) {}

bool Level::init()
{
	if (!Layer::init())
		return false;
	
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["levels"][id];
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::vector<std::vector<std::string>> table_map = readMapFromCSV(config["map"].asString());
	std::vector<std::vector<std::string>> table_path = readMapFromCSV(config["path"].asString());
	
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile(config["tileset"].asString(), config["tilesetpng"].asString());
	
	size.width = table_map[0].size();
	size.height = table_map.size();
	
	auto fileUtils = FileUtils::getInstance();
	std::string tilemaptable = fileUtils->getStringFromFile(config["tilemaptable"].asString());

	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful(false);
	parsingSuccessful = reader.parse(tilemaptable, root, false);

	if (!parsingSuccessful)
	{
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
	}
	double factor = visibleSize.height / (size.height * Cell::getCellHeight());
	double ratioX = visibleSize.width / 960;
	
	for (int i(0); i < size.width ; ++i){
		std::vector<Cell*> row;
		for (int j(0); j < size.height; ++j){
			std::string::size_type sz;
			int i_dec = Value(table_map[j][i]).asInt();
			std::string filename = root["frames"][i_dec]["filename"].asString();
			Cell* cell = Cell::create(filename);
			if (table_path[j][i] == "s"){
				start = Point(j,i);
			}
			else if (table_path[j][i] == "e"){
				end = Point(j,i);
			}
			row.push_back(cell);
			cell->setPosition(Vec2((i + 0.5) * Cell::getCellWidth(), (size.height - j - 1 + 0.5) * Cell::getCellHeight()));
			//cell->setScale(Cell::getCellWidth()/cell->getContentSize().width);
			cell->setVisible(false);
			
			addChild(cell);
		}
		cells.push_back(row);
	}
	createPath(table_path);


	setAnchorPoint(Vec2(0, 0));
	setPosition(Vec2(0, 0));
	
	Label* title = Label::createWithTTF("Level " + Value(id+1).asString(), "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 12.0));
	title->setColor(Color3B::YELLOW);
	title->enableOutline(Color4B::BLACK,3);
	title->setPosition(round(visibleSize.width * 3 / 8.0), round(visibleSize.height / 2.0));
	
	Label* start_counter = Label::createWithTTF("3...", "fonts/Love Is Complicated Again.ttf", round(visibleSize.width / 12.0));
	start_counter->setColor(Color3B::YELLOW);
	start_counter->enableOutline(Color4B::BLACK,3);
	start_counter->setPosition(round(visibleSize.width * 3 / 8.0), round(visibleSize.height / 2.0));
	start_counter->setVisible(false);
	
	
	addChild(title,100,"title");
	addChild(start_counter,100,"start_counter");
	
	std::string mapFile = fileUtils->fullPathForFilename("res/map/level1v2.tmx");
	TMXTiledMap* tileMap = TMXTiledMap::create(mapFile);
    tileMap->setScale(ratioX * 0.5);
    Sprite* background = Sprite::create(config["background"].asString());
    background->setScale(ratioX);
    background->setAnchorPoint(Vec2(0,0));
    addChild(background);
    addChild(tileMap);
    
    ValueVector objects = tileMap->getObjectGroup ("Object")->getObjects();
    std::vector<Sprite*> elements;
    for(auto object : objects){

		Sprite* obj = Sprite::createWithSpriteFrameName(object.asValueMap()["type"].asString());
		obj->setPosition(Vec2(object.asValueMap()["x"].asInt() * ratioX * 0.5,
		object.asValueMap()["y"].asInt() * ratioX * 0.5 + Cell::getCellHeight()));
		obj->setAnchorPoint(Vec2(0,0));
		obj->setScale(ratioX* 0.5);
		addChild(obj);
		elements.push_back(obj);
	}
	std::sort (elements.begin(), elements.end(), sortZOrder);
	int i = 1;
	for(auto& element : elements){
		element->setLocalZOrder(i);
		++i;
	}
	zGround = i;

	if(config["introDialogue"].size() != 0){
		introDialogue = Dialogue::createFromConfig(config["introDialogue"]);
		addChild(introDialogue,zGround,"dialogue");
		introDialogue->launch();
		getChildByName("title")->setVisible(false);
	}
	else {
		state = TITLE;
	}

	generator = DangoGenerator::createWithFilename(config["generator"].asString());
	sugar = config["sugar"].asDouble();
	experience = config["exp"].asInt();
	return true;
}

Level::~Level()
{
	log("deletion of level");
	removeAllChildren();
	delete introDialogue;
	log("deleted dialogue");
	delete generator;
	log("deleted generator");
}
void Level::update(float dt)
{
	switch(state){
		case INTRO:
			introDialogue->update();
			if(introDialogue->hasFinished()){
				getChildByName("title")->setVisible(true);
				removeChild(introDialogue);
				state = TITLE;
			}
			break;
		case TITLE:
			timer += dt;
			if(timer > 3){
				getChildByName("title")->setVisible(false);
				state = STARTING;
				timer = 0;
			}
			break;
		case STARTING:
			timer += dt;
			if(timer >= 0 && timer < 1 && !getChildByName("start_counter")->isVisible()){
				getChildByName("start_counter")->setVisible(true);
			}
			else if(timer >= 1 && timer < 2 && ((Label*)getChildByName("start_counter"))->getString() == "3..."){
				((Label*)getChildByName("start_counter"))->setString("2...");
			}
			else if(timer >= 2 && timer < 3 && ((Label*)getChildByName("start_counter"))->getString() == "2..."){
				((Label*)getChildByName("start_counter"))->setString("1");
			}
			else if(timer > 3 && ((Label*)getChildByName("start_counter"))->getString() == "1"){
				getChildByName("start_counter")->setVisible(false);
				state = RUNNING;
			}
			break;
		case RUNNING:
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
						if(generator->isDone() && dangos.size() == 1){
							Size visibleSize = Director::getInstance()->getVisibleSize();
							Node* node = Node::create();
							Sprite* sugar = Sprite::create("res/buttons/life.png");
							Sprite* shining = Sprite::create("res/levels/shining.png");

							sugar->setScale(Cell::getCellWidth()/sugar->getContentSize().width);
							shining->setScale(2*Cell::getCellWidth()/(sugar->getContentSize().width));
							node->addChild(shining);
							node->addChild(sugar);

							Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["levels"][id];
							EaseOut* move1 = EaseOut::create(MoveBy::create(0.25f,Vec2(0, 75)),2);
							EaseIn* move2 = EaseIn::create(MoveBy::create(0.25f,Vec2(0, -75)),2);
							EaseOut* move3 = EaseOut::create(MoveBy::create(0.125f,Vec2(0, 40)),2);
							EaseIn* move4 = EaseIn::create(MoveBy::create(0.125f,Vec2(0, -40)),2);
							EaseOut* move5 = EaseOut::create(MoveBy::create(0.075f,Vec2(0, 20)),2);
							EaseIn* move6 = EaseIn::create(MoveBy::create(0.075f,Vec2(0, -20)),2);

							if(config["reward"] != "none"){
								Menu* menu = Menu::create();
								Sprite* reward = Sprite::create(config["reward"].asString());
								MenuItemSprite* reward_item = MenuItemSprite::create(reward, reward, CC_CALLBACK_1(Level::rewardCallback, this));
								menu->addChild(reward_item);
								menu->setPosition(dango->getPosition());
								menu->setScale(Cell::getCellWidth()/reward->getContentSize().width);
								addChild(menu);
								MoveBy* right1 = MoveBy::create(0.25f,Vec2(30, 0));
								MoveBy* right2 = MoveBy::create(0.125f,Vec2(20, 0));
								MoveBy* right3 = MoveBy::create(0.125f,Vec2(10, 0));
								EaseOut* scale = EaseOut::create(ScaleTo::create(1.0f,1),2);

								menu->runAction(Sequence::create(Spawn::createWithTwoActions(move1->clone(),right1),
										Spawn::createWithTwoActions(move2->clone(),right1),
										Spawn::createWithTwoActions(move3->clone(),right2),
										Spawn::createWithTwoActions(move4->clone(),right2),
										Spawn::createWithTwoActions(move5->clone(),right3),
										Spawn::createWithTwoActions(move6->clone(),right3),
										scale,nullptr));
							}

							node->setPosition(dango->getPosition());
							addChild(node);

							RotateBy* rotation = RotateBy::create(3.0f,360);
							shining->runAction(RepeatForever::create(rotation));

							MoveBy* left = MoveBy::create(0.25f,Vec2(-30, 0));
							MoveBy* left2 = MoveBy::create(0.125f,Vec2(-20, 0));
							MoveBy* left3 = MoveBy::create(0.125f,Vec2(-10, 0));

							EaseIn* move = EaseIn::create(MoveTo::create(2.0,Vec2(visibleSize.width/2,visibleSize.height)),2);
							ScaleTo* scale = ScaleTo::create(2.0f,0.01);

							node->runAction(Sequence::create(Spawn::createWithTwoActions(move1,left),
										Spawn::createWithTwoActions(move2,left),
										Spawn::createWithTwoActions(move3,left2),
										Spawn::createWithTwoActions(move4,left2),
										Spawn::createWithTwoActions(move5,left3),
										Spawn::createWithTwoActions(move6,left3),
										Spawn::createWithTwoActions(move, scale),nullptr));
						}
						removeChild(dango);
						dango = nullptr;
						//std::cerr << "Dango Destroyed !" << dangos.size()<< std::endl;
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
						//std::cerr << "Tower Destroyed !" << std::endl;
						tower = nullptr;
					}
				}
				
				
				for (auto& bullet: bullets){
					bullet->update(dt);
					if(bullet->isDone()){
						removeChild(bullet);
						bullet = nullptr;
						//std::cerr << "Bullet Destroyed !" << std::endl;
					}
				}
				bullets.erase(std::remove(bullets.begin(), bullets.end(), nullptr), bullets.end());
				turrets.erase(std::remove(turrets.begin(), turrets.end(), nullptr), turrets.end());
				dangos.erase(std::remove(dangos.begin(), dangos.end(), nullptr), dangos.end());
				if(generator->isDone() && dangos.empty()){
					//state = ENDING;

				}
			}
			break;
		case ENDING:
			break;
		case DONE:
			break;
	};
	
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
						if (table[ccell.x + i][ccell.y + j] != "-1" && (path.size() < 3 || 
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
bool Level::isFinishing(){
	return state == ENDING;
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

int Level::getLevelId(){
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
	dango->setVisible(true);
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
		return !(sprite1->getPosition().x < sprite2->getPosition().x);
	}
	else{
		return false;
	}
}

void Level::reorder(){
	std::vector<Sprite*> elements;
	for(auto& tower : turrets){
		if (tower != nullptr)
			elements.push_back(tower);
	}
	for(auto& dango : dangos){
		if (dango != nullptr)
			elements.push_back(dango);
	}
	std::sort (elements.begin(), elements.end(), sortZOrder);
	int i = zGround;
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
	for (auto& bullet : bullets){
		removeChild(bullet);
	}
	bullets.clear();
	sugar = ((AppDelegate*)Application::getInstance())->getConfig()["levels"][id]["sugar"].asDouble();
	life = 3;
	paused = false;
	state = TITLE;
	getChildByName("title")->setVisible(true);
	getChildByName("start_counter")->setVisible(false);
	((Label*)getChildByName("start_counter"))->setString("3...");
	timer = 0;
	generator->reset();
	for (auto& tcell : cells){
		for (auto& cell : tcell){
			cell->setObject(nullptr);
		}
	}
}

bool Level::hasWon(){
	return (state == DONE);
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
	auto fileUtils = FileUtils::getInstance();
	std::string pathtable = fileUtils->getStringFromFile(filename);
	std::vector<std::string> table_map;
	split(pathtable,'\n',table_map);
	return table_map;
}


std::vector<std::vector<std::string>> readMapFromCSV(std::string filename){

	auto fileUtils = FileUtils::getInstance();
	std::string tilemaptable = fileUtils->getStringFromFile(filename);
	std::vector<std::vector<std::string>> table_map;
	std::vector<std::string> lines;
	split(tilemaptable,'\n',lines);
	for(int i(0); i < lines.size(); ++i){
		std::vector<std::string> elems;
		split(lines[i], ',', elems);
		table_map.push_back(elems);
	}
	return table_map;
	
}

void Level::rewardCallback(Level* sender){
	state = DONE;
}

Level::State Level::getState(){
	return state;
}
