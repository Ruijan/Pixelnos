#include "Dangosimple.h"
#include "../Cell.h"
#include "../../../AppDelegate.h"

USING_NS_CC;

Dangosimple::Dangosimple(std::vector<Cell*> npath, int nlevel) :
Dango(npath, nlevel){
}

Dangosimple::~Dangosimple() {
	//std::cerr << "Dangosimple Destroyed ! confirmed !" << std::endl;
}

Dangosimple* Dangosimple::create(std::vector<Cell*> npath, int nlevel)
{
	Dangosimple* pSprite = new Dangosimple(npath, nlevel);

	/*if (pSprite->initWithFile(getConfig()["level"][nlevel]["image"].asString()))
	{*/
		pSprite->initFromConfig();
		//pSprite->setScale(Cell::getCellWidth() / pSprite->getContentSize().width);
		pSprite->autorelease();

		//pSprite->setAnchorPoint(Point(0.5,0.25));
		//pSprite->updateAnimation();

		return pSprite;
	//}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Json::Value Dangosimple::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::DANGO)["dangosimple"];
}
Json::Value Dangosimple::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::DANGO)["dangosimple"];
}
