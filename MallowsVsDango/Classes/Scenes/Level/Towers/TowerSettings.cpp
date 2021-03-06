#include "TowerSettings.h"

TowerSettings* TowerSettings::create(const std::string & configFilename, const std::string & towerName) {
	TowerSettings* settings = new TowerSettings();
	settings->init(configFilename, towerName);
	return settings;
}

TowerSettings* TowerSettings::create(const Json::Value& configuration, const std::string & towerName) {
	TowerSettings* settings = new TowerSettings();
	settings->init(configuration, towerName);
	return settings;
}

void TowerSettings::init(const std::string & configFilename, const std::string & towerName)
{
	Settings::init(configFilename);
	fillDatabase(towerName);
}

void TowerSettings::init(const Json::Value& configuration, const std::string & towerName)
{
	settingsMap = configuration;
	fillDatabase(towerName);
}

void TowerSettings::fillDatabase(const std::string & towerName)
{
	for (unsigned int i(0); i < settingsMap[towerName]["cost"].size(); ++i) {
		costs.push_back(settingsMap[towerName]["cost"][i].asInt());
		sells.push_back(settingsMap[towerName]["sell"][i].asInt());
		damages.push_back(settingsMap[towerName]["damages"][i].asDouble());
		ranges.push_back(settingsMap[towerName]["range"][i].asDouble());
		attackSpeeds.push_back(settingsMap[towerName]["attack_speed"][i].asDouble());
		xp_levels.push_back(settingsMap[towerName]["xp_level"][i].asDouble());
	}
	maxExistingLevel = settingsMap[towerName]["xp_level"].size();
}

const std::vector<int>& TowerSettings::getSells() {
	return sells;
}
const std::vector<int>& TowerSettings::getCosts() {
	return costs;
}
const std::vector<double>& TowerSettings::getXPLevels() {
	return xp_levels;
}
const std::vector<double>& TowerSettings::getRanges() {
	return ranges;
}
const std::vector<double>& TowerSettings::getDamages() {
	return damages;
}
const std::vector<double>& TowerSettings::getAttackSpeeds() {
	return attackSpeeds;
}

int TowerSettings::getSell(unsigned int level) {
	if (level < maxExistingLevel){
		return sells[level];
	}
	else {
		return 0;
	}
}

int TowerSettings::getCost(unsigned int level) {
	if (level < maxExistingLevel) {
		return costs[level];
	}
	else {
		return 0;
	}
}
double TowerSettings::getXP(unsigned int level) {
	if (level < maxExistingLevel) {
		return xp_levels[level];
	}
	else {
		return 0;
	}
}
double TowerSettings::getRange(unsigned int level) {
	if (level < maxExistingLevel) {
		return ranges[level];
	}
	else {
		return 0;
	}
}
double TowerSettings::getDamage(unsigned int level) {
	if (level < maxExistingLevel) {
		return damages[level];
	}
	else {
		return 0;
	}
}
double TowerSettings::getAttackSpeed(unsigned int level) {
	if (level < maxExistingLevel) {
		return attackSpeeds[level];
	}
	else {
		return 0;
	}
}

unsigned int TowerSettings::getMaxExistingLevel()
{
	return maxExistingLevel;
}
