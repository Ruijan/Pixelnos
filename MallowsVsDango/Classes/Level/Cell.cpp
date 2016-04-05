#include "Cell.h"
#include <iostream>

USING_NS_CC;

Cell::Cell(): Node(), object(nullptr), path(false), off_limit(false){}

Cell::~Cell() {}

Cell* Cell::create(){
	Cell* cell = new Cell();
	if (cell->init()) {
		DrawNode* node = DrawNode::create();
		Vec2 rectangle[4];
		rectangle[0] = Vec2(-Cell::getCellWidth()/2, -Cell::getCellWidth()/2);
		rectangle[1] = Vec2(Cell::getCellWidth()/2, -Cell::getCellWidth() / 2);
		rectangle[2] = Vec2(Cell::getCellWidth() / 2, Cell::getCellWidth() / 2);
		rectangle[3] = Vec2(-Cell::getCellWidth() / 2, Cell::getCellWidth() / 2);

		Color4F white(1, 1, 1, 0.5);
		node->drawPolygon(rectangle, 4, white, 1, white);
		cell->addChild(node, 1, "on");

		DrawNode* node_off = DrawNode::create();
		Color4F red(1, 0, 0, 0.5);
		node_off->drawPolygon(rectangle, 4, red, 1, red);
		cell->addChild(node_off,1,"off");
		node_off->setVisible(false);
		return cell;
	}

	CC_SAFE_DELETE(cell);
	return NULL;
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
	Size visibleSize = Director::getInstance()->getVisibleSize();
	double ratio = visibleSize.width / visibleSize.height;
	double min_width_ratio = 4.0 / 3.0;
	double min_height_ratio = 16.0 / 9.0;
	int nb_cells_width = 12;
	double size_cell = (3.0 / 4.0) * visibleSize.width * sqrt(min_width_ratio / ratio) / nb_cells_width;
	return size_cell;
}

const int Cell::getCellHeight(){
	Size visibleSize = Director::getInstance()->getVisibleSize();
	double ratio = visibleSize.width / visibleSize.height;
	double min_width_ratio = 4.0 / 3.0;
	double min_height_ratio = 16.0 / 9.0;
	int nb_cells_width = 12;
	double size_cell = (3.0 / 4.0) * visibleSize.width * sqrt(min_width_ratio / ratio) / nb_cells_width;
	return size_cell;
}

bool Cell::isOffLimit() {
	return off_limit;
}
void Cell::setOffLimit(bool limit) {
	off_limit = limit;
	if (limit) {
		getChildByName("off")->setVisible(true);
		getChildByName("on")->setVisible(false);
	}
	else {
		getChildByName("off")->setVisible(false);
		getChildByName("on")->setVisible(true);
	}
}