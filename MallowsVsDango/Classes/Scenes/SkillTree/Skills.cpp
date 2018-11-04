#include "Skills.h"
#include "../../AppDelegate.h"
#include "../../SceneManager.h"

USING_NS_CC;

bool Skills::init(Config* config) {
	configClass = config;
	if (!Scene::init()) { return false; }
	std::string language = configClass->getSettings()->getLanguage();

	tutorial_running = false;
	c_talent = Json::Value::null;
	c_button = nullptr;
	tutorial = new SkillTutorial(configClass->getSkillTutorialSettings(), this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Json::Value talents = configClass->getConfigValues(Config::ConfigType::TALENT)["talents"]; //load data file
	Json::Value savedSkills = ((AppDelegate*)Application::getInstance())->getSave(); //load save file
	Json::Value buttons = configClass->getConfigValues(Config::ConfigType::BUTTON);

	initSkillTreeProperties(savedSkills, talents);

	addBackground(visibleSize);
	addBlackMask(visibleSize);
	createBackToMainMenuButton(visibleSize);
	createLeftPanelForSkillDescrition(visibleSize, buttons, language, savedSkills);
	createValidationLayout(visibleSize, buttons, language);
	createBuyingButtonLayout(visibleSize, buttons, language);
	createRequirementDescription(visibleSize, configClass);
	initSkills();

	return true;
}

void Skills::createRequirementDescription(cocos2d::Size &visibleSize, Config* configClass)
{
	requirementDescription = RequirementDescription::create(this, visibleSize, configClass);
	requirementDescription->setPosition(Vec2(visibleSize.width / 8, skillDescription->getPosition().y -
		skillDescription->getContentSize().height -
		visibleSize.height / 25));
	addChild(requirementDescription, 3, "requirement_description");
}

void Skills::initSkillTreeProperties(Json::Value &savedSkills, Json::Value &talents)
{
	if (!savedSkills.isMember("holy_sugar")) {
		savedSkills["holy_sugar"] = 0;
	}
	if (!savedSkills.isMember("holy_sugar_spent")) {
		savedSkills["holy_sugar_spent"] = 0;
	}
	// FILLING SAVE FILE WITH VALUES
	Json::Value savedSkillsCopy = savedSkills;
	initializeDefaultSkillTree(savedSkillsCopy, talents);
	updateSkillTreeWithSaveFile(savedSkills, talents, savedSkillsCopy);
	savedSkills = savedSkillsCopy;
	configClass->setSave(savedSkills);
	configClass->save();
}

void Skills::createLeftPanelForSkillDescrition(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language, Json::Value &savedSkills)
{
	Sprite* skill_info = Sprite::create("res/buttons/menupanel5.png");
	addChild(skill_info, 1);
	skill_info->setScaleX(visibleSize.width / skill_info->getContentSize().width / 4);
	skill_info->setScaleY(visibleSize.height / skill_info->getContentSize().height);
	skill_info->setPosition(Vec2(0.f, 0.f));
	skill_info->setAnchorPoint(Vec2(0.f, 0.f));

	// Scene Title
	Label* title = Label::createWithTTF(buttons["talents"][language].asString(),
		"fonts/LICABOLD.ttf", round(visibleSize.width / 20));
	title->setColor(Color3B::YELLOW);
	title->enableOutline(Color4B::BLACK, 2);
	title->setPosition(Vec2(visibleSize.width / 8, visibleSize.height - title->getContentSize().height * title->getScaleY() / 2 -
		visibleSize.height / 25
	));
	addChild(title, 1, "title");

	Sprite* sugar_sprite1 = Sprite::create("res/buttons/holy_sugar.png");
	sugar_sprite1->setScale(visibleSize.width / sugar_sprite1->getContentSize().width / 12);
	sugar_sprite1->setPosition(Vec2(skill_info->getContentSize().width * skill_info->getScaleX() / 3, title->getPosition().y -
		title->getContentSize().height * title->getScaleY() / 2 -
		sugar_sprite1->getContentSize().height * sugar_sprite1->getScaleY() / 2 - visibleSize.height / 40));
	addChild(sugar_sprite1, 1);

	//current sugar info
	Label* sugar_amount = Label::createWithTTF("X " + savedSkills["holy_sugar"].asString(), "fonts/LICABOLD.ttf", round(visibleSize.width / 25));
	sugar_amount->setColor(Color3B::YELLOW);
	sugar_amount->enableOutline(Color4B::BLACK, 2);
	sugar_amount->setPosition(Vec2(skill_info->getContentSize().width * skill_info->getScaleX() / 3, 0) + sugar_sprite1->getPosition());
	addChild(sugar_amount, 1, "sugar_amount");

	//label for setting skill_name
	Label* skill_name = Label::createWithTTF(" ",
		"fonts/LICABOLD.ttf", round(visibleSize.width / 40));
	skill_name->setColor(Color3B::YELLOW);
	skill_name->enableOutline(Color4B::BLACK, 2);
	skill_name->setWidth(visibleSize.width / 5);
	skill_name->setHorizontalAlignment(TextHAlignment::CENTER);
	skill_name->setVerticalAlignment(TextVAlignment::TOP);
	skill_name->setPosition(Vec2(visibleSize.width / 8, sugar_sprite1->getPosition().y -
		sugar_sprite1->getContentSize().height * sugar_sprite1->getScaleY() / 2 -
		skill_name->getContentSize().height / 2 -
		visibleSize.height / 40
	));
	addChild(skill_name, 1, "skill_name");

	//label for setting skill_description
	skillDescription = Label::createWithTTF(buttons["waiting_talent"][language].asString(),
		"fonts/LICABOLD.ttf", round(visibleSize.width / 55.f));
	skillDescription->setColor(Color3B::BLACK);
	skillDescription->setWidth(visibleSize.width / 5);
	skillDescription->setPosition(Vec2(visibleSize.width / 8, skill_name->getPosition().y - skill_name->getContentSize().height / 2 -
		visibleSize.height / 25));
	skillDescription->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
	skillDescription->setAnchorPoint(Vec2(0.5f, 1.f));
	skillDescription->runAction(RepeatForever::create(Sequence::createWithTwoActions(FadeIn::create(1.f), FadeOut::create(1.f))));
	addChild(skillDescription, 1, "skillDescription");
}

void Skills::createBuyingButtonLayout(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language)
{
	buyingLayout = cocos2d::ui::Layout::create();
	buyingLayout->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.1));
	addChild(buyingLayout, 1, "buyingLayout");

	cocos2d::ui::Button* buy_button = ui::Button::create("res/buttons/shop.png");
	buy_button->setScale(visibleSize.width / buy_button->getContentSize().width / 8);
	buy_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			showValidationPanel();
		}
	});
	buyingLayout->addChild(buy_button, 1, "buy_button");

	//skill_cost + sprite under buy_label
	Sprite* sugar_sprite2 = Sprite::create("res/buttons/holy_sugar.png");
	sugar_sprite2->setScale(visibleSize.width / sugar_sprite2->getContentSize().width / 20);
	sugar_sprite2->setPosition(Vec2(-buy_button->getContentSize().width * buy_button->getScaleX() / 5, 0));
	buyingLayout->addChild(sugar_sprite2, 1, "sugar_sprite2");

	Label* skill_cost = Label::createWithTTF("5", "fonts/LICABOLD.ttf", round(visibleSize.width / 30));
	skill_cost->setColor(Color3B::WHITE);
	skill_cost->enableOutline(Color4B::BLACK, 1);
	skill_cost->setPosition(Vec2(buy_button->getContentSize().width * buy_button->getScaleX() / 5, 0));
	buyingLayout->addChild(skill_cost, 1, "skill_cost");
	buyingLayout->setVisible(false);

	// add unlocked description
	std::string txt = buttons["skill_bought"][language].asString();
	txt[0] = toupper(txt[0]);
	Label* unlocked_txt = Label::createWithTTF(txt, "fonts/LICABOLD.ttf", round(visibleSize.width / 25));
	unlocked_txt->setColor(Color3B::YELLOW);
	unlocked_txt->enableOutline(Color4B::BLACK, 1);
	unlocked_txt->setPosition(buyingLayout->getPosition());
	addChild(unlocked_txt, 1, "unlocked_txt");
	unlocked_txt->setVisible(false);
}

void Skills::createValidationLayout(cocos2d::Size &visibleSize, Json::Value &buttons, std::string &language)
{
	auto validate_buy = ui::Layout::create();
	addChild(validate_buy, 2, "validate_buy");
	ui::Button* panel = ui::Button::create("res/buttons/centralMenuPanel2.png");
	panel->setZoomScale(0);
	validate_buy->addChild(panel, 1, "panel");
	panel->setScale9Enabled(true);
	panel->setScale(0.4*visibleSize.width / panel->getContentSize().width);

	validate_buy->setPosition(Vec2(visibleSize.width / 2, visibleSize.height +
		getChildByName("validate_buy")->getChildByName("panel")->getContentSize().height *
		getChildByName("validate_buy")->getChildByName("panel")->getScaleY()));

	//add closing button
	auto close = ui::Button::create("res/buttons/close2.png");
	close->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			hideValidationPanel();
		}
	});
	close->setScale(panel->getContentSize().width*panel->getScaleX() / 8 / close->getContentSize().width);
	close->setPosition(Vec2(panel->getContentSize().width*panel->getScaleX() / 2,
		panel->getContentSize().height*panel->getScaleY() / 2));
	Sprite* close_shadow = Sprite::create("res/buttons/close2_shadow.png");
	close_shadow->setScale(close->getScale() * 1.05);
	close_shadow->setPosition(close->getPosition());
	validate_buy->addChild(close_shadow, -1);
	validate_buy->addChild(close, 5, "close");

	//add fixed validation text
	Label* confirm_text = Label::createWithTTF(buttons["confirm"][language].asString(),
		"fonts/LICABOLD.ttf", round(visibleSize.width / 30.f));
	confirm_text->setColor(Color3B::YELLOW);
	confirm_text->enableOutline(Color4B::BLACK, 2);
	confirm_text->setPosition(Vec2(0, panel->getContentSize().height * panel->getScaleY() / 3 -
		visibleSize.height / 25));
	confirm_text->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
	validate_buy->addChild(confirm_text, 1, "confirm_text");

	//add buy validation button
	auto validate_button = ui::Button::create("res/buttons/yellow_button.png");
	validate_button->setTitleText(configClass->getConfigValues(Config::ConfigType::BUTTON)
		["validate"][language].asString());
	validate_button->setTitleFontName("fonts/LICABOLD.ttf");
	validate_button->setTitleFontSize(60.f);
	Label* validate_label = validate_button->getTitleRenderer();
	validate_label->setColor(Color3B::WHITE);
	validate_label->enableOutline(Color4B::BLACK, 2);
	validate_label->setPosition(validate_button->getContentSize() / 2);
	validate_button->setScale(panel->getContentSize().width*panel->getScaleX() / 2 / validate_button->getContentSize().width);
	validate_button->setPosition(Vec2(0,
		-panel->getContentSize().height*panel->getScaleY() / 2));
	validate_button->addTouchEventListener([&](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file
			save_root["talents"][getSavedSkillPosFromID(c_talent["id"].asInt())]["bought"] = true;
			save_root["holy_sugar"] = save_root["holy_sugar"].asInt() - c_talent["cost"].asInt();
			save_root["holy_sugar_spent"] = save_root["holy_sugar_spent"].asInt() + c_talent["cost"].asInt();
			((Label*)getChildByName("sugar_amount"))->setString("X " + save_root["holy_sugar"].asString());
			configClass->setSave(save_root);
			configClass->save();

			//update skill panels, check box
			removeChildByName("talent_page");
			initSkills();
			selectSkill(c_talent["id"].asInt());
			hideValidationPanel();
		}
	});
	Sprite* validate_shadow = Sprite::create("res/buttons/shadow_button.png");
	validate_shadow->setScale(validate_button->getScale());
	validate_shadow->setPosition(validate_button->getPosition());
	validate_buy->addChild(validate_shadow, -1);
	validate_buy->addChild(validate_button, 5, "validate_button");
}

void Skills::addBlackMask(cocos2d::Size &visibleSize)
{
	addChild(ui::Layout::create(), 2, "black_mask");
	ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("black_mask")->addChild(mask, 4);
	getChildByName("black_mask")->setVisible(false);
}

void Skills::createBackToMainMenuButton(cocos2d::Size &visibleSize)
{
	cocos2d::ui::Button* back = ui::Button::create("res/buttons/restart_button.png");
	Sprite* back_wood = Sprite::create("res/buttons/close2_shadow.png");

	back->addChild(back_wood, -1);
	addChild(back, 2);
	back->setScale(visibleSize.width / back->getContentSize().width / 15);
	back->setAnchorPoint(Vec2(1.f, 1.f));
	back_wood->setScale(0.98f);
	back_wood->setPosition(Vec2(back->getContentSize().width / 2,
		back->getContentSize().height / 2));
	back->setPosition(Vec2(visibleSize.width, visibleSize.height));
	back->addTouchEventListener([back](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			SceneManager::getInstance()->setScene(SceneFactory::LEVELS);
		}
	});
}

void Skills::addBackground(cocos2d::Size &visibleSize)
{
	Sprite* loading_background = Sprite::create("res/background/space.png");
	loading_background->setPosition(Vec2(visibleSize.width * 5 / 8, visibleSize.height / 2));
	loading_background->setScale(visibleSize.height / loading_background->getContentSize().height);
	addChild(loading_background);
}

void Skills::updateSkillTreeWithSaveFile(Json::Value &save_root, Json::Value &talents, Json::Value &root2)
{
	for (unsigned int i(0); i < save_root["talents"].size(); ++i) {
		if (save_root["talents"][i].isMember("id")) {
			if (talents[i]["id"].asInt() != save_root["talents"][i]["id"].asInt()) {
				for (unsigned int k(0); k < talents.size(); ++k) {
					if (talents[k]["id"].asInt() == save_root["talents"][i]["id"].asInt()) {
						root2["talents"][k]["id"] = talents[k]["id"].asInt();
						root2["talents"][k]["bought"] = save_root["talents"][i]["bought"].asBool();
					}
				}
			}
			else {
				root2["talents"][i]["id"] = talents[i]["id"].asInt();
				root2["talents"][i]["bought"] = save_root["talents"][i]["bought"].asBool();
			}
		}
	}
}

void Skills::initializeDefaultSkillTree(Json::Value &root2, Json::Value &talents)
{
	root2["talents"] = {};
	for (unsigned int k(0); k < talents.size(); ++k) {
		auto conf = talents[k]["id"].asInt();
		root2["talents"][k]["id"] = talents[k]["id"].asInt();
		root2["talents"][k]["bought"] = false;
	}
}

Skills * Skills::create(Config * config)
{
	Skills* skillScene = new Skills();

	if (skillScene->init(config))
	{
		skillScene->autorelease();
		return skillScene;
	}
	else
	{
		delete skillScene;
		return NULL;
	}
}

Json::Value Skills::getSkillFromID(int id) {
	Json::Value talents = ((AppDelegate*)Application::getInstance())->getConfigClass()->getConfigValues(Config::ConfigType::TALENT)["talents"]; //load data file
	for (auto& talent : talents) {
		if (talent["id"] == id) {
			return talent;
		}
	}
	return Json::Value::null;
}

Json::Value Skills::getSavedSkillFromID(int id) {
	Json::Value talents = ((AppDelegate*)Application::getInstance())->getConfigClass()->getSaveValues()["talents"]; //load data file
	for (auto& talent : talents) {
		if (talent["id"] == id) {
			return talent;
		}
	}
	return Json::Value::null;
}

int Skills::getSavedSkillPosFromID(int id) {
	Json::Value talents = configClass->getSaveValues()["talents"]; //load data file
	for (unsigned int i(0); i < talents.size(); ++i) {
		if (talents[i]["id"] == id) {
			return i;
		}
	}
	return -1;
}

void Skills::initSkills() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Json::Value talents = configClass->getConfigValues(Config::ConfigType::TALENT)["talents"]; //load data file
	Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file
	std::string language = configClass->getSettings()->getLanguage();
	Json::Value buttons = configClass->getConfigValues(Config::ConfigType::BUTTON);

	// create Page view for talents
	ui::PageView* talent_pages = ui::PageView::create();
	talent_pages->setContentSize(Size(visibleSize.width * 3 / 4, visibleSize.height));
	talent_pages->setPosition(Vec2(visibleSize.width * 5 / 8 - talent_pages->getContentSize().width / 2,
		visibleSize.height / 2 - talent_pages->getContentSize().height / 2));

	int row = 0;
	int column = 0;
	int max_row = 3;
	int max_column = 4;
	ui::Layout* c_page = nullptr;

	for (auto& talent : talents) {
		// if current page is not created, let's create it
		if (c_page == nullptr) {
			c_page = ui::Layout::create();
			talent_pages->addPage(c_page);
		}

		// create the button for the specific talent
		float buttonScale = talent_pages->getContentSize().width / (1.75 * max_row);
		cocos2d::ui::Button* skillButton = createSkillButton(save_root, talent, buttonScale);
		float position_x = column * talent_pages->getContentSize().width * 1 / max_column + talent_pages->getContentSize().width * 1 / (2 * max_column);
		float position_y = -row * talent_pages->getContentSize().height * 1 / max_row + talent_pages->getContentSize().height * (2 * max_row - 1) / (2 * max_row);
		skillButton->setPosition(Vec2(position_x, position_y));
		talentButtons[talent["id"].asInt()] = skillButton;
		c_page->addChild(skillButton);
		++row;
		if (row >= max_row) {
			++column;
			row = 0;
			if (column >= max_column) {
				column = 0;
				c_page = nullptr;
			}
		}

	}
	addChild(talent_pages, 1, "talent_page");
}

cocos2d::ui::Button* Skills::createSkillButton(Json::Value &save_root, Json::Value & talent, float buttonScale)
{
	std::string sprite_name = getSkillSpriteName(save_root, talent);
	ui::Button* skillButton = ui::Button::create(sprite_name);
	skillButton->setEnabled(shouldEnableSkillButton(save_root, talent));
	skillButton->setScale(buttonScale / skillButton->getContentSize().width);

	skillButton->addTouchEventListener([&, talent, skillButton](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED && skillButton->isEnabled()) {
			selectSkill(talent["id"].asInt());
		}
	});
	if (getSavedSkillFromID(talent["id"].asInt())["bought"].asBool()) {
		setTalentBtnBought(skillButton);
	}
	if (c_talent != Json::Value::null && c_talent["id"].asInt() == talent["id"].asInt()) {
		c_button = skillButton;
	}
	return skillButton;
}
bool Skills::shouldEnableSkillButton(Json::Value & save_root, Json::Value & talent)
{
	return save_root["c_level"].asInt() > talent["condition_level"].asInt() && save_root["c_world"].asInt() > talent["condition_world"].asInt();
}

std::string Skills::getSkillSpriteName(Json::Value &save_root, Json::Value & talent) {
	std::string sprite_name = "";
	if (save_root["c_level"].asInt() > talent["condition_level"].asInt() && save_root["c_world"].asInt() > talent["condition_world"].asInt() &&
		save_root["holy_sugar_spent"].asInt() > talent["condition_sugar"].asInt() && (talent["condition_id"].asInt() < 0 ||
		(talent["condition_id"].asInt() > 0 && getSavedSkillFromID(talent["condition_id"].asInt())["bought"].asBool()))) {
		sprite_name = talent["sprite_enabled"].asString();
	}
	else {
		sprite_name = talent["sprite_disabled"].asString();
	}
	return sprite_name;
}

void Skills::setTalentBtnBought(ui::Button* btn) {
	Sprite* validate = Sprite::create("res/buttons/validate.png");
	validate->setScale(btn->getContentSize().width * 0.4 / validate->getContentSize().width);
	validate->setPosition(Vec2(btn->getContentSize().width * 5 / 6, validate->getContentSize().height * validate->getScaleY() / 3));
	btn->addChild(validate);
}

void Skills::selectSkill(int id) {
	Json::Value talent = getSkillFromID(id);
	ui::Button* talent_btn = talentButtons[id];
	
	std::string language = configClass->getSettings()->getLanguage();
	Json::Value talents = configClass->getConfigValues(Config::ConfigType::TALENT)["talents"]; //load data file
	Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file
	Json::Value buttons = configClass->getConfigValues(Config::ConfigType::BUTTON);
	Size visibleSize = Director::getInstance()->getVisibleSize();

	((Label*)getChildByName("skill_name"))->setString(talent["name_" + language].asString());

	updateSkillDescription(talent, language);
	updateBuyingCapacity(talent, save_root);
	requirementDescription->update(language, talent, save_root);
	requirementDescription->setPosition(Vec2(visibleSize.width / 8, skillDescription->getPosition().y - skillDescription->getContentSize().height -
		visibleSize.height / 25));
	updateBuyingLayoutVisibility(talent);
	removeSelectedEffectFromSkillButton();
	c_button = talent_btn;
	c_talent = talent;
	addSelectedEffectToSkillButton(talent_btn);
}

cocos2d::ui::Button * Skills::getTalentButton(int index)
{
	return talentButtons[index];
}

cocos2d::ui::Button * Skills::getBuyButton()
{
	return ((cocos2d::ui::Button*)buyingLayout->getChildByName("buy_button"));
}

void Skills::removeSelectedEffectFromSkillButton()
{
	if (c_button != nullptr) {
		c_button->removeChildByName("yellow");
	}
}

void Skills::addSelectedEffectToSkillButton(cocos2d::ui::Button * talent_btn)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Sprite* selected = Sprite::create("res/buttons/Skills/selected.png");
	selected->setScale((talent_btn->getScale() * talent_btn->getContentSize().width + visibleSize.width / 20) / selected->getContentSize().width);
	selected->setPosition(Vec2(talent_btn->getContentSize().width / 2, talent_btn->getContentSize().height / 2));
	talent_btn->addChild(selected, -1, "yellow");
}

void Skills::updateBuyingLayoutVisibility(Json::Value &talent)
{
	if (getSavedSkillFromID(talent["id"].asInt())["bought"].asBool()) {
		buyingLayout->setVisible(false);
		getChildByName("unlocked_txt")->setVisible(true);
	}
	else {
		buyingLayout->setVisible(true);
		getChildByName("unlocked_txt")->setVisible(false);
	}
}

void Skills::updateSkillDescription(Json::Value &talent, std::string &language)
{
	skillDescription->stopAllActions();
	skillDescription->setOpacity(255);
	skillDescription->setString(talent["description_" + language].asString());
}

void Skills::updateBuyingCapacity(Json::Value &talent, Json::Value &save_root)
{
	if (!hasEnoughHolySugar(talent, save_root)) {
		((Label*)buyingLayout->getChildByName("skill_cost"))->setColor(Color3B::RED);
		((ui::Button*)buyingLayout->getChildByName("buy_button"))->setEnabled(false);
	}
	else {
		((Label*)buyingLayout->getChildByName("skill_cost"))->setColor(Color3B::WHITE);
		((ui::Button*)buyingLayout->getChildByName("buy_button"))->setEnabled(true);
	}
	((Label*)buyingLayout->getChildByName("skill_cost"))->setString(talent["cost"].asString());
}

bool Skills::hasEnoughHolySugar(Json::Value &talent, Json::Value &save_root)
{
	return talent["cost"].asInt() < save_root["holy_sugar"].asInt();
}

void Skills::hideValidationPanel() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto* hideAction = EaseBackIn::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height +
		getChildByName("validate_buy")->getChildByName("panel")->getContentSize().width *
		getChildByName("validate_buy")->getChildByName("panel")->getScaleY() * 0.6)));
	getChildByName("validate_buy")->runAction(hideAction);
	getChildByName("black_mask")->setVisible(false);
}

void Skills::showValidationPanel() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	std::string language = configClass->getSettings()->getLanguage();
	Json::Value talents = configClass->getConfigValues(Config::ConfigType::TALENT)["talents"]; //load data file
	Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave(); //load save file
	Json::Value buttons = configClass->getConfigValues(Config::ConfigType::BUTTON);
	Node* validate_buy = getChildByName("validate_buy");
	Node* panel = getChildByName("validate_buy")->getChildByName("panel");
	getChildByName("black_mask")->setVisible(true);

	if (validate_buy->getChildByName("validation_text") != nullptr) {
		validate_buy->removeChildByName("validation_text");
	}


	ui::RichText* validation_text = ui::RichText::create();
	validation_text->setContentSize(Size(panel->getContentSize().width * panel->getScaleX() * 0.85,
		panel->getContentSize().height * panel->getScaleY() * 2 / 3));
	validation_text->setPosition(Vec2(0, -validation_text->getContentSize().height / 2));
	validation_text->ignoreContentAdaptWithSize(false);
	validate_buy->addChild(validation_text, 1, "validation_text");
	int font_size = round(visibleSize.width / 35.f);
	validation_text->pushBackElement(ui::RichElementText::create(1, Color3B::BLACK, 255, buttons["validate_part_1"][language].asString(),
		"fonts/LICABOLD.ttf", font_size));
	validation_text->pushBackElement(ui::RichElementText::create(2, Color3B::YELLOW, 255, c_talent["name_" + language].asString(),
		"fonts/LICABOLD.ttf", font_size));
	validation_text->pushBackElement(ui::RichElementText::create(3, Color3B::BLACK, 255, buttons["validate_part_2"][language].asString(),
		"fonts/LICABOLD.ttf", font_size));
	validation_text->pushBackElement(ui::RichElementText::create(4, Color3B::YELLOW, 255, c_talent["cost"].asString(),
		"fonts/LICABOLD.ttf", font_size));
	validation_text->pushBackElement(ui::RichElementText::create(5, Color3B::BLACK, 255, buttons["validate_part_3"][language].asString(),
		"fonts/LICABOLD.ttf", font_size));
	auto* showAction = EaseBackOut::create(MoveTo::create(0.5f, Vec2(visibleSize.width / 2, visibleSize.height / 2)));
	validate_buy->runAction(showAction);
}

void Skills::update(float dt) {
	tutorial->update(dt);
}

void Skills::onEnterTransitionDidFinish() {
	removeChildByName("talent_page");
	initSkills();
	((Label*)getChildByName("sugar_amount"))->setString("x " + ((AppDelegate*)Application::getInstance())->getSave()["holy_sugar"].asString());
	scheduleUpdate();
}