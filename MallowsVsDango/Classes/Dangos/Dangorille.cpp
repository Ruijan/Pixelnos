#include "Dangorille.h"
#include "../Level/Cell.h"
#include "../AppDelegate.h"

USING_NS_CC;

Dangorille::Dangorille(std::vector<Cell*> npath, int nlevel) :
Dangosimple(npath, nlevel), attack_spe_done(true){
	auto config = getSpecConfig();
	attack_spe_reload_time = config["level"][nlevel]["reload_spe"].asDouble();
	nb_monkeys = config["level"][nlevel]["attack_spe"]["nb_monkeys"].asInt();
	attack_spe_reload_timer = 0;
	attack_spe_text = config["level"][nlevel]["attack_spe"]["text_" + ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage()];
}

Dangorille::~Dangorille() {
	for (unsigned int i(0); i < monkeys.size(); ++i) {
		((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->removeChild(monkeys[i]);
	}

	//std::cerr << "Dangorille Destroyed ! confirmed !" << std::endl;
}

Dangorille* Dangorille::create(std::vector<Cell*> npath, int nlevel)
{
	Dangorille* pSprite = new Dangorille(npath, nlevel);

	/*if (pSprite->initWithFile(getConfig()["level"][nlevel]["image"].asString()))
	{*/
		pSprite->initFromConfig();
		//pSprite->setScale(Cell::getCellWidth() / pSprite->getContentSize().width);
		pSprite->autorelease();

		//pSprite->setAnchorPoint(Point(0.5,0.25));
		//pSprite->updateAnimation();

		return pSprite;
	//}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void Dangorille::attackSpe(float dt) {
	((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->pause();
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
	Size visibleSize = Director::getInstance()->getVisibleSize();

	reload_timer += dt;
	attack_spe_reload_timer += dt;
	switch (state) {
	case IDLE:
		if (shouldAttack() && reload_timer > attack_reloading) {
			state = ATTACK;
			updateAnimation();
			reload_timer = 0;
		}
		else if (path[targetedCell]->isFree()) {
			state = MOVE;
			updateAnimation();
		}
		break;
	case ATTACK:
		if (shouldAttack()) {
			attack(dt);
			state = RELOAD;
		}
		else {
			state = MOVE;
			updateAnimation();
		}
		break; 
	case ATTACK_SPE:
		if (shouldAttackSpe() && attack_spe_done) {
			attackSpe(dt);
			attack_spe_done = false;
		}
		else if(attack_spe_done){
			state = MOVE;
			updateAnimation();
			attack_spe_reload_timer = 0;
		}
		break;
	case MOVE:
		if (path[targetedCell]->isFree()) {
			move(dt);
			if (shouldAttackSpe()) {
				state = ATTACK_SPE;
				updateAnimation();
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
		break;
	}
}

bool Dangorille::shouldAttackSpe() {
	return monkeys.size() < nb_monkeys && attack_spe_reload_timer > attack_spe_reload_time;
}
void Dangorille::updateAnimation() {
	Dango::updateAnimation();
	// Add the case where the state of the system is ATTACK_SPE
}

void Dangorille::generateMonkeys() {
	// generate monkeys
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Level* c_level = ((SceneManager*)SceneManager::getInstance())->getGame()->getLevel();
	std::vector<Tower*> towers = c_level->getTowers();
	std::vector<int> tower_indices;
	int actual_monkeys = monkeys.size();
	for (unsigned int i(0); i < ((nb_monkeys < towers.size()) ? nb_monkeys : towers.size()) - actual_monkeys; ++i) {
		int index;
		bool already_index = false;
		do {
			already_index = false;
			index = rand() % towers.size();
			if (!towers[index]->isTowerBlocked()) {
				for (unsigned int j(0); j < tower_indices.size(); ++j) {
					if (tower_indices[j] == index) {
						already_index = true;
					}
				}
			}
			else {
				already_index = true;
			}
		} while (already_index);
		tower_indices.push_back(index);
		Tower* target_tower = towers[index];
		target_tower->blockTower(true);

		auto* showAction = EaseBackIn::create(MoveTo::create(0.5f, target_tower->getPosition()));

		Sprite* liane = Sprite::create("res/dango/liane.png");
		liane->setScale(Cell::getCellWidth() * 0.8 / liane->getContentSize().width);
		liane->setPosition(Vec2(target_tower->getPosition().x, visibleSize.height / 2 + liane->getContentSize().height / 2));
		liane->setAnchorPoint(Vec2(0.5, 0));
		liane->runAction(Sequence::createWithTwoActions(DelayTime::create(0.25f), showAction->clone()));

		auto monkey_layout = ui::Layout::create();
		Sprite* shadow = Sprite::create("res/buttons/monkey_shadow.png");
		shadow->setPosition(target_tower->getPosition());
		shadow->setScale(Cell::getCellWidth() / shadow->getContentSize().width * 0.01f);
		shadow->runAction(ScaleTo::create(0.5f, Cell::getCellWidth() / shadow->getContentSize().width));

		ui::Button* monkey = ui::Button::create("res/dango/monkey.png");
		monkey->setScale(Cell::getCellWidth() / monkey->getContentSize().width);
		monkey->setPosition(Vec2(target_tower->getPosition().x,
			visibleSize.height + monkey->getContentSize().height * monkey->getScaleY() * 0.6));
		
		monkey->runAction(Sequence::createWithTwoActions(DelayTime::create(0.25f),showAction));
		monkey->addTouchEventListener([&, monkey, target_tower, shadow, liane, monkey_layout, this](Ref* sender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				target_tower->blockTower(false);
				Size visibleSize = Director::getInstance()->getVisibleSize();
				auto* hideAction = EaseBackIn::create(MoveTo::create(0.5f, Vec2(monkey->getPosition().x, visibleSize.height +
					monkey->getContentSize().height * monkey->getScaleY() * 0.6)));
				auto delete_monkey = CallFunc::create([monkey_layout]() {
					((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->removeChild(monkey_layout);
				});
				monkey->runAction(Sequence::createWithTwoActions(hideAction, delete_monkey));
				liane->runAction(EaseBackIn::create(MoveTo::create(0.5f, Vec2(liane->getPosition().x, visibleSize.height))));
				shadow->runAction(ScaleTo::create(0.5f, Cell::getCellWidth() / shadow->getContentSize().width * 0.01f));
				monkey->setEnabled(false);
				int index = 0;
				for (unsigned int j(0); j < monkeys.size(); ++j) {
					if (monkey_layout == monkeys[j]) {
						index = j;
					}
				}
				monkeys.erase(monkeys.begin() + index);
			}
		});
		monkey_layout->addChild(liane);
		monkey_layout->addChild(shadow);
		monkey_layout->addChild(monkey);
		((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->addChild(monkey_layout);
		monkeys.push_back(monkey_layout);
	}
}

void Dangorille::startAttackSpeAnimation() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	ui::Layout* spe_attack = ui::Layout::create();
	ui::Button* background_mask = ui::Button::create("res/buttons/mask.png");
	background_mask->setScaleX(visibleSize.width / background_mask->getContentSize().width);
	background_mask->setScaleY(visibleSize.height / background_mask->getContentSize().height);
	Sprite* background = Sprite::create("res/buttons/spe_attack_background.png");
	background->setScaleX(visibleSize.width / background->getContentSize().width);
	background->setScaleY(visibleSize.height / 3 / background->getContentSize().height);
	spe_attack->addChild(background_mask);
	spe_attack->addChild(background);

	int nb_dots(6);
	std::vector<float> positions_y;
	
	for (int i(0); i < nb_dots; ++i) {
		Sprite* dot_1 = Sprite::create("res/buttons/spe_attack_dot.png");
		float scalex = (rand() % int(visibleSize.width / 3) + visibleSize.width / 3) / dot_1->getContentSize().width;
		float scaley = visibleSize.height / 3 * 0.05 / dot_1->getContentSize().height;
		float posx = rand() % int(visibleSize.width) - visibleSize.width / 2 - dot_1->getContentSize().width * dot_1->getScaleX() / 2;
		float sign = rand() % 2;
		float posy;
		bool is_inside(false);
		sign = (-1 * (sign == 0) + 1 * (sign == 1));
		do {
			is_inside = false;
			sign = sign > 0 ? -1 : 1;
			posy = sign *
				(rand() % int(background->getContentSize().height * background->getScaleY())) * 0.4;
			for (unsigned int j(0); j < positions_y.size(); ++j) {
				if (abs(posy - positions_y[j]) < visibleSize.height / 3 * 0.05) {
					is_inside = true;
				}
			}
		} while (is_inside);
		positions_y.push_back(posy);
		dot_1->setScaleX(scalex);
		dot_1->setScaleY(scaley);
		dot_1->setPosition(Vec2(posx, posy));
		auto reset_position = CallFunc::create([dot_1, visibleSize]() {
			dot_1->setPosition(Vec2(-dot_1->getContentSize().width * dot_1->getScaleX() / 2 - visibleSize.width / 2,
				dot_1->getPosition().y));
		});
		dot_1->runAction(RepeatForever::create(Sequence::createWithTwoActions(MoveTo::create(rand() % 100 / 100.0f * 0.3 + 0.3,
			Vec2(visibleSize.width / 2 + dot_1->getContentSize().width * dot_1->getScaleX() / 2,
				dot_1->getPosition().y)), reset_position)));
		spe_attack->addChild(dot_1);
	}
	Sprite* dangorille = Sprite::create("res/dango/dangorille1.png");
	dangorille->setScale(visibleSize.width / dangorille->getContentSize().width * 0.2);
	dangorille->setPosition(Vec2(visibleSize.width / 2 + dangorille->getContentSize().width*dangorille->getScaleX() / 2,
		0));
	dangorille->runAction(Sequence::create(
		EaseOut::create(MoveTo::create(1.0f, Vec2(-visibleSize.width / 4, dangorille->getPosition().y)), 3.f),
		MoveTo::create(2.5f, Vec2(-visibleSize.width / 3, dangorille->getPosition().y)),
		MoveTo::create(0.25f, Vec2(-visibleSize.width / 2 - dangorille->getContentSize().width*dangorille->getScaleX() / 2,
			dangorille->getPosition().y)),
		nullptr));
	spe_attack->addChild(dangorille);

	std::vector<Label*> texts;
	int index(0);
	float bin_size = (visibleSize.height * 4 / 5) / attack_spe_text.size();
	
	for (unsigned int i(0); i < attack_spe_text.size(); ++i) {
		Label* text1 = Label::createWithTTF(attack_spe_text[i].asString(), "fonts/This Night.ttf", 120.0f);
		text1->setColor(Color3B::RED);
		text1->enableOutline(Color4B::BLACK, 15);
		text1->setPosition(Vec2(visibleSize.width / 4, visibleSize.height * 2 / 5  - (1 - attack_spe_text.size() % 2) * text1->getContentSize().height / 2 - 
			(i + attack_spe_text.size() % 2) * visibleSize.height * 1 / 5));
		text1->setScale(3.0f);
		text1->runAction(Hide::create());
		spe_attack->addChild(text1);
		texts.push_back(text1);
	}

	auto resume_game = CallFunc::create([spe_attack]() {
		((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->resume();
		((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->removeChild(spe_attack);
	});
	auto call_monkeys = CallFunc::create([this]() {
		spTrackEntry* anim = skeleton->setAnimation(0, "call_monkeys", true);
	});

	auto animate_words = CallFunc::create([this, texts, index]() {
		animateWords(texts, index);
	});

	auto hide_words = CallFunc::create([this, texts]() {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		for (unsigned int i(0); i < texts.size(); ++i) {
			texts[i]->runAction(EaseOut::create(MoveTo::create(0.5f,
				Vec2(visibleSize.width / 2 + texts[i]->getContentSize().width, texts[i]->getPosition().y)), 3.f));
		}
	});

	spe_attack->runAction(Sequence::create(
		DelayTime::create(1.0f),
		animate_words,
		DelayTime::create(1.f + 0.5f * texts.size()),
		hide_words,
		DelayTime::create(0.5f),
		TargetedAction::create(background_mask, FadeIn::create(0.125f)),
		Hide::create(),
		resume_game,
		call_monkeys,
		nullptr)
	);

	spe_attack->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->addChild(spe_attack, 5);
}

void Dangorille::animateWords(std::vector<Label*> texts, int index) {
	if (index < (int)texts.size()) {
		index = index + 1;
		auto animate_words = CallFunc::create([this, texts, index]() {
			animateWords(texts, index);
		});
		texts[index - 1]->runAction(Sequence::create(
			Show::create(),
			EaseIn::create(ScaleTo::create(0.5f, 1), 3.f),
			animate_words,
			nullptr
		));
	}
}

void Dangorille::skeletonAnimationHandle() {
	Dango::skeletonAnimationHandle();
	std::string name = skeleton->getCurrent()->animation->name;
	if (Value(name).asString() == "call_monkeys") {
		generateMonkeys();
		attack_spe_done = true;
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
		Dango::endDyingAnimation();
	});

	runAction(Sequence::create(liane_action_down, Spawn::createWithTwoActions(liane_action_up, dangorille_action_up), switch_state, nullptr));
}