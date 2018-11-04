#ifndef CHALLENGE_HANDLER_HPP
#define CHALLENGE_HANDLER_HPP

#include "cocos2d.h"
#include "../../Config/json.h"
#include "Towers/Tower.h"

class Challenge;
class Level;

class ChallengeHandler: public cocos2d::Node {
public:
	/**
	* @brief Create a ChallengeHandler from the configuration file and the save file. 
	* @return A pointer to the ChallengeHandler.
	*/
	static ChallengeHandler* create(const Json::Value& value, Level* level);
	~ChallengeHandler();
	bool init(const Json::Value& value, Level* level);
	void createChallengesButtons(cocos2d::Size &visibleSize, const Json::Value & value);
	void createChallenge(Json::Value &configChallenge, Json::Value &j_challenges, std::string &language, float size_button);
	void showChallenge(cocos2d::ui::Button * challenge, const std::string &name, const std::string &description, Challenge * c_challenge);
	void updateChallengeText(Challenge * c_challenge, const std::string & description, const std::string & name, cocos2d::ui::Button * challenge);
	void createTextLayout(cocos2d::Size &visibleSize);
	void addTower(Tower::TowerType type, cocos2d::Vec2 position);
	void showChallengeFailure(Challenge *& challenge);
	void addTowerMisanthrope(std::vector<Tower *> &towers, cocos2d::Vec2 &position, Challenge *& challenge);
	void addTowerPhilanthrope(std::vector<Tower *> &towers, bool &touching, cocos2d::Vec2 &position, Challenge *& challenge);
	void addTowerDiscriminant(Challenge *& challenge, Tower::TowerType type);
	void addTowerNarrow(Challenge *& challenge, Tower::TowerType type);
	void addTowerReckless(cocos2d::Vec2 &position, bool &touching, Challenge *& challenge);
	void upgradeTower(Tower::TowerType type, cocos2d::Vec2 position);
	void update();
	void endChallengeHandler();
	int countSucceedChallenges();
	void hideDescription();

protected:
	std::vector<Challenge*> challenges;
	Level* level;
	cocos2d::ui::Layout* text_layout;
	cocos2d::Sprite* background;
	cocos2d::ui::Text* title_text;
	cocos2d::ui::Text* description_text;
	cocos2d::ui::Text* progress_text;
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