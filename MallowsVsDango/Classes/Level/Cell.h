#ifndef CELL_HPP
#define CELL_HPP

#include "cocos2d.h"

class Cell : public cocos2d::Node
{
public:
	static const int getCellWidth();
	static const int getCellHeight();
	Cell();
	virtual ~Cell();
	static Cell* create();

	void touchEvent(cocos2d::Touch* touch, cocos2d::Vec2 _p);

	const Node* getObject() { return object; }
	void setObject(Node* nObject);
	bool isFree();
	bool isPath();
	void setPath(bool ispath);
	bool isOffLimit();
	void setOffLimit(bool limit);

protected:

private:
	//La case sais qu'elle possede un object (Dango ou tourelle)
	cocos2d::Node* object;
	bool path;
	bool off_limit;
};

#endif
