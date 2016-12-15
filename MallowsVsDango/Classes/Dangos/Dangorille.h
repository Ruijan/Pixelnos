#ifndef DANGORILLE_HPP
#define DANGORILLE_HPP

#include "cocos2d.h"
#include "../Config/json.h"
#include "Dangosimple.h"
#include "ui/CocosGUI.h"


class Cell;

class Dangorille : public Dangosimple{

public:
	Dangorille(std::vector<Cell*> npath, int nlevel);
	virtual ~Dangorille();
	static Dangorille* create(std::vector<Cell*> npath, int nlevel);

	virtual void skeletonAnimationHandle();
	static Json::Value getConfig();
	Json::Value getSpecConfig();
	void attackSpe(float dt);
	virtual void update(float dt);
	virtual void updateAnimation();
	bool shouldAttackSpe();
	void generateMonkeys();
	void startAttackSpeAnimation();
	void animateWords(std::vector<cocos2d::Label*> texts, int index);
	virtual void endDyingAnimation();

private:
	double nb_monkeys;
	std::vector<cocos2d::ui::Layout*> monkeys;
	double attack_spe_reload_timer;
	double attack_spe_reload_time;
	Json::Value attack_spe_text;
	bool attack_spe_done;
};

#endif
