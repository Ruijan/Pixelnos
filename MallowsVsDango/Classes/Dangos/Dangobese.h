#ifndef DANGOBESE_HPP
#define DANGOBESE_HPP

#include "cocos2d.h"
#include "../Config/json.h"
#include "Dango.h"

class Cell;

class Dangobese : public Dango{

public:
	Dangobese(std::vector<Cell*> npath, int nlevel);
	virtual ~Dangobese();
	static Dangobese* create(std::vector<Cell*> npath, int nlevel);

	static Json::Value getConfig();
	Json::Value getSpecConfig();
};

#endif
