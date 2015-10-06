#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include "json.h"

class Config{
private:
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	std::string filename;

public:
	Config(std::string nfilename);
	Json::Value const getValues() const;
};

#endif