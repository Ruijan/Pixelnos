#ifndef SHOP_HPP
#define SHOP_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../Config/json.h"

class Shop : public cocos2d::Scene
{
public:
	virtual bool init();
	CREATE_FUNC(Shop);



private:
	Json::Value config; //load data file
	Json::Value root; //load save file
};
#endif