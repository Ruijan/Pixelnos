#ifndef DANGORILLE_HPP
#define DANGORILLE_HPP

#include "cocos2d.h"
#include "../Config/json.h"
#include "Dangosimple.h"
#include "ui/CocosGUI.h"
#include "Monkey.h"


class Cell;

class Dangorille : public Dangosimple{

public:
	enum SpecialAttackState {
		WAITING,
		STARTING,
		DONE
	};
	Dangorille(std::vector<Cell*> npath, int nlevel);
	virtual ~Dangorille();
	static Dangorille* create(std::vector<Cell*> npath, int nlevel);

	virtual void skeletonAnimationHandle();
	static Json::Value getConfig();
	Json::Value getSpecConfig();
	void attackSpe(float dt);
	virtual void update(float dt);
	void updateIDLE();
	void updateMove(float dt);
	void updateSpecialAttack(float dt);
	virtual std::string getAttackSpeAnimationName();
	bool shouldAttackSpe();
	void generateMonkeys();
	void startAttackSpeAnimation();
	virtual void endDyingAnimation();

private:
	double nb_monkeys;
	double attack_spe_reload_timer;
	double attack_spe_reload_time;
	std::vector<std::string> textToDisplay;
	SpecialAttackState speAttackState;
};

#endif
