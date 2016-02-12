#include "Cell.h"
#include <iostream>

USING_NS_CC;

Cell::Cell(): object(nullptr), path(false) {}

Cell::~Cell() {}

Cell* Cell::create(std::string image)
{
	Cell* pSprite = new Cell();

	if (pSprite->initWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(image)))
	{
		//pSprite->setScale(Cell::getCellWidth() / pSprite->getContentSize().width);
		pSprite->autorelease();

		pSprite->initOptions();


		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void Cell::initOptions(){
	// do things here like setTag(), setPosition(), any custom logic.
}

void Cell::touchEvent(cocos2d::Touch* touch, cocos2d::Vec2 _point){
	//CCLOG("touched Cell");
}

void Cell::setObject(Node* nObject){
	object = nObject;
}

bool Cell::isFree(){
	return (object == nullptr) ? true : false;
}
bool Cell::isPath(){
	return path;
}

void Cell::setPath(bool ispath){
	path = ispath;
}

const int Cell::getCellWidth(){
	return cocos2d::Director::getInstance()->getVisibleSize().width * 3 / 4 / 12;
}
const int Cell::getCellHeight(){
	return cocos2d::Director::getInstance()->getVisibleSize().width * 3 / 4 / 12;
}
