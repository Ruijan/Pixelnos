#pragma once
#include "Settings.h"
#include "cocos2d.h"
#include <iostream>

class TowerSettings : public Settings {
public:
	static TowerSettings * create(const std::string & configFilename, const std::string & towerName);
	virtual void init(const std::string& configFilename, const std::string & towerName);

	const std::vector<int>& getSells();
	const std::vector<int>& getCosts();
	const std::vector<double>& getXPLevels();
	const std::vector<double>& getRanges();
	const std::vector<double>& getDamages();
	const std::vector<double>& getAttackSpeeds();

	int getSell(unsigned int level);
	int getCost(unsigned int level);
	double getXP(unsigned int level);
	double getRange(unsigned int level);
	double getDamage(unsigned int level);
	double getAttackSpeed(unsigned int level);
	unsigned int getMaxExistingLevel();

protected:
	std::vector<int> sells;
	std::vector<int> costs;
	std::vector<double> damages;
	std::vector<double> attackSpeeds;
	std::vector<double> ranges;
	std::vector<double> xp_levels;

	unsigned int maxExistingLevel;

	std::string name;
};