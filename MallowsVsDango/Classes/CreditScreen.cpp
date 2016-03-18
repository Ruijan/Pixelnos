#include "CreditScreen.h"
#include "Config/Config.h"
#include "AppDelegate.h"
#include "SceneManager.h"
#include "Lib/Functions.h"
#include "Lib/FadeMusic.h"

USING_NS_CC;

Label* credit_label;
std::string	credit_title = "Credits";
std::vector <std::string> credit_subtitle = {	"Game developers", 
												"Game designers", 
												"Music & sounds", 
												"Graphic helpers", 
												"Acknowledgement", 
												"Softwares & libraries" };
std::vector <std::string> credit_content_subtitle = {	"Julien Rechenmann\nXianle Wang\nThomas Czereba", 
														"Julien Rechenmann\n Xianle Wang\nThomas Czereba", 
														"Bensound.com\nSoundeffect-lab.info\npurple-planet.com", 
														"Subtlepatterns.com", 
														"Omid Ahoura\nAnn Pongsakul\nGenia Shevchenko\nAvril Li",
														"Cocos2d-x\nTexturePacker\nTiledMap\nInkscape\nGIMP\nSpriter" };

bool CreditScreen::init(){
	if (!Scene::init()){ return false; }

	// Instanciation du background et menu
	Size visibleSize = Director::getInstance()->getVisibleSize();
	addChild(Sprite::create("res/background/crissXcross.png"), 1, "background");
	getChildByName("background")->setAnchorPoint(Vec2(0.5, 0.5));
	getChildByName("background")->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	double ratioX = visibleSize.width / 960;
	double ratioY = visibleSize.height / 640;
	getChildByName("background")->setScale(visibleSize.width / getChildByName("background")->getContentSize().width);
	
	menu = Menu::create();

	// Instanciation du bouton de retour à la selection des niveaux
	std::string return_to_game_button_icon = "res/buttons/back.png";
	Color3B return_to_game_button_icon_color = Color3B::WHITE;
	Sprite* return_to_game_sprite = Sprite::create(return_to_game_button_icon);
	MenuItemSprite* return_to_game_item = MenuItemSprite::create(return_to_game_sprite, return_to_game_sprite, CC_CALLBACK_1(CreditScreen::returnToGame, this));
	return_to_game_item->setPosition(Vec2(visibleSize.width - return_to_game_item->getContentSize().width / 2,
		visibleSize.height - return_to_game_item->getContentSize().height / 2));
	return_to_game_item->setEnabled(true);
	return_to_game_item->setScale(ratioX);
	
	// Instanciation du texte des crédits. On itère les deux listes en faisant correspondre les éléments entre eux.
	credit_label = Label::createWithTTF(credit_title, "fonts/Love Is Complicated Again.ttf", 36);
	int between_sub_content = 15;
	double shift_left = credit_label->getContentSize().width / 2;
	for (unsigned int i = 0; i < credit_content_subtitle.size(); i++) {
		Label* credit_subtitle_bold = Label::createWithTTF(credit_subtitle[i], "fonts/Love Is Complicated Again.ttf", 28);
		credit_subtitle_bold->setAlignment(TextHAlignment::CENTER);
		credit_subtitle_bold->setPosition(Vec2(shift_left, -between_sub_content));
		credit_subtitle_bold->setAnchorPoint(Vec2(0.5, 1));
		credit_subtitle_bold->enableOutline(Color4B::ORANGE, 3);
		credit_label->addChild(credit_subtitle_bold);
		between_sub_content += 28;
		Label* credit_subtitle_content = Label::createWithTTF(credit_content_subtitle[i], "fonts/Love Is Complicated Again.ttf", 22);
		credit_subtitle_content->setAlignment(TextHAlignment::CENTER);
		credit_subtitle_content->setColor(Color3B::WHITE);
		credit_subtitle_content->setPosition(Vec2(shift_left, -between_sub_content));
		credit_subtitle_content->setAnchorPoint(Vec2(0.5, 1));
		credit_subtitle_content->enableOutline(Color4B::WHITE, 1);
		credit_label->addChild(credit_subtitle_content);
		between_sub_content += 20 + ((countLine(credit_content_subtitle[i])) * 25);
	}
	credit_label->enableOutline(Color4B::ORANGE, 3);
	//credit_label->setPosition(Vec2(-visibleSize.width / 16 - 10 * visibleSize.width / 960 - 360, 0));
	credit_label->setPosition(Vec2(visibleSize.width / 2, 0));
	credit_label->setAnchorPoint(Vec2(0.5, 1));

	//return_to_game_item->addChild(credit_label);
	addChild(credit_label,2);


	menu->addChild(return_to_game_item);
	menu->setPosition(0, 0);

	addChild(menu, 2);

	return true;
}

void CreditScreen::onEnterTransitionDidFinish(){
	Scene::onEnterTransitionDidFinish();
	// On créé le mouvement de scroll + le callback permettant de revenir à la "Selection des niveaux" lorsque les crédits sont finis.
	auto moveBy = MoveBy::create(15, Vec2(0, 1500));
	auto callBackToGame = CallFunc::create([]() {
		SceneManager::getInstance()->setScene(SceneManager::LEVELS);
	});
	Sequence* seq = Sequence::create(moveBy, callBackToGame, NULL);
	credit_label->runAction(seq);
}

void CreditScreen::onExitTransitionDidStart() {
	Scene::onExitTransitionDidStart();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	credit_label->setPosition(Vec2(visibleSize.width / 2, 0));
}


/**	Fonction permettant de retourner dans le la scène "Selection des niveaux"
*	@param Ref sender : Scène qui va être changée.
*/
void CreditScreen::returnToGame(Ref* sender){
	SceneManager::getInstance()->setScene(SceneManager::LEVELS);
}

/** Fonction permettant de compter le nombre de ligne dans un texte donné
*	@param std:string text : texte de lignes à compter.
*	@return int : Nombre de lignes.
*/
int CreditScreen::countLine(std::string text) {
	int count = 0;
	for (unsigned int i = 0; i < text.size(); i++)
		if (text[i] == '\n') count++;
	return (count + 1);
}
