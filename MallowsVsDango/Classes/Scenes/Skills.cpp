#include "Skills.h"
#include "../AppDelegate.h"
#include "../SceneManager.h"


USING_NS_CC;

bool Skills::init(){

	if (!Scene::init()){ return false; }
	std::string language = ((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	config = ((AppDelegate*)Application::getInstance())->getConfig()["skills"]; //load data file
	root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file

	if (!root.isMember("holy_sugar")) {
		root["holy_sugar"] = 0;
	}

	Json::Value root2 = root;
	root2["skill"] = {};
	for (unsigned int k(0); k < config.size(); ++k) {
		for (unsigned int l(0); l < config[k].size(); ++l) {
			auto conf = config[k][l]["id"].asInt();
			root2["skill"][k][l]["id"] = config[k][l]["id"].asInt();
			root2["skill"][k][l]["bought"] = false;
		}
	}
	for (unsigned int i(0); i < root["skill"].size(); ++i) {
		for (unsigned int j(0); j < root["skill"][i].size(); ++j) {
			if (root["skill"][i][j].isMember("id")) {
				if (config[i][j]["id"].asInt() != root["skill"][i][j]["id"].asInt()) {
					for (unsigned int k(0); k < config.size(); ++k) {
						for (unsigned int l(0); l < config[k].size(); ++l) {
							if (config[k][l]["id"].asInt() == root["skill"][i][j]["id"].asInt()) {
								root2["skill"][k][l]["id"] = config[k][l]["id"].asInt();
								root2["skill"][k][l]["bought"] = root["skill"][i][j]["bought"].asBool();
							}
						}
					}
				}
				else {
					root2["skill"][i][j]["id"] = config[i][j]["id"].asInt();
					root2["skill"][i][j]["bought"] = root["skill"][i][j]["bought"].asBool();
				}
			}
		}
	}
	root = root2;
	((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(root);
	((AppDelegate*)Application::getInstance())->getConfigClass()->save();

	//generating a background for skills 
	Sprite* loading_background = Sprite::create("res/background/space.png");
	loading_background->setPosition(Vec2(visibleSize.width * 5 / 8, visibleSize.height / 2));
	loading_background->setScale(visibleSize.height / loading_background->getContentSize().height);
	addChild(loading_background);

	//return button to levels on the top right corner
	cocos2d::ui::Button* back = ui::Button::create("res/buttons/restart_button.png");
	Sprite* back_wood = Sprite::create("res/buttons/close2_shadow.png");

	back->addChild(back_wood, -1);
	addChild(back, 2);
	back->setScale(visibleSize.width / back->getContentSize().width / 15);
	back->setAnchorPoint(Vec2(1.f, 1.f));
	back_wood->setScale(0.98f);
	back_wood->setPosition(Vec2(back->getContentSize().width  / 2,
		back->getContentSize().height / 2));
	back->setPosition(Vec2(visibleSize.width, visibleSize.height));
	back->addTouchEventListener([back](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			SceneManager::getInstance()->setScene(SceneManager::LEVELS);
		}
	});

	
	//Skill_info, window left side
	Sprite* skill_info = Sprite::create("res/buttons/menupanel5.png");
	addChild(skill_info, 2);
	skill_info->setScaleX(visibleSize.width / skill_info->getContentSize().width / 4);
	skill_info->setScaleY(visibleSize.height / skill_info->getContentSize().height);
	skill_info->setPosition(Vec2(0.f, 0.f));
	skill_info->setAnchorPoint(Vec2(0.f, 0.f));

	Sprite* sugar_sprite1 = Sprite::create("res/buttons/holy_sugar.png");
	sugar_sprite1->setScale(visibleSize.width / sugar_sprite1->getContentSize().width / 12);
	sugar_sprite1->setPosition(Vec2(skill_info->getContentSize().width * skill_info->getScaleX() / 3, visibleSize.height -
		sugar_sprite1->getContentSize().height * sugar_sprite1->getScaleY() / 2 - visibleSize.height / 40));
	addChild(sugar_sprite1, 3);

	//current sugar info
	Label* sugar_amount = Label::createWithTTF("X " + root["holy_sugar"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 25));
	sugar_amount->setColor(Color3B::YELLOW);
	sugar_amount->enableOutline(Color4B::BLACK, 2);
	sugar_amount->setPosition(Vec2(skill_info->getContentSize().width * skill_info->getScaleX() / 3, 0) + sugar_sprite1->getPosition());
	addChild(sugar_amount, 3, "sugar_amount");

	//shop button
	cocos2d::ui::Button* shop = ui::Button::create("res/buttons/shop.png");
	shop->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			((SceneManager*)SceneManager::getInstance())->setScene(SceneManager::SceneType::SHOP);
		}
	});
	shop->setScale(visibleSize.width / shop->getContentSize().width * 0.18);
	shop->setPosition(Vec2(visibleSize.width / 8, sugar_sprite1->getPosition().y -
		sugar_sprite1->getContentSize().height * sugar_sprite1->getScaleY() / 2 -
		shop->getContentSize().height * shop->getScaleY() / 2
	));
	shop->setTitleText(((AppDelegate*)Application::getInstance())->getConfig()
		["buttons"]["shop"][language].asString());
	shop->setTitleFontName("fonts/LICABOLD.ttf");
	shop->setTitleFontSize(visibleSize.width / 30);
	Label* shop_label = shop->getTitleRenderer();
	shop_label->setColor(Color3B::RED);
	shop_label->enableOutline(Color4B::BLACK, 2);
	shop_label->setAlignment(TextHAlignment::CENTER);
	shop_label->setPosition(Vec2(shop->getContentSize().width * 0.7, shop->getContentSize().height / 2));
	addChild(shop, 3, "shop");

	Sprite* shop_drawing = Sprite::create("res/buttons/shop_drawing.png");
	shop_drawing->setScale(shop->getContentSize().width / 3 / shop_drawing->getContentSize().width);
	shop_drawing->setPosition(shop->getContentSize().width / 4, shop->getContentSize().height / 2);
	shop->addChild(shop_drawing);

	//label for setting skill_name
	Label* skill_name = Label::createWithTTF(config[0][0]["name_" + 
		((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage()].asString(), 
		"fonts/LICABOLD.ttf", round(visibleSize.width / 40));
	skill_name->setColor(Color3B::ORANGE);
	skill_name->enableOutline(Color4B::BLACK, 2);
	skill_name->setWidth(visibleSize.width / 5);
	skill_name->setHorizontalAlignment(TextHAlignment::CENTER);
	skill_name->setVerticalAlignment(TextVAlignment::TOP);
	skill_name->setAnchorPoint(Vec2(0.5f, 1.f));
	skill_name->setPosition(Vec2(visibleSize.width / 8, shop->getPosition().y - 
		shop->getContentSize().height * shop->getScaleY() / 2 - 
		skill_name->getContentSize().height / 2 - 
		visibleSize.height / 25
	));
	addChild(skill_name, 3, "skill_name");

	//label for setting skill_description
	Label* skill_description = Label::createWithTTF(config[0][0]["description_" + 
		((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage()].asString(), 
		"fonts/LICABOLD.ttf", round(visibleSize.width / 45));
	skill_description->setColor(Color3B::BLACK);
	skill_description->setDimensions(visibleSize.width / 5, visibleSize.height / 4);
	skill_description->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.45));
	skill_description->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
	skill_description->setAnchorPoint(Vec2(0.5f, 1.f));
	addChild(skill_description, 3, "skill_description");

	//add buy_button
	cocos2d::ui::Button* buy_button = ui::Button::create("res/buttons/buy.png");
	buy_button->setScale(visibleSize.width / buy_button->getContentSize().width / 8);
	//buy_button->setScaleY(visibleSize.height / buy_button->getContentSize().height / 6);
	buy_button->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.1));
	addChild(buy_button, 3, "buy_button");
	if (root["skill"][0][0]["bought"].asBool() || 
		config[0][0]["cost"].asInt() > root["holy_sugar"].asInt()){
		buy_button->setEnabled(false);
	}
	//add label buy
	Label* buy_label = Label::createWithTTF(((AppDelegate*)Application::getInstance())->getConfig()
		["buttons"]["buy"][language].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 35));
	buy_label->setColor(Color3B::ORANGE);
	buy_label->enableOutline(Color4B::BLACK, 2);
	buy_label->setAlignment(TextHAlignment::CENTER);
	buy_label->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.12));
	addChild(buy_label, 3, "buy_label");

	//skill_cost + sprite under buy_label
	Sprite* sugar_sprite2 = Sprite::create("res/buttons/holy_sugar.png");
	sugar_sprite2->setScale(visibleSize.width / sugar_sprite2->getContentSize().width / 35);
	sugar_sprite2->setPosition(Vec2(visibleSize.width / 8 - visibleSize.width / 50, visibleSize.height * 0.07));
	addChild(sugar_sprite2, 3, "sugar_sprite2");
	
	Label* skill_cost = Label::createWithTTF("X " + config[0][0]["cost"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 45));
	skill_cost->setColor(Color3B::BLACK);
	skill_cost->enableOutline(Color4B::BLACK, 2);
	skill_cost->setPosition(Vec2(visibleSize.width / 8 + visibleSize.width / 50, sugar_sprite2->getPosition().y - visibleSize.height * 0.005));
	if (config[0][0]["cost"].asInt() > root["holy_sugar"].asInt()) {
		skill_cost->setColor(Color3B::RED);
	}
	addChild(skill_cost, 3, "skill_cost");

	//total number of skill tiers
	int tier_size = config.size();
	// Create the scrollview by horizontal
	ui::ScrollView* scrollView = ui::ScrollView::create();
	scrollView->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	scrollView->setContentSize(Size(visibleSize.width * 3 / 4, visibleSize.height));
	scrollView->setPosition(Vec2(visibleSize.width / 4, 0));
	scrollView->setInnerContainerSize(Size(tier_size * visibleSize.width / 6 + visibleSize.width / 20, visibleSize.height));
	addChild(scrollView, 1, "scrollView");

	//add mask to "disable" buttons
	cocos2d::ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(mask, 3, "mask");
	mask->setVisible(false);

	mask->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			removeChildByName("buy_layout");
			getChildByName("mask")->setVisible(false);
		}
	});

	//generating skills' button
	for (unsigned int i(0); i < config.size(); ++i) {
		for (unsigned int j(0); j < config[i].size(); ++j){
			set_skill(i, j);
			set_dependancy(i, j);
			auto a = config[i][j];
		};
	};

	//current selected skill
	Sprite* selected_skill = Sprite::create("res/buttons/yellow.png");
	selected_skill->setPosition(getChildByName("scrollView")->getChildByName("0_0")->getPosition());
	selected_skill->setScaleX(getChildByName("scrollView")->getChildByName("0_0")->getChildByName("skill")->getContentSize().width *
		getChildByName("scrollView")->getChildByName("0_0")->getChildByName("skill")->getScaleX() /
		selected_skill->getContentSize().width * 1.15);
	selected_skill->setScaleY(getChildByName("scrollView")->getChildByName("0_0")->getChildByName("skill")->getContentSize().height *
		getChildByName("scrollView")->getChildByName("0_0")->getChildByName("skill")->getScaleY() /
		selected_skill->getContentSize().height * 1.15);
	scrollView->addChild(selected_skill, 1, "selected_skill");





	//buy_skill + confirmation message
	buy_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			cocos2d::ui::Layout* buy_layout = ui::Layout::create();
			addChild(buy_layout, 3, "buy_layout");

			//buy confirmation 
			cocos2d::ui::Button* buy_info = ui::Button::create("res/buttons/centralMenuPanel2.png");
			buy_info->setScaleX(visibleSize.width / buy_info->getContentSize().width * 0.4);
			buy_info->setScaleY(visibleSize.height / buy_info->getContentSize().height * 0.5);
			buy_info->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
			buy_info->setZoomScale(0.f);
			buy_layout->addChild(buy_info, 3, "buy_info");

			Label* buy_confirmation = Label::createWithTTF("Do you want to buy the skill ?", "fonts/LICABOLD.ttf", 
				round(buy_info->getContentSize().width * buy_info->getScaleX() / 11));
			buy_confirmation->setColor(Color3B::BLACK);
			buy_confirmation->setDimensions(buy_info->getContentSize().width * buy_info->getScaleX() * 0.8,
				buy_info->getContentSize().height * buy_info->getScaleY() * 0.3);
			buy_confirmation->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + buy_info->getContentSize().height * buy_info->getScaleY() * 0.25));
			buy_layout->addChild(buy_confirmation, 3, "buy_confirmation");

			Label* buy_confirmation_skill_name = Label::createWithTTF("", "fonts/LICABOLD.ttf", 
				round(buy_info->getContentSize().width * buy_info->getScaleX() / 11));
			buy_confirmation_skill_name->setColor(Color3B::ORANGE);
			buy_confirmation_skill_name->enableOutline(Color4B::BLACK, 2);
			buy_confirmation_skill_name->setAlignment(TextHAlignment::CENTER);
			buy_confirmation_skill_name->setDimensions(buy_info->getContentSize().width * buy_info->getScaleX() * 0.6, 
				buy_info->getContentSize().height * buy_info->getScaleY() * 0.3);
			buy_confirmation_skill_name->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 
				buy_info->getContentSize().height * buy_info->getScaleY() * 0.1));
			((Label*)buy_confirmation_skill_name)->setString(((Label*)getChildByName("skill_name"))->getString());
			buy_layout->addChild(buy_confirmation_skill_name, 3, "buy_confirmation_skill_name");

			cocos2d::ui::Button* yes = ui::Button::create("res/buttons/yellow_button.png");
			yes->setTitleText("BUY");
			yes->setTitleFontName("fonts/LICABOLD.ttf");
			yes->setTitleFontSize(70.f * visibleSize.width / 1280);
			Label* yes_label = yes->getTitleRenderer();
			yes_label->setColor(Color3B::WHITE);
			yes_label->enableOutline(Color4B::BLACK, 2);
			yes_label->setPosition(yes->getContentSize() / 2);
			yes->setScale(buy_info->getContentSize().width*buy_info->getScaleX() * 0.4 / yes->getContentSize().width);
			yes->setPosition(Vec2(visibleSize.width / 2 - buy_info->getContentSize().width*buy_info->getScaleX() / 4,
				visibleSize.height / 2 - buy_info->getContentSize().height*buy_info->getScaleY() / 2 -
				yes->getContentSize().height*yes->getScaleY() * 0.41));
			Sprite* yes_shadow = Sprite::create("res/buttons/shadow_button.png");
			yes_shadow->setScale(yes->getScale());
			yes_shadow->setPosition(yes->getPosition());
			buy_layout->addChild(yes_shadow, -1);
			buy_layout->addChild(yes, 3, "yes");

			cocos2d::ui::Button* cancel = ui::Button::create("res/buttons/red_button.png");
			cancel->setTitleText("Cancel");
			cancel->setTitleFontName("fonts/LICABOLD.ttf");
			cancel->setTitleFontSize(70.f * visibleSize.width / 1280);
			Label* cancel_label = cancel->getTitleRenderer();
			cancel_label->setColor(Color3B::WHITE);
			cancel_label->enableOutline(Color4B::BLACK, 2);
			cancel_label->setPosition(cancel->getContentSize() / 2);
			cancel->setScale(buy_info->getContentSize().width*buy_info->getScaleX() * 0.4 / cancel->getContentSize().width);
			cancel->setPosition(Vec2(visibleSize.width / 2 + buy_info->getContentSize().width*buy_info->getScaleX() / 4,
				visibleSize.height / 2 - buy_info->getContentSize().height*buy_info->getScaleY() / 2 -
				cancel->getContentSize().height*cancel->getScaleY() * 0.41));
			Sprite* cancel_shadow = Sprite::create("res/buttons/shadow_button.png");
			cancel_shadow->setScale(cancel->getScale());
			cancel_shadow->setPosition(cancel->getPosition());
			buy_layout->addChild(cancel_shadow, -1);
			buy_layout->addChild(cancel, 3, "cancel");

			getChildByName("mask")->setVisible(!getChildByName("mask")->isVisible());

			cancel->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
				if (type == ui::Widget::TouchEventType::ENDED) {
					removeChildByName("buy_layout");
					getChildByName("mask")->setVisible(false);
				}
			});
			
			yes->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
				if (type == ui::Widget::TouchEventType::ENDED) {
					root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file

					int is_skill_bought = root["skill"][selected_tier_id][selected_skill_id]["bought"].asBool();

					//have enough holy_sugar ???
					if (root["holy_sugar"].asInt() >= config[selected_tier_id][selected_skill_id]["cost"].asInt()){
						//is skill already bought
						if (is_skill_bought == 1){
							return;
						}
						if (is_skill_bought == 0){
							root["skill"][selected_tier_id][selected_skill_id]["bought"] = true;

							((cocos2d::ui::Button*)getChildByName("buy_button"))->setEnabled(false);
							removeChildByName("buy_layout");
							getChildByName("mask")->setVisible(false);

							//update total amount holy_sugar
							root["holy_sugar"] = root["holy_sugar"].asInt() - config[selected_tier_id][selected_skill_id]["cost"].asInt();
							((Label*)getChildByName("sugar_amount"))->setString("X " + root["holy_sugar"].asString());
							((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(root);
							((AppDelegate*)Application::getInstance())->getConfigClass()->save();

							//update skill panels, check box
							getChildByName("scrollView")->getChildByName(Value(selected_tier_id).asString() + "_" + Value(selected_skill_id).asString())->getChildByName("skill")->getChildByName("skill_bought")->setVisible(true);
							skill_update(selected_tier_id, selected_skill_id, (ui::Button*) getChildByName("scrollView")->getChildByName(Value(selected_tier_id).asString() + "_" + Value(selected_skill_id).asString())->getChildByName("skill"));
							
							for (unsigned int i(0); i < config[selected_tier_id + 1].size(); ++i){
							skill_update(selected_tier_id + 1, i, (ui::Button*) getChildByName("scrollView")->getChildByName(Value(selected_tier_id +1).asString() + "_" + Value((int)i).asString())->getChildByName("skill"));
							}
						}
					}
					else {
						return;
					}
				}
			});
		}
	});

	return true;
}

void Skills::onEnterTransitionDidFinish() {
	root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file
	((Label*)getChildByName("sugar_amount"))->setString("x " + root["holy_sugar"].asString());
}

void Skills::set_skill(int tier_id, int skill_id){ //create skill button
	Size visibleSize = Director::getInstance()->getVisibleSize();

	//create a layout which contains sprites (skill, text)
	cocos2d::ui::Layout* layout = ui::Layout::create();
	getChildByName("scrollView")->addChild(layout, 2, Value(tier_id).asString() + "_" + Value(skill_id).asString());

	//add skill to layout
	ui::Button* skill = ui::Button::create("res/buttons/Skills/skill_disabled.png");

	//check mark on bought skill
	Sprite* skill_bought = Sprite::create("res/buttons/validate.png");
	skill_bought->setAnchorPoint(Vec2(1, 1));
	skill_bought->setScale(skill->getContentSize().width / 5 / skill_bought->getContentSize().width);
	skill->addChild(skill_bought, 3, "skill_bought");
	skill_bought->setVisible(false);
	if (root["skill"][tier_id][skill_id]["bought"].asBool()){
		skill_bought->setVisible(true);
	}
	skill_update(tier_id, skill_id, skill);

	layout->addChild(skill, 2, "skill");

	skill->addTouchEventListener([&, tier_id, skill_id, skill](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Size visibleSize = Director::getInstance()->getVisibleSize();
			//update skill info and the selection rectangular		
			((Label*)getChildByName("skill_cost"))->setString("X " + config[tier_id][skill_id]["cost"].asString());
			((Label*)getChildByName("skill_name"))->setString(config[tier_id][skill_id]["name_" +
				((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage()].asString());
			((Label*)getChildByName("skill_description"))->setString(config[tier_id][skill_id]["description_" +
				((AppDelegate*)Application::getInstance())->getConfigClass()->getLanguage()].asString());
			((Label*)getChildByName("skill_description"))->setPosition(getChildByName("skill_description")->getPosition().x,
				getChildByName("skill_name")->getPosition().y - getChildByName("skill_name")->getContentSize().height - 
				visibleSize.height / 25);

			getChildByName("scrollView")->getChildByName("selected_skill")->setScaleX(skill->getContentSize().width * skill->getScaleX() /
				getChildByName("scrollView")->getChildByName("selected_skill")->getContentSize().width * 1.15);
			getChildByName("scrollView")->getChildByName("selected_skill")->setScaleY(skill->getContentSize().height * skill->getScaleY() /
				getChildByName("scrollView")->getChildByName("selected_skill")->getContentSize().height * 1.15);
			getChildByName("scrollView")->getChildByName("selected_skill")->setPosition(
				getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getPosition());
			selected_tier_id = tier_id;
			selected_skill_id = skill_id;

			//reset buy_button to enable
			((ui::Button*)getChildByName("buy_button"))->setEnabled(true);
			//test if skill can be bought (enough sugar, unlocked, prerequisite...)
			if (root["holy_sugar"].asInt() < config[tier_id][skill_id]["cost"].asInt()){
				((Label*)getChildByName("skill_cost"))->setColor(Color3B::RED);
				((ui::Button*)getChildByName("buy_button"))->setEnabled(false);
			}
			else{
				((Label*)getChildByName("skill_cost"))->setColor(Color3B::BLACK);
				if (root["level"].asInt() < config[tier_id][skill_id]["level_unlocked"].asInt()){
					((ui::Button*)getChildByName("buy_button"))->setEnabled(false);
				}
				else{
					if (root["skill"][tier_id][skill_id]["bought"].asBool()){
						((ui::Button*)getChildByName("buy_button"))->setEnabled(false);
					}
					else{
						bool enable1(true);
						for (unsigned i(0); i < config[tier_id][skill_id]["dependance"].size(); ++i){
							int j = config[tier_id][skill_id]["dependance"][i].asInt();
							if (j != -1) {
								if (!root["skill"][tier_id - 1][j]["bought"].asBool()) {
									enable1 = false;
								}

								if (enable1 == false) {
									((ui::Button*)getChildByName("buy_button"))->setEnabled(false);
								}
							}
						}
					}
				}
			}
		}
	});
	
	//positionning skills tree and buy button
	//total number of skills in tier_id
	int skill_size = config[tier_id].size(); 
	if (tier_id == 0){
		layout->setPosition(Vec2(visibleSize.width / 10, visibleSize.height / 2));

	}
	else{
		if (config[tier_id].size() == 0){
			return;
		}
		if (config[tier_id].size() == 1){
			Vec2 pos_previous_skill = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_0")->getPosition();
			layout->setPosition(Vec2(visibleSize.width / 6 + pos_previous_skill.x, visibleSize.height / 2));
		}
		if (config[tier_id].size() == 2){
			Vec2 pos_previous_skill = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_0")->getPosition();
			layout->setPosition(Vec2(visibleSize.width / 6 + pos_previous_skill.x, visibleSize.height * (skill_id + 1) / (skill_size + 1)));
		}
		else {
			Vec2 pos_previous_skill = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_0")->getPosition();
			layout->setPosition(Vec2(visibleSize.width / 6 + pos_previous_skill.x, visibleSize.height * (4 * skill_id + 2) / 12));
		}
	}
}

void Skills::set_dependancy(int tier_id, int skill_id){ // create link between skills
	Size visibleSize = Director::getInstance()->getVisibleSize();

	

	if (config[tier_id][skill_id]["dependance"][0].asInt() == -1) {
		return;
	}
	else {
		unsigned int a = config[tier_id][skill_id]["dependance"].size();
		for (unsigned int i(0); i < a; i++) {
			int b = config[tier_id][skill_id]["dependance"][i].asInt();
			Sprite* link = Sprite::create("res/buttons/link_skills2.png");
			double distance = getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getPosition().
				distance(getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_" + Value(b).asString())->getPosition());
			double angle = -(getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getPosition() -
				getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_" + Value(b).asString())->getPosition()).getAngle() * 180 / (MATH_PIOVER2 * 2);
			link->setScale(distance/link->getContentSize().width);
			link->setRotation(angle);
			link->setPosition((getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_" + Value(b).asString())->getPosition() + 
				getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getPosition()) / 2);
			getChildByName("scrollView")->addChild(link, 1);
			/*DrawNode* arrow = DrawNode::create();
			Vec2 pos[4];

			//get scrollView then skill's layout "tier_id_skill_id"
			pos[0] = getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getPosition();
			pos[1] = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_" + Value(b).asString())->getPosition();
			pos[2] = getChildByName("scrollView")->getChildByName(Value(tier_id - 1).asString() + "_" + Value(b).asString())->getPosition();
			pos[3] = getChildByName("scrollView")->getChildByName(Value(tier_id).asString() + "_" + Value(skill_id).asString())->getPosition();
			arrow->drawPolygon(pos, 4, Color4F::BLUE, 4, Color4F::BLUE);
			getChildByName("scrollView")->addChild(arrow, 1);*/
		};
	};
}

void Skills::skill_update(int tier_id, int skill_id, ui::Button* skill){ //create skill button
	Size visibleSize = Director::getInstance()->getVisibleSize();

	if (tier_id == 0){ // skill n° 0_0
		skill->loadTextureNormal(config[tier_id][skill_id]["sprite_enabled"].asString());
	}
	if (root["level"].asInt() < config[tier_id][skill_id]["level_unlocked"].asInt()){ // compaign level < level_unlocked then disabled
		skill->loadTextureNormal(config[tier_id][skill_id]["sprite_disabled"].asString());
	}
	else {
		if (config[tier_id][skill_id]["dependance"][0].asInt() == -1){ // skill without dependance then enabled
			skill->loadTextureNormal(config[tier_id][skill_id]["sprite_enabled"].asString());
		}
		else{
			bool enable(true);
			for (unsigned i(0); i < config[tier_id][skill_id]["dependance"].size(); ++i){ // count dependance size
				int j = config[tier_id][skill_id]["dependance"][i].asInt();
				if (!root["skill"][tier_id - 1][j]["bought"].asBool()){ // if one prerequisite is not checked then false
					enable = false;
				}
			}
			if (enable){
				skill->loadTextureNormal(config[tier_id][skill_id]["sprite_enabled"].asString());
			}
			else{
				skill->loadTextureNormal(config[tier_id][skill_id]["sprite_disabled"].asString());
			}
		}
	}
	skill->setScale(visibleSize.width / skill->getContentSize().width * 0.1);
	skill->setPosition(Vec2(0.f, 0.f));
	skill->getChildByName("skill_bought")->setPosition(Vec2(skill->getContentSize().width / 1.05, skill->getContentSize().height / 1.05));
}