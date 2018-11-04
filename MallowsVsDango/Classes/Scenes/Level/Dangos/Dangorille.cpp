#include "Dangorille.h"
#include "../Cell.h"
#include "../../../AppDelegate.h"
#include "GUI/SpecialAttackAnnouncement.h"
#include <random>

USING_NS_CC;

Dangorille::Dangorille(std::vector<Cell*> npath, int nlevel, Level* globalLevel) :
Dangosimple(npath, nlevel), speAttackState(WAITING), globalLevel(globalLevel){
	auto config = getSpecConfig();
	attack_spe_reload_time = config["attack_spe"]["reload_spe"][nlevel].asDouble();
	nb_monkeys = config["attack_spe"]["nb_monkeys"][nlevel].asInt();
	attack_spe_reload_timer = 0;
	Json::Value attack_spe_text = config["attack_spe"]["text_" + ((AppDelegate*)Application::getInstance())->getConfigClass()->getSettings()->getLanguage()];
	for (unsigned int i(0); i < attack_spe_text.size(); ++i) {
		textToDisplay.push_back(attack_spe_text[i].asString());
	}
}

Dangorille::~Dangorille() {

}

Dangorille* Dangorille::create(std::vector<Cell*> npath, int nlevel, Level* globalLevel)
{
	Dangorille* pSprite = new Dangorille(npath, nlevel, globalLevel);

	pSprite->initFromConfig();
	pSprite->autorelease();
	return pSprite;

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void Dangorille::attackSpe(float dt) {
	globalLevel->pause();
	startAttackSpeAnimation();
}

Json::Value Dangorille::getConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::DANGO)["dangorille"];
}
Json::Value Dangorille::getSpecConfig(){
	return ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::DANGO)["dangorille"];
}

void Dangorille::update(float dt) {
	updateEffects(dt);

	reload_timer += dt;
	attack_spe_reload_timer += dt;
	switch (state) {
	case IDLE:
		updateIDLE();
		break;
	case ATTACK:
		updateAttack(dt);
		break; 
	case ATTACK_SPE:
		updateSpecialAttack(dt);
		break;
	case MOVE:
		updateMove(dt);
		break;
	}
}

void Dangorille::updateIDLE()
{
	if (shouldAttack() && reload_timer > attack_reloading) {
		state = ATTACK;
		updateAnimation();
		reload_timer = 0;
	}
	else if (path[targetedCell]->isFree()) {
		state = MOVE;
		updateAnimation();
	}
}

void Dangorille::updateMove(float dt)
{
	if (path[targetedCell]->isFree()) {
		move(dt);
		if (shouldAttackSpe()) {
			state = ATTACK_SPE;
			speAttackState = STARTING;
			attackSpe(dt);
		}
		else if (shouldAttack() && reload_timer > attack_reloading) {
			state = ATTACK;
			updateAnimation();
			reload_timer = 0;
		}
	}
	else {
		state = IDLE;
	}
}

void Dangorille::updateSpecialAttack(float dt)
{
	if (speAttackState == DONE) {
		state = MOVE;
		updateAnimation();
		attack_spe_reload_timer = 0;
	}
}

std::string Dangorille::getAttackSpeAnimationName() {
	return "calls_monkeys";
}

bool Dangorille::shouldAttackSpe() {
	unsigned int nbOfMonkeys = ((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->getNbOfMonkeys();
	return nbOfMonkeys < nb_monkeys && attack_spe_reload_timer > attack_spe_reload_time;
}

void Dangorille::generateMonkeys() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	MyGame* game = ((SceneManager*)SceneManager::getInstance())->getGame();
	std::vector<Tower*> towers = globalLevel->getTowers();
	std::vector<int> blockedTowerIndices;
	unsigned int currentNbOfMonkeys = game->getMenu()->getNbOfMonkeys();

	for (unsigned int i(0); i < ((nb_monkeys < towers.size()) ? nb_monkeys : towers.size()) - currentNbOfMonkeys; ++i) {
		Monkey* monkey = Monkey::create(towers, blockedTowerIndices);
		game->getMenu()->addMonkey(monkey);
	}
}

void Dangorille::startAttackSpeAnimation() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto call_monkeys = CallFunc::create([this]() {
		spTrackEntry* anim = skeleton->setAnimation(0, "call_monkeys", true);
	});

	SpecialAttackAnnouncement* annoucement = SpecialAttackAnnouncement::create(textToDisplay);
	annoucement->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->addChild(annoucement, 5);
	runAction(Sequence::create(annoucement->getAnnoucementAction(), call_monkeys, nullptr));
}

void Dangorille::skeletonAnimationHandle() {
	Dango::skeletonAnimationHandle();
	std::string name = skeleton->getCurrent()->animation->name;
	if (Value(name).asString() == "call_monkeys") {
		generateMonkeys();
		speAttackState = DONE;
	}
}

void Dangorille::endDyingAnimation() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	Sprite* liane = Sprite::create("res/dango/liane.png");
	liane->setScale(Cell::getCellWidth() * 0.8 / liane->getContentSize().width);
	liane->setPosition(Vec2(getPosition().x, visibleSize.height / 2 + liane->getContentSize().height / 2));
	liane->setAnchorPoint(Vec2(0.5, 0));
	auto* showAction = EaseBackIn::create(MoveTo::create(0.5f, getPosition()));
	auto* hideAction = EaseBackIn::create(MoveTo::create(0.5f, Vec2(getPosition().x, visibleSize.height / 2 + liane->getContentSize().height / 2)));
	((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->addChild(liane);

	TargetedAction* liane_action_down = TargetedAction::create(liane, Sequence::createWithTwoActions(DelayTime::create(0.25f), showAction->clone()));
	TargetedAction* liane_action_up = TargetedAction::create(liane, Sequence::createWithTwoActions(DelayTime::create(0.25f), hideAction->clone()));
	TargetedAction* dangorille_action_up = TargetedAction::create(this, Sequence::createWithTwoActions(DelayTime::create(0.25f), hideAction->clone()));
	auto switch_state = CallFunc::create([this]() {
		((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->makeAllMonkeysGoAway();
		Dango::endDyingAnimation();
	});

	runAction(Sequence::create(liane_action_down, Spawn::createWithTwoActions(liane_action_up, dangorille_action_up), switch_state, nullptr));
}