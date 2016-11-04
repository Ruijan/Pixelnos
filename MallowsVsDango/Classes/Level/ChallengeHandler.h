#ifndef CHALLENGE_HANDLER_HPP
#define CHALLENGE_HANDLER_HPP

#include "cocos2d.h"
#include "../Config/json.h"
#include "../Towers/Tower.h"

class Challenge;

class ChallengeHandler: public cocos2d::Node {
private:
	std::vector<Challenge*> challenges;

public:
	/**
	* @brief Create a ChallengeHandler from the configuration file and the save file. 
	* @return A pointer to the ChallengeHandler.
	*/
	static ChallengeHandler* create(const Json::Value& value);
	ChallengeHandler(const Json::Value& value);
	~ChallengeHandler();
	void addTower(Tower::TowerType type, cocos2d::Vec2 position);
	void upgradeTower(Tower::TowerType type, cocos2d::Vec2 position);
	void update();
	void endChallengeHandler();
	int countSucceedChallenges();
	void hideDescription();
};

class Challenge {
public:
	enum State {
		Running,
		Failed,
		Succeed
	};
	enum ChallengeType {
		Untouchable,
		Perfectionist,
		Misanthrope,
		Philanthrope,
		Stingy,
		Believer,
		Discriminant,
		Narrow,
		Reckless,
		Barbarian
	};
	static ChallengeType getChallengeTypeFromString(std::string type);
	ChallengeType getChallengeType();
	int getIntValue();
	std::string getStringValue();
	bool hasFailed();
	void setState(State n_state);
	State getState();

	static Challenge* create(ChallengeType type, int value, std::string string_value);
	Challenge(ChallengeType type, int value, std::string string_value);

private:
	ChallengeType type;
	int int_value;
	std::string str_value;
	State state;
	
};
#endif