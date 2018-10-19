#include "SpecialAttackAnnouncement.h"
#include "../../SceneManager.h"

SpecialAttackAnnouncement * SpecialAttackAnnouncement::create(std::vector<std::string> textToDisplay)
{
	SpecialAttackAnnouncement * annoucement = new SpecialAttackAnnouncement();
	annoucement->init(textToDisplay);
	return annoucement;
}

bool SpecialAttackAnnouncement::init(std::vector<std::string> textToDisplay) {
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	initBackground();
	initDotsAnimation();
	initDangorillaSprite();
	initLabelsToDisplay(textToDisplay);
	return true;
}

void SpecialAttackAnnouncement::initBackground()
{
	background_mask = cocos2d::ui::Button::create("res/buttons/mask.png");
	background_mask->setScaleX(visibleSize.width / background_mask->getContentSize().width);
	background_mask->setScaleY(visibleSize.height / background_mask->getContentSize().height);

	background = cocos2d::Sprite::create("res/buttons/spe_attack_background.png");
	background->setScaleX(visibleSize.width / background->getContentSize().width);
	background->setScaleY(visibleSize.height / 3 / background->getContentSize().height);
	addChild(background_mask);
	addChild(background);
}

void SpecialAttackAnnouncement::initDotsAnimation()
{
	int nb_dots(6);
	double maxHeightDotPosition = background->getContentSize().height * background->getScaleY()*0.4;
	double maxWidthDotPosition = visibleSize.width / 2.0;
	std::vector<float> positions_y;
	std::random_device randomDevice;  //Will be used to obtain a seed for the random number engine
	std::mt19937 generator(randomDevice()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> verticalDistribution(-maxHeightDotPosition, maxHeightDotPosition);
	std::uniform_real_distribution<> horizontalDistribution(-maxWidthDotPosition, maxWidthDotPosition);
	std::uniform_real_distribution<> scaleXDistribution(visibleSize.width / 3, 2 * visibleSize.width / 3);
	std::uniform_real_distribution<> durationDistribution(0.3, 0.6); // in seconds


	for (int i(0); i < nb_dots; ++i) {
		cocos2d::Sprite* dot = generateRandomDot(scaleXDistribution, generator, horizontalDistribution,
			verticalDistribution, positions_y);
		auto reset_position = cocos2d::CallFunc::create([this, dot]() {
			dot->setPosition(cocos2d::Vec2(-dot->getContentSize().width * dot->getScaleX() / 2 - visibleSize.width / 2,
				dot->getPosition().y));
		});
		dot->runAction(cocos2d::RepeatForever::create(cocos2d::Sequence::createWithTwoActions(cocos2d::MoveTo::create(durationDistribution(generator),
			cocos2d::Vec2(visibleSize.width / 2 + dot->getContentSize().width * dot->getScaleX() / 2,
				dot->getPosition().y)), reset_position)));
		addChild(dot);
	}
}

cocos2d::Sprite* SpecialAttackAnnouncement::generateRandomDot(
	std::uniform_real_distribution<> &scaleXDistribution, std::mt19937 &generator, 
	std::uniform_real_distribution<> &horizontalDistribution, std::uniform_real_distribution<> &verticalDistribution, 
	std::vector<float> &positions_y)
{
	cocos2d::Sprite* dot = cocos2d::Sprite::create("res/buttons/spe_attack_dot.png");
	float scalex = getRandomXScale(scaleXDistribution, generator, dot);
	float scaley = visibleSize.height / 3 * 0.05 / dot->getContentSize().height;
	float posx = getRandomXPosition(horizontalDistribution, generator, dot);
	float posy = getRandomYPosition(verticalDistribution, generator, positions_y);
	positions_y.push_back(posy);
	dot->setScaleX(scalex);
	dot->setScaleY(scaley);
	dot->setPosition(cocos2d::Vec2(posx, posy));
	positions_y.push_back(posy);
	return dot;
}

float SpecialAttackAnnouncement::getRandomYPosition(std::uniform_real_distribution<> & verticalDistribution, std::mt19937 & generator, std::vector<float> & positions_y)
{
	float posy;
	bool is_inside(false);
	do {
		is_inside = false;
		posy = verticalDistribution(generator);
		for (unsigned int j(0); j < positions_y.size(); ++j) {
			if (abs(posy - positions_y[j]) < visibleSize.height / 3 * 0.05) {
				is_inside = true;
			}
		}
	} while (is_inside);
	return posy;
}

double SpecialAttackAnnouncement::getRandomXPosition(std::uniform_real_distribution<> & horizontalDistribution, std::mt19937 & generator, cocos2d::Sprite * dot)
{
	return horizontalDistribution(generator) - dot->getContentSize().width * dot->getScaleX() / 2;
}

double SpecialAttackAnnouncement::getRandomXScale(std::uniform_real_distribution<> & scaleXDistribution, std::mt19937 & generator, cocos2d::Sprite * dot)
{
	return scaleXDistribution(generator) / dot->getContentSize().width;
}

void SpecialAttackAnnouncement::initDangorillaSprite()
{
	dangorille = cocos2d::Sprite::create("res/dango/dangorille1.png");
	dangorille->setScale(visibleSize.width / dangorille->getContentSize().width * 0.2);
	dangorille->setPosition(cocos2d::Vec2(visibleSize.width / 2 + dangorille->getContentSize().width*dangorille->getScaleX() / 2,
		0));
	dangorille->runAction(cocos2d::Sequence::create(
		cocos2d::EaseOut::create(cocos2d::MoveTo::create(1.0f, cocos2d::Vec2(-visibleSize.width / 4, dangorille->getPosition().y)), 3.f),
		cocos2d::MoveTo::create(2.5f, cocos2d::Vec2(-visibleSize.width / 3, dangorille->getPosition().y)),
		cocos2d::MoveTo::create(0.25f, cocos2d::Vec2(-visibleSize.width / 2 - dangorille->getContentSize().width*dangorille->getScaleX() / 2,
			dangorille->getPosition().y)),
		nullptr));
	addChild(dangorille);
}

void SpecialAttackAnnouncement::initLabelsToDisplay(std::vector<std::string> &textToDisplay)
{
	for (unsigned int i(0); i < textToDisplay.size(); ++i) {
		cocos2d::Label* text1 = cocos2d::Label::createWithTTF(textToDisplay[i], "fonts/This Night.ttf", 120.0f);
		text1->setColor(cocos2d::Color3B::RED);
		text1->enableOutline(cocos2d::Color4B::BLACK, 15);
		text1->setPosition(cocos2d::Vec2(visibleSize.width / 4, visibleSize.height * 2 / 5 - (1 - textToDisplay.size() % 2) * text1->getContentSize().height / 2 -
			(i + textToDisplay.size() % 2) * visibleSize.height * 1 / 5));
		text1->setScale(3.0f);
		text1->runAction(cocos2d::Hide::create());
		addChild(text1);
		textLabels.push_back(text1);
	}
}

void SpecialAttackAnnouncement::start()
{
	runAction(getAnnoucementAction());
}

cocos2d::FiniteTimeAction* SpecialAttackAnnouncement::getAnnoucementAction() {
	int index(0);
	auto resume_game = cocos2d::CallFunc::create([this]() {
		((SceneManager*)SceneManager::getInstance())->getGame()->getLevel()->resume();
		((SceneManager*)SceneManager::getInstance())->getGame()->getMenu()->removeChild(this);
	});
	auto animateWordAction = cocos2d::CallFunc::create([this, index]() {
		animateWords(index);
	});

	auto hide_words = cocos2d::CallFunc::create([this]() {
		cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
		for (unsigned int i(0); i < textLabels.size(); ++i) {
			textLabels[i]->runAction(cocos2d::EaseOut::create(cocos2d::MoveTo::create(0.5f,
				cocos2d::Vec2(visibleSize.width / 2 + textLabels[i]->getContentSize().width, textLabels[i]->getPosition().y)), 3.f));
		}
	});

	return cocos2d::TargetedAction::create(this, cocos2d::Sequence::create(
		cocos2d::DelayTime::create(1.0f),
		animateWordAction,
		cocos2d::DelayTime::create(1.f + 0.5f * textLabels.size()),
		hide_words,
		cocos2d::DelayTime::create(0.5f),
		cocos2d::TargetedAction::create(background_mask, cocos2d::FadeIn::create(0.125f)),
		cocos2d::Hide::create(),
		resume_game,
		nullptr));
}

void SpecialAttackAnnouncement::animateWords(int index) {
	if (index < (int)textLabels.size()) {
		index = index + 1;
		auto animateWordAnimation = cocos2d::CallFunc::create([this, index]() {
			animateWords(index);
		});
		textLabels[index - 1]->runAction(cocos2d::Sequence::create(
			cocos2d::Show::create(),
			cocos2d::EaseIn::create(cocos2d::ScaleTo::create(0.5f, 1), 3.f),
			animateWordAnimation,
			nullptr
		));
	}
}

std::vector<cocos2d::Label*> SpecialAttackAnnouncement::getTextLabels()
{
	return textLabels;
}
