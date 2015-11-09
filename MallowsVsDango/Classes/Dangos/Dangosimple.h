#ifndef DANGOSIMPLE_HPP
#define DANGOSIMPLE_HPP

#include "cocos2d.h"
#include "../Config/json.h"
#include "Dango.h"

class Cell;

class Dangosimple : public Dango{

public:
	Dangosimple(std::vector<Cell*> npath, int nlevel);
	virtual ~Dangosimple();
	static Dangosimple* create(std::vector<Cell*> npath, int nlevel);

	static Json::Value getConfig();
	Json::Value getSpecConfig();
};

#endif
