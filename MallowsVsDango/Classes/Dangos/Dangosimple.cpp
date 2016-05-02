#include "Dangosimple.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"

USING_NS_CC;

Dangosimple::Dangosimple(std::vector<Cell*> npath, int nlevel) :
Dango(npath, Dangosimple::getConfig()["level"][nlevel]["speed"].asDouble(),
	Dangosimple::getConfig()["level"][nlevel]["hitpoints"].asDouble(),nlevel,
	Dangosimple::getConfig()["level"][nlevel]["attack"].asDouble(),
	Dangosimple::getConfig()["level"][nlevel]["reload"].asDouble(),
	Dangosimple::getConfig()["level"][nlevel]["animation_duration"].asDouble(),
	Dangosimple::getConfig()["level"][nlevel]["nb_images_animation"].asInt(),
	Dangosimple::getConfig()["level"][nlevel]["name"].asString()){
}

Dangosimple::~Dangosimple() {
	//std::cerr << "Dangosimple Destroyed ! confirmed !" << std::endl;
}

Dangosimple* Dangosimple::create(std::vector<Cell*> npath, int nlevel)
{
	Dangosimple* pSprite = new Dangosimple(npath, nlevel);

	if (pSprite->initWithFile(getConfig()["level"][nlevel]["image"].asString()))
	{
		pSprite->setScale(Cell::getCellWidth() / pSprite->getContentSize().width);
		pSprite->autorelease();

		pSprite->setAnchorPoint(Point(0.5,0.25));
		pSprite->updateAnimation();

		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Dangosimple::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"]["dangosimple"];
}
Json::Value Dangosimple::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"]["dangosimple"];
}
