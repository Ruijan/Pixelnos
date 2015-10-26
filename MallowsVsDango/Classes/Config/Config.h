#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include "json.h"

class Config{
private:
	Json::Value root;   // will contains the root value after parsing.
	std::string config_filename;
	Json::Value rootSav;   // will contains the root value after parsing.
	std::string save_filename;
	bool saveFile;

public:
	Config(std::string configfilename, std::string savename);
	Json::Value const getConfigValues() const;
	bool isSaveFile() const;
	Json::Value const getSaveValues() const;
	void setSave(Json::Value nroot);
	void save();
};

#endif
