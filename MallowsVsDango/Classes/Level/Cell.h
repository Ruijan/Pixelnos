#ifndef CELL_HPP
#define CELL_HPP

#include "cocos2d.h"

class Cell : public cocos2d::Sprite
{
public:
	static const int getCellWidth();
	static const int getCellHeight();
	Cell();
	virtual ~Cell();
	static Cell* create(std::string image);
	void initOptions();

	void addEvents();
	void touchEvent(cocos2d::Touch* touch, cocos2d::Vec2 _p);

	Node* getObject() { return object; }
	void setObject(Node* nObject);
	bool isFree();
	bool isPath();
	void setPath(bool ispath);
protected:

private:
	//La case sais qu'elle possède un object (Dango ou tourelle)
	cocos2d::Node* object;
	bool path;
	
	
};

#endif
