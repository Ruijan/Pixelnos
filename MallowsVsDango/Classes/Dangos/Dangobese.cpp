#include "Dangobese.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"

USING_NS_CC;

Dangobese::Dangobese(std::vector<Cell*> npath, int nlevel) :
Dango(npath, Dangobese::getConfig()["level"][nlevel]["speed"].asDouble(),
		Dangobese::getConfig()["level"][nlevel]["hitpoints"].asDouble(),nlevel){
}

Dangobese::~Dangobese() {
	std::cerr << "Dangobese Destroyed ! confirmed !" << std::endl;
}

Dangobese* Dangobese::create(std::vector<Cell*> npath, int nlevel)
{
	Dangobese* pSprite = new Dangobese(npath, nlevel);

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

Json::Value Dangobese::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"]["dangobese"];
}
Json::Value Dangobese::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfig()["dangos"]["dangobese"];
}
