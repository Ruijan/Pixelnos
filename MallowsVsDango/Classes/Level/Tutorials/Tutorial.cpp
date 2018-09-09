#include "Tutorial.h"

Tutorial::Tutorial(TutorialSettings* settings) :
	running(false),
	settings(settings)
{
}

void Tutorial::shakeElement(cocos2d::Node* element, bool loop) {
	cocos2d::RotateTo* left = cocos2d::RotateTo::create(0.1f, 15);
	cocos2d::RotateTo* right = cocos2d::RotateTo::create(0.1f, -15);
	cocos2d::RotateTo* center = cocos2d::RotateTo::create(0.2f, 0);
	cocos2d::Sequence* seq = cocos2d::Sequence::create(
		left, right,
		left->clone(), right->clone(),
		left->clone(), right->clone(),
		center, cocos2d::DelayTime::create(1.f),
		nullptr);
	if (loop) {
		element->runAction(cocos2d::RepeatForever::create(seq));
	}
	else {
		element->runAction(seq);
	}

}

void Tutorial::shakeScaleElement(cocos2d::Node* element, bool loop) {
	cocos2d::RotateTo* left = cocos2d::RotateTo::create(0.1f, 15);
	cocos2d::RotateTo* right = cocos2d::RotateTo::create(0.1f, -15);
	cocos2d::RotateTo* center = cocos2d::RotateTo::create(0.2f, 0);
	cocos2d::Spawn* spawn = cocos2d::Spawn::createWithTwoActions(
		cocos2d::Sequence::create(
			left, right,
			left->clone(), right->clone(),
			left->clone(), right->clone(),
			center, cocos2d::DelayTime::create(1.f),
			nullptr),
		cocos2d::Sequence::create(
			cocos2d::ScaleTo::create(0.4f, 1.5f),
			cocos2d::ScaleTo::create(0.4f, 1.f),
			nullptr)
	);

	if (loop) {
		element->runAction(cocos2d::RepeatForever::create(spawn));
	}
	else {
		element->runAction(spawn);
	}
}

void Tutorial::endTutorial() {
	running = false;
}

Tutorial::~Tutorial()
{
}
