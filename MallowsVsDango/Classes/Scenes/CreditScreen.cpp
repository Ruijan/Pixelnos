#include "CreditScreen.h"
#include "../Config/Config.h"
#include "../AppDelegate.h"
#include "../SceneManager.h"
#include "../Lib/Functions.h"
#include "../Lib/FadeMusic.h"

USING_NS_CC;

Label* credit_label;
std::string	credit_title = "Credits";
std::vector <std::string> credit_subtitle = { "Game developers",
												"Game designers",
												"Designers",
												"Music & sounds",
												"Graphic helpers",
												"Acknowledgement",
												"Softwares & libraries" };
std::vector <std::string> credit_content_subtitle = { "Julien Rechenmann",
														"Julien Rechenmann",
														"Julien Rechenmann",
														"Bensound.com\nSoundeffect-lab.info\npurple-planet.com",
														"Subtlepatterns.com",
														"Charline Loisil\nXianle Wang\nKevin Passageon\nThomas Czereba\nOmid Ahoura\nAnn Pongsakul\nGenia Shevchenko\nAvril Li",
														"Cocos2d-x\nTexturePacker\nTiledMap\nInkscape\nGIMP\nSpine" };

bool CreditScreen::init() {
	if (!Scene::init()) { return false; }

	// Instanciation du background et menu
	Size visibleSize = Director::getInstance()->getVisibleSize();
	addChild(Sprite::create("res/background/crissXcross.png"), 1, "background");
	getChildByName("background")->setAnchorPoint(Vec2(0.5, 0.5));
	getChildByName("background")->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	double ratioX = visibleSize.width / 960;
	double ratioY = visibleSize.height / 640;
	getChildByName("background")->setScale(visibleSize.width / getChildByName("background")->getContentSize().width);

	// Instanciation du bouton de retour à la selection des niveaux
	auto return_to_game_button = ui::Button::create("res/buttons/restart_button.png");
	return_to_game_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			SceneManager::getInstance()->setScene(SceneFactory::LEVELS);
		}
	});
	return_to_game_button->setPosition(Vec2(visibleSize.width, visibleSize.height));
	return_to_game_button->setAnchorPoint(Vec2(1, 1));
	return_to_game_button->setEnabled(true);
	return_to_game_button->setScale(visibleSize.width / 15 / return_to_game_button->getContentSize().width);
	addChild(return_to_game_button, 2);

	// Instanciation du texte des crédits. On itère les deux listes en faisant correspondre les éléments entre eux.
	auto layout = ui::Layout::create();
	addChild(layout, 2, "layout");
	credit_label = Label::createWithTTF(credit_title, "fonts/Love Is Complicated Again.ttf", 72 * visibleSize.width / 1280);
	credit_label->enableOutline(Color4B::ORANGE, 3);
	credit_label->setPosition(Vec2(visibleSize.width / 2, 0));
	credit_label->setAnchorPoint(Vec2(0.5, 1));
	layout->addChild(credit_label, 2);
	Label* previous_label = credit_label;
	double total_height = credit_label->getContentSize().height;
	for (unsigned int i = 0; i < credit_content_subtitle.size(); i++) {
		Label* credit_subtitle_bold = Label::createWithTTF(credit_subtitle[i], "fonts/Love Is Complicated Again.ttf", 56 * visibleSize.width / 1280);
		credit_subtitle_bold->setAlignment(TextHAlignment::CENTER);
		credit_subtitle_bold->setPosition(Vec2(visibleSize.width / 2, previous_label->getPosition().y -
			previous_label->getContentSize().height - visibleSize.height / 15));
		credit_subtitle_bold->setAnchorPoint(Vec2(0.5, 1));
		credit_subtitle_bold->enableOutline(Color4B::ORANGE, 3);
		layout->addChild(credit_subtitle_bold);
		previous_label = credit_subtitle_bold;

		Label* credit_subtitle_content = Label::createWithTTF(credit_content_subtitle[i], "fonts/Love Is Complicated Again.ttf", 44 * visibleSize.width / 1280);
		credit_subtitle_content->setAlignment(TextHAlignment::CENTER);
		credit_subtitle_content->setColor(Color3B::WHITE);
		credit_subtitle_content->setPosition(Vec2(visibleSize.width / 2, previous_label->getPosition().y -
			previous_label->getContentSize().height));
		credit_subtitle_content->setAnchorPoint(Vec2(0.5, 1));
		credit_subtitle_content->enableOutline(Color4B::WHITE, 1);
		layout->addChild(credit_subtitle_content);
		total_height = -credit_subtitle_content->getPosition().y + credit_subtitle_content->getContentSize().height;
		previous_label = credit_subtitle_content;
	}
	layout->setContentSize(Size(visibleSize.width, total_height));

	return true;
}

void CreditScreen::onEnterTransitionDidFinish() {
	Scene::onEnterTransitionDidFinish();
	Size visibleSize = Director::getInstance()->getVisibleSize();

	// On créé le mouvement de scroll + le callback permettant de revenir à la "Selection des niveaux" lorsque les crédits sont finis.
	auto moveBy = MoveBy::create(15, Vec2(0, getChildByName("layout")->getContentSize().height + visibleSize.height));
	auto callBackToGame = CallFunc::create([]() {
		SceneManager::getInstance()->setScene(SceneFactory::LEVELS);
	});
	Sequence* seq = Sequence::create(moveBy, callBackToGame, NULL);
	getChildByName("layout")->runAction(seq);
}

void CreditScreen::onExitTransitionDidStart() {
	Scene::onExitTransitionDidStart();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	getChildByName("layout")->stopAllActions();
	getChildByName("layout")->setPosition(Vec2(0, 0));
}

void CreditScreen::switchLanguage() {

}