#include "Wall.h"
#include "AppDelegate.h"

USING_NS_CC;

Wall::Wall(unsigned int n_hp, unsigned int n_max_hp): hp(n_hp), max_hp(n_max_hp){

}
Wall::~Wall() {}

Wall* Wall::create() {
	Json::Value config = ((AppDelegate*)Application::getInstance())->getConfig()["wall"];
	Json::Value save = ((AppDelegate*)Application::getInstance())->getSave()["wall"];
	Wall* wall = new Wall(config["max_hp"].asInt(), config["max_hp"].asInt());
	if (wall->init()) {
		for (unsigned int i(0); i < config["sprites"].size(); ++i) {
			wall->addChild(Sprite::create(config["sprites"][i].asString()));
			wall->getChildren().at(i)->setVisible(false);
		}
		wall->getChildren().at(0)->setVisible(true);
		return wall;
	}
	CC_SAFE_DELETE(wall);
	return NULL;
}

void Wall::takeDamages(unsigned int damages) {
	if (hp > 0){
		hp -= 1;
	}
	updateDisplay();
}

bool Wall::isDestroyed() {
	return hp == 0;
}

void Wall::updateDisplay() {
	// if there are 3 images and there are 5 hps in maximum, it will try to show a different image every 2 hit points.
	int c_image = getChildrenCount() - round((double)hp / (double)max_hp * (getChildrenCount() - 1)) - 1;
	for (ssize_t i(0); i < getChildrenCount(); ++i) {
		if (c_image == i) {
			getChildren().at(i)->setVisible(true);
		}
		else {
			getChildren().at(i)->setVisible(false);
		}
	}
}