#include "LevelEditor.h"
#include "../Config/Config.h"
#include "../AppDelegate.h"
#include "../SceneManager.h"
#include "../Level/Cell.h"
#include <cstdio>      /* printf */
#include <cmath>
#include "../Config/NetworkController.h"
#include <ctime>


USING_NS_CC;

bool LevelEditor::init(){
	if (!Scene::init()){ return false; }

	initialized = true;
	textfield_selected = nullptr;
	enemy_checkbox = nullptr;
	enemy_level = -1;
	enemy_name = "";
	current_wave = -1;
	enemies_id = 0;
	wave_id = 0;
	c_enemy_id = -1;
	enemy_path = -1;
	level_settings = Json::Value();
	t_level_settings = Json::Value();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	menuHidden = false;
	path_selected = -1;
	move_by_cell = true;
	selecting_cell = false;
	idle_timer = 0;
	generator = new DangoGenerator();

	Label* file_saved_label = Label::createWithTTF("Level Saved", "fonts/LICABOLD.ttf", 54.f * visibleSize.width / 1280.f);
	file_saved_label->setColor(Color3B::ORANGE);
	file_saved_label->enableOutline(Color4B::BLACK, 2.f);
	file_saved_label->setPosition(visibleSize.width * 5 / 8, visibleSize.height * 1 / 10);
	file_saved_label->setOpacity(0.f);
	addChild(file_saved_label, 5, "level_saved_label");

	// Black mask to prevent any action during selection of enemy or dialogue or anything else like this.
	addChild(ui::Layout::create(), 2, "black_mask");
	ui::Button* mask = ui::Button::create("res/buttons/mask.png");
	mask->setScaleX(visibleSize.width / mask->getContentSize().width);
	mask->setScaleY(visibleSize.height / mask->getContentSize().height);
	mask->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("black_mask")->addChild(mask);
	getChildByName("black_mask")->setVisible(false);

	// Instanciation du bouton de retour à la selection des niveaux
	auto return_to_game_button = ui::Button::create("res/buttons/back.png");
	return_to_game_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			SceneManager::getInstance()->setScene(SceneManager::LEVELS);
		}
	});
	return_to_game_button->setPosition(Vec2(visibleSize.width, visibleSize.height));
	return_to_game_button->setAnchorPoint(Vec2(1, 1));
	return_to_game_button->setEnabled(true);
	return_to_game_button->setScale(visibleSize.width / 15 / return_to_game_button->getContentSize().width);
	addChild(return_to_game_button, 2, "back");

	addChild(Layer::create(), 1, "level");
	getChildByName("level")->addChild(Layer::create(), 0, "backgrounds");
	getChildByName("level")->addChild(Layer::create(), 0, "objects");
	getChildByName("level")->addChild(Layer::create(), 0, "paths");

	ui::Layout* confirm_deletion = ui::Layout::create();
	addChild(confirm_deletion, 2, "confirm_deletion");
	confirm_deletion->setVisible(false);
	auto button_confirm_deletion = ui::Button::create("res/buttons/buttonDeleteLevel.png", "res/buttons/buttonDeleteLevel.png");
	button_confirm_deletion->addTouchEventListener(CC_CALLBACK_2(LevelEditor::deleteCurrentLevel, this));
	auto button_cancel_deletion = ui::Button::create("res/buttons/buttonCancel.png", "res/buttons/buttonCancel.png");
	button_cancel_deletion->addTouchEventListener(CC_CALLBACK_2(LevelEditor::hideDeleteConfirm, this));
	auto middle_panel_deletion = Sprite::create("res/buttons/centralMenuPanel.png");
	middle_panel_deletion->setScaleY(visibleSize.height / 3 / middle_panel_deletion->getContentSize().height);
	middle_panel_deletion->setScaleX((2 * button_confirm_deletion->getContentSize().width * button_confirm_deletion->getScale() +
		button_cancel_deletion->getContentSize().width * 2 / 5 * button_cancel_deletion->getScale()) /
		middle_panel_deletion->getContentSize().width);
	Label* question = Label::createWithTTF("Do you want to delete the level ?", "fonts/Love Is Complicated Again.ttf", 35);
	question->setColor(Color3B::BLACK);
	question->setAlignment(TextHAlignment::CENTER);
	question->setPosition(Vec2(0,
		0.80*middle_panel_deletion->getContentSize().height * middle_panel_deletion->getScaleY() / 2));
	question->setDimensions(middle_panel_deletion->getContentSize().width * middle_panel_deletion->getScaleX() * 4 / 5,
		middle_panel_deletion->getContentSize().height * middle_panel_deletion->getScaleY() * 2 / 3);
	question->setAnchorPoint(Vec2(0.5, 1));
	button_cancel_deletion->setPosition(Vec2(middle_panel_deletion->getContentSize().width * middle_panel_deletion->getScaleX() / 4,
		-middle_panel_deletion->getContentSize().height * middle_panel_deletion->getScaleY() / 2));
	button_confirm_deletion->setPosition(Vec2(-middle_panel_deletion->getContentSize().width * middle_panel_deletion->getScaleX() / 4,
		-middle_panel_deletion->getContentSize().height * middle_panel_deletion->getScaleY() / 2));

	confirm_deletion->addChild(middle_panel_deletion, 0, "middle_panel_deletion");
	confirm_deletion->addChild(question, 1, "question");
	confirm_deletion->addChild(button_confirm_deletion, 1, "button_confirm_deletion");
	confirm_deletion->addChild(button_cancel_deletion, 1, "button_cancel_deletion");
	
	ui::Layout* select_level = ui::Layout::create();
	addChild(select_level, 2, "select_level");
	select_level->setVisible(false);
	auto load_level = ui::Button::create("res/buttons/buttonLoadLevel.png", "res/buttons/buttonLoadLevel.png", "res/buttons/buttonLoadLevel_disable.png");
	load_level->setEnabled(false);
	load_level->setScale(visibleSize.width / 5 / load_level->getContentSize().width);
	load_level->addTouchEventListener(CC_CALLBACK_2(LevelEditor::loadLevel, this));

	auto button_cancel = ui::Button::create("res/buttons/buttonCancel.png", "res/buttons/buttonCancel.png");
	button_cancel->setScale(visibleSize.width / 5 / button_cancel->getContentSize().width);
	button_cancel->addTouchEventListener(CC_CALLBACK_2(LevelEditor::hideExistingLevels, this));
	auto middle_panel = Sprite::create("res/buttons/centralMenuPanel.png");
	middle_panel->setScaleY(visibleSize.height * 0.7 / middle_panel->getContentSize().height);
	middle_panel->setScaleX((3 * load_level->getContentSize().width * load_level->getScale() + 
		button_cancel->getContentSize().width * 2 / 5 * button_cancel->getScale()) /
		middle_panel->getContentSize().width);
	Label* title = Label::createWithTTF("SELECT A LEVEL", "fonts/Love Is Complicated Again.ttf", 35 * visibleSize.width / 1280);
	title->setColor(Color3B::BLACK);
	title->setPosition(Vec2(0,
		0.80*middle_panel->getContentSize().height * middle_panel->getScaleY() / 2));
	title->setAnchorPoint(Vec2(0.5,1));
	Label* local_label = Label::createWithTTF("Local", "fonts/Love Is Complicated Again.ttf", 30 * visibleSize.width / 1280);
	local_label->setColor(Color3B::BLACK);
	local_label->setPosition(Vec2(-middle_panel->getContentSize().width * middle_panel->getScaleX() / 4,
		title->getPosition().y - title->getContentSize().height));
	local_label->setAnchorPoint(Vec2(0.5, 1));
	Label* server_label = Label::createWithTTF("Online", "fonts/Love Is Complicated Again.ttf", 30 * visibleSize.width / 1280);
	server_label->setColor(Color3B::BLACK);
	server_label->setPosition(Vec2(middle_panel->getContentSize().width * middle_panel->getScaleX() / 4,
		title->getPosition().y - title->getContentSize().height));
	server_label->setAnchorPoint(Vec2(0.5, 1));
	auto list_local_levels = ui::ListView::create();
	list_local_levels->setGravity(ui::ListView::Gravity::CENTER_HORIZONTAL);
	list_local_levels->setAnchorPoint(Vec2(0.5f, 0.5f));
	list_local_levels->setContentSize(Size(middle_panel->getContentSize().width * middle_panel->getScaleX() * 0.42f,
		middle_panel->getContentSize().height * middle_panel->getScaleY() * 0.6f));
	list_local_levels->setPosition(Vec2(-middle_panel->getContentSize().width * middle_panel->getScaleX()/ 4, -local_label->getContentSize().height));
	auto list_server_levels = ui::ListView::create();
	list_server_levels->setGravity(ui::ListView::Gravity::CENTER_HORIZONTAL);
	list_server_levels->setAnchorPoint(Vec2(0.5f, 0.5f));
	list_server_levels->setContentSize(Size(middle_panel->getContentSize().width * middle_panel->getScaleX() * 0.42f,
		middle_panel->getContentSize().height * middle_panel->getScaleY() * 0.6f));
	list_server_levels->setPosition(Vec2(middle_panel->getContentSize().width * middle_panel->getScaleX() / 4, -server_label->getContentSize().height));

	button_cancel->setPosition(Vec2(middle_panel->getContentSize().width * middle_panel->getScaleX() / 4 ,
		-middle_panel->getContentSize().height * middle_panel->getScaleY() / 2));
	load_level->setPosition(Vec2(-middle_panel->getContentSize().width * middle_panel->getScaleX() / 4 ,
		-middle_panel->getContentSize().height * middle_panel->getScaleY() / 2));
	
	select_level->addChild(middle_panel, 0, "middle_panel");
	select_level->addChild(list_local_levels, 1, "list_local_levels");
	select_level->addChild(list_server_levels, 1, "list_server_levels");
	select_level->addChild(load_level, 1, "load_level");
	select_level->addChild(button_cancel, 1, "button_cancel");
	select_level->addChild(title, 1, "title");
	select_level->addChild(local_label, 1, "local_label");
	select_level->addChild(server_label, 1, "server_label");



	ui::Layout* menu_left = ui::Layout::create();
	addChild(menu_left, 1, "menu_left");
	auto panel = ui::Button::create("res/buttons/menupanel3.png", "res/buttons/menupanel3.png");
	panel->addTouchEventListener(CC_CALLBACK_2(LevelEditor::hideShowMenu, this));
	auto button_hide = ui::Button::create("res/buttons/wood_button_hide.png", "res/buttons/wood_button_hide.png");
	button_hide->addTouchEventListener(CC_CALLBACK_2(LevelEditor::hideShowMenu, this));
	button_hide->setScale(visibleSize.width / 30 / button_hide->getContentSize().width);

	menu_left->addChild(panel, 0, "panel");
	menu_left->addChild(button_hide, 0, "button_hide");
	panel->setScaleY(visibleSize.height / panel->getContentSize().height);
	panel->setScaleX(visibleSize.width / panel->getContentSize().width / 4);
	panel->setPosition(Vec2(panel->getContentSize().width * panel->getScaleX() / 2, visibleSize.height / 2));
	button_hide->setPosition(Vec2(visibleSize.width / 4 + button_hide->getContentSize().width * button_hide->getScale() / 2,
		panel->getPosition().y));

	ui::Layout* interface_manager = ui::Layout::create();
	interface_manager->setVisible(false);
	auto zoom_in = ui::Button::create("res/buttons/round_zoomin.png", "res/buttons/round_zoomin_selected.png", "res/buttons/round_zoomin.png");
	zoom_in->addTouchEventListener(CC_CALLBACK_2(LevelEditor::zoomIn, this));
	auto zoom_out = ui::Button::create("res/buttons/round_zoomout.png", "res/buttons/round_zoomout_selected.png", "res/buttons/round_zoomout.png");
	zoom_out->addTouchEventListener(CC_CALLBACK_2(LevelEditor::zoomOut, this));

	auto grid = cocos2d::ui::CheckBox::create("res/buttons/round_grid.png", "res/buttons/round_grid.png",
		"res/buttons/round_grid_hidden.png", "res/buttons/round_grid.png", "res/buttons/round_grid.png");
	grid->addTouchEventListener(CC_CALLBACK_2(LevelEditor::showGrid, this));
	auto lock_cell = cocos2d::ui::CheckBox::create("res/buttons/round_lock_cell.png", "res/buttons/round_lock_cell.png",
		"res/buttons/round_lock_cell_hidden.png", "res/buttons/round_lock_cell.png", "res/buttons/round_lock_cell.png");
	lock_cell->addTouchEventListener(CC_CALLBACK_2(LevelEditor::allowLockingCell, this));
	lock_cell->setSelected(true);
	auto resolution = cocos2d::ui::CheckBox::create("res/buttons/round_resolution.png", "res/buttons/round_resolution.png",
		"res/buttons/round_resolution_hidden.png", "res/buttons/round_resolution.png", "res/buttons/round_resolution.png");
	resolution->addTouchEventListener(CC_CALLBACK_2(LevelEditor::showResolutions, this));
	auto background = cocos2d::ui::CheckBox::create("res/buttons/round_background.png", "res/buttons/round_background.png",
		"res/buttons/round_background_hidden.png", "res/buttons/round_background.png", "res/buttons/round_background.png");
	background->addTouchEventListener(CC_CALLBACK_2(LevelEditor::showBackgrounds, this));
	auto object = cocos2d::ui::CheckBox::create("res/buttons/round_object.png", "res/buttons/round_object.png",
		"res/buttons/round_object_hidden.png", "res/buttons/round_object.png", "res/buttons/round_object.png");
	object->addTouchEventListener(CC_CALLBACK_2(LevelEditor::showObjects, this));
	auto show_path = cocos2d::ui::CheckBox::create("res/buttons/round_path.png", "res/buttons/round_path.png",
		"res/buttons/round_path_hidden.png", "res/buttons/round_path.png", "res/buttons/round_path.png");
	show_path->addTouchEventListener(CC_CALLBACK_2(LevelEditor::showPath, this));

	ui::Layout* selected_path_interface = ui::Layout::create();
	selected_path_interface->setVisible(false);
	selected_path_interface->setContentSize(Size(visibleSize.width * 3.0 / 4.0, object->getContentSize().height));
	selected_path_interface->setPosition(Vec2(visibleSize.width * 5.0 / 8.0, visibleSize.height / 2));
	auto validate = cocos2d::ui::Button::create("res/buttons/round_validate.png", "res/buttons/round_validate.png");
	validate->addTouchEventListener(CC_CALLBACK_2(LevelEditor::validatePath, this));
	auto remove_path = cocos2d::ui::Button::create("res/buttons/round_remove_path.png", "res/buttons/round_remove_path.png",
		"res/buttons/round_remove_path_disable.png");
	remove_path->addTouchEventListener(CC_CALLBACK_2(LevelEditor::removePathTile, this));
	auto delete_path = cocos2d::ui::Button::create("res/buttons/round_delete.png", "res/buttons/round_delete.png");
	delete_path->addTouchEventListener(CC_CALLBACK_2(LevelEditor::removePath, this,-1));
	auto inverse_path = cocos2d::ui::Button::create("res/buttons/round_inverse_path.png", "res/buttons/round_inverse_path.png");
	inverse_path->addTouchEventListener(CC_CALLBACK_2(LevelEditor::inversePath, this));
	selected_path_interface->addChild(validate, 0, "validate");
	selected_path_interface->addChild(remove_path, 0, "remove_path");
	selected_path_interface->addChild(delete_path, 0, "delete_path");
	selected_path_interface->addChild(inverse_path, 0, "inverse_path");


	double offsetTop = 0.02 * panel->getContentSize().height * panel->getScaleY();
	double offsetLeft = 0.03 * panel->getContentSize().width * panel->getScaleX();

	validate->setScale(visibleSize.width / 1280);
	remove_path->setScale(visibleSize.width / 1280);
	remove_path->setPosition(Vec2(0, -remove_path->getContentSize().height * remove_path->getScale() / 2 - offsetLeft));
	validate->setPosition(Vec2(remove_path->getContentSize().width * remove_path->getScale() / 2 + offsetLeft + validate->getContentSize().width / 2,
		-validate->getContentSize().height * validate->getScale() / 2 - offsetLeft));
	delete_path->setPosition(Vec2(-remove_path->getContentSize().width * remove_path->getScale() / 2 - offsetLeft - delete_path->getContentSize().width / 2,
		-delete_path->getContentSize().height * delete_path->getScale() / 2 - offsetLeft));
	inverse_path->setPosition(Vec2(validate->getPosition().x + validate->getContentSize().width * validate->getScale() / 2 +
		offsetLeft + inverse_path->getContentSize().width / 2,
		-inverse_path->getContentSize().height * inverse_path->getScale() / 2 - offsetLeft));

	ui::Layout* selected_sprite_interface = ui::Layout::create();
	selected_sprite_interface->setVisible(false);
	selected_sprite_interface->setContentSize(Size(visibleSize.width * 3.0 / 4.0, object->getContentSize().height));
	selected_sprite_interface->setPosition(Vec2(visibleSize.width * 5.0 / 8.0, visibleSize.height / 2));
	auto rescale = cocos2d::ui::Button::create("res/buttons/round_scale.png", "res/buttons/round_scale.png");
	rescale->addTouchEventListener(CC_CALLBACK_2(LevelEditor::scaleObject, this));
	auto rotate = cocos2d::ui::Button::create("res/buttons/round_rotate.png", "res/buttons/round_rotate.png");
	rotate->addTouchEventListener(CC_CALLBACK_2(LevelEditor::rotateObject, this));
	auto flip = cocos2d::ui::Button::create("res/buttons/round_flip.png", "res/buttons/round_flip.png");
	flip->addTouchEventListener(CC_CALLBACK_2(LevelEditor::flipObject, this));
	auto flipY = cocos2d::ui::Button::create("res/buttons/round_flipY.png", "res/buttons/round_flipY.png");
	flipY->addTouchEventListener(CC_CALLBACK_2(LevelEditor::flipObjectY, this));
	auto move_cell = cocos2d::ui::CheckBox::create("res/buttons/round_move.png", "res/buttons/round_move.png",
		"res/buttons/round_move_hidden.png", "res/buttons/round_move.png", "res/buttons/round_move.png");
	move_cell->addTouchEventListener(CC_CALLBACK_2(LevelEditor::setMoveByCell, this));

	selected_sprite_interface->addChild(rescale, 0, "rescale");
	selected_sprite_interface->addChild(rotate, 0, "rotate");
	selected_sprite_interface->addChild(flip, 0, "flip");
	selected_sprite_interface->addChild(flipY, 0, "flipY");
	selected_sprite_interface->addChild(move_cell, 0, "move_cell");

	zoom_in->setScale(visibleSize.width / 1280);
	zoom_out->setScale(visibleSize.width / 1280);
	grid->setScale(visibleSize.width / 1280);
	lock_cell->setScale(visibleSize.width / 1280);
	resolution->setScale(visibleSize.width / 1280);
	background->setScale(visibleSize.width / 1280);
	object->setScale(visibleSize.width / 1280);
	rescale->setScale(visibleSize.width / 1280);
	rotate->setScale(visibleSize.width / 1280);
	show_path->setScale(visibleSize.width / 1280);
	flip->setScale(visibleSize.width / 1280);
	flipY->setScale(visibleSize.width / 1280);
	move_cell->setScale(visibleSize.width / 1280);
	delete_path->setScale(visibleSize.width / 1280);
	inverse_path->setScale(visibleSize.width / 1280);


	zoom_in->setPosition(Vec2(offsetLeft + visibleSize.width / 4 + zoom_in->getContentSize().width / 2,
		visibleSize.height / 2 - zoom_in->getContentSize().height * zoom_in->getScale() / 2 - offsetTop));
	zoom_out->setPosition(Vec2(offsetLeft + visibleSize.width / 4 + zoom_out->getContentSize().width / 2,
		zoom_in->getPosition().y - zoom_out->getContentSize().height * zoom_out->getScale() - offsetTop));
	grid->setPosition(Vec2(offsetLeft + visibleSize.width / 4 + grid->getContentSize().width / 2,
		zoom_out->getPosition().y - grid->getContentSize().height * grid->getScale() - offsetTop));
	resolution->setPosition(Vec2(offsetLeft + visibleSize.width / 4 + resolution->getContentSize().width / 2,
		grid->getPosition().y - resolution->getContentSize().height * resolution->getScale() - offsetTop));
	lock_cell->setPosition(Vec2(2*offsetLeft + visibleSize.width / 4 + lock_cell->getContentSize().width / 2,
		resolution->getPosition().y - lock_cell->getContentSize().height * lock_cell->getScale() - offsetTop));
	background->setPosition(Vec2(offsetLeft + visibleSize.width / 4 + background->getContentSize().width / 2,
		- visibleSize.height / 2 + background->getContentSize().height * background->getScale() / 2 + offsetTop));
	object->setPosition(Vec2(offsetLeft + visibleSize.width / 4 + object->getContentSize().width / 2,
		background->getPosition().y + object->getContentSize().height * object->getScale() + offsetTop));
	show_path->setPosition(Vec2(offsetLeft + visibleSize.width / 4 + show_path->getContentSize().width / 2,
		object->getPosition().y + show_path->getContentSize().height * show_path->getScale() + offsetTop));


	rescale->setPosition(Vec2(0, -rescale->getContentSize().height * rescale->getScale() / 2 - offsetLeft));
	rotate->setPosition(Vec2(rescale->getContentSize().width * rescale->getScale() / 2 + offsetLeft + rotate->getContentSize().width / 2,
		-rotate->getContentSize().height * rotate->getScale() / 2 - offsetLeft));
	flip->setPosition(Vec2(rotate->getPosition().x + rotate->getContentSize().width * rotate->getScale() / 2 + 
		offsetLeft + flip->getContentSize().width / 2,
		-flip->getContentSize().height * flip->getScale() / 2 - offsetLeft));
	flipY->setPosition(Vec2(flip->getPosition().x + flip->getContentSize().width * flip->getScale() / 2 +
		offsetLeft + flipY->getContentSize().width / 2,
		-flipY->getContentSize().height * flipY->getScale() / 2 - offsetLeft));
	move_cell->setPosition(Vec2(-rescale->getContentSize().width * rescale->getScale() / 2 - offsetLeft - move_cell->getContentSize().width / 2,
		-move_cell->getContentSize().height * move_cell->getScale() / 2 - offsetLeft));

	interface_manager->addChild(zoom_in, 0, "zoom_in");
	interface_manager->addChild(zoom_out, 0, "zoom_out");
	interface_manager->addChild(grid, 0, "grid");
	interface_manager->addChild(resolution, 0, "resolution");
	interface_manager->addChild(lock_cell, 0, "lock_cell");
	interface_manager->addChild(background, 0, "background");
	interface_manager->addChild(object, 0, "object");
	interface_manager->addChild(show_path, 0, "path");
	interface_manager->addChild(selected_sprite_interface, 0, "selected_sprite_interface");
	interface_manager->addChild(selected_path_interface, 0, "selected_path_interface");

	MenuItemSprite* new_level = MenuItemImage::create("res/buttons/wood_new.png", "res/buttons/wood_new.png",
		CC_CALLBACK_1(LevelEditor::createLevel, this));
	new_level->setScale(visibleSize.width / 960);
	MenuItemSprite* save = MenuItemImage::create("res/buttons/wood_save.png", "res/buttons/wood_save.png",
		"res/buttons/wood_save_disable.png", CC_CALLBACK_1(LevelEditor::saveLevelIntoDB, this));
	save->setScale(visibleSize.width / 960);
	MenuItemSprite* open = MenuItemImage::create("res/buttons/wood_open.png", "res/buttons/wood_open.png",
		CC_CALLBACK_1(LevelEditor::showExistingLevels, this));
	open->setScale(visibleSize.width / 960);
	MenuItemSprite* del = MenuItemImage::create("res/buttons/wood_del.png", "res/buttons/wood_del.png",
		"res/buttons/wood_del_disable.png", CC_CALLBACK_1(LevelEditor::showDeleteConfirm, this));
	del->setScale(visibleSize.width / 960);

	save->setEnabled(false);
	del->setEnabled(false);

	Menu* main_menu = Menu::create();
	main_menu->addChild(new_level, 1, "new_level");
	main_menu->addChild(save, 1, "save");
	main_menu->addChild(open, 1, "open");
	main_menu->addChild(del, 1, "del");
	main_menu->alignItemsHorizontally();

	ui::Layout* general_infos = ui::Layout::create();
	ui::TextField* level_title = ui::TextField::create("", "Arial", 20 * (visibleSize.width / 960));
	level_title->setString("LevelName");
	level_title->setMaxLengthEnabled(true);
	level_title->setTouchAreaEnabled(true);
	level_title->setSwallowTouches(true);
	level_title->setMaxLength(10);
	level_title->setColor(Color3B::BLACK);
	level_title->setPlaceHolderColor(Color4B::BLACK);
	level_title->setAnchorPoint(Vec2(0.5, 0.5));
	level_title->addChild(Sprite::create("res/buttons/cursor.png"),0,"cursor");
	level_title->getChildByName("cursor")->setVisible(false);
	level_title->getChildByName("cursor")->setPosition(level_title->getContentSize().width +
		level_title->getChildByName("cursor")->getContentSize().width, level_title->getChildByName("cursor")->getContentSize().height / 2);
	level_title->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			if (((ui::TextField*)sender)->getString() == "LevelName") {
				((ui::TextField*)sender)->setString("");
			}
			((ui::TextField*)sender)->getChildByName("cursor")->setVisible(false);
			((ui::TextField*)sender)->getChildByName("cursor")->runAction(RepeatForever::create(Blink::create(1.0f, 1)));
			((ui::TextField*)sender)->getChildByName("cursor")->setPosition(((ui::TextField*)sender)->getContentSize().width +
				((ui::TextField*)sender)->getChildByName("cursor")->getContentSize().width, 
				((ui::TextField*)sender)->getChildByName("cursor")->getContentSize().height / 2);
			textfield_selected = ((ui::TextField*)sender);
		}
		else if (type == ui::Widget::TouchEventType::CANCELED) {
			textfield_selected = nullptr;
			((ui::TextField*)sender)->getChildByName("cursor")->setVisible(false);
		}
	});
	auto eventListener = EventListenerKeyboard::create();
	eventListener->onKeyReleased = [&, level_title](EventKeyboard::KeyCode keyCode, Event* event) {
		std::string title_content = level_title->getString();
		std::size_t pos = std::string::npos;
		do {
			pos = title_content.find(' ');
			if (pos != std::string::npos) {
				title_content.erase(pos, 1);
			}
		} while (pos != std::string::npos);
		level_title->setString(title_content);
		level_settings["name"] = title_content;
		level_title->getChildByName("cursor")->setPosition(level_title->getContentSize().width +
			level_title->getChildByName("cursor")->getContentSize().width,
			level_title->getChildByName("cursor")->getContentSize().height / 2);
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, level_title);

	general_infos->addChild(level_title,1,"level_name");
	general_infos->addChild(Sprite::create("res/buttons/wood_textfield.png"), 0, "background");
	general_infos->getChildByName("background")->setScale(panel->getContentSize().width * panel->getScaleX() * 0.8 / 
		general_infos->getChildByName("background")->getContentSize().width);

	level_title->setTouchSize(Size(general_infos->getChildByName("background")->getContentSize().width * 
		general_infos->getChildByName("background")->getScaleX(),
		general_infos->getChildByName("background")->getContentSize().height * 
		general_infos->getChildByName("background")->getScaleX()));

	ui::ScrollView* scroll_view = ui::ScrollView::create();
	scroll_view->setDirection(ui::ScrollView::Direction::VERTICAL);
	scroll_view->setAnchorPoint(Vec2(0.5, 0.5));
	scroll_view->setContentSize(Size(0.90*visibleSize.width / 4, visibleSize.height * 0.65));

	double innerWidth = 0.89*visibleSize.width / 4;
	double innerHeight = visibleSize.height * 2;
	scroll_view->setInnerContainerSize(Size(innerWidth, innerHeight));

	std::vector <std::string> buttons_name = { "Background", "Objects", "Paths", "Waves", "Dialogues" };

	for (unsigned int i(0); i < buttons_name.size(); ++i) {
		ui::Button* level_background = ui::Button::create("res/buttons/wood_pulldown.png", "res/buttons/wood_pulldown.png");
		level_background->setScale(visibleSize.width / 960);
		level_background->setTitleText(buttons_name[i]);
		level_background->setTitleFontSize(20);
		level_background->setTitleFontName("fonts/Love Is Complicated Again.ttf");
		level_background->addChild(Sprite::create("res/buttons/plus.png"), 1, "plus");
		level_background->addChild(Sprite::create("res/buttons/minus.png"), 1, "minus");
		level_background->getChildByName("plus")->setPosition(Vec2(0.05*level_background->getContentSize().width +
			level_background->getChildByName("plus")->getContentSize().width / 2,
			level_background->getContentSize().height / 2));
		level_background->getChildByName("minus")->setPosition(Vec2(0.05*level_background->getContentSize().width +
			level_background->getChildByName("minus")->getContentSize().width / 2,
			level_background->getContentSize().height / 2));
		level_background->getChildByName("minus")->setVisible(false);
		level_background->addChild(ui::Layout::create(),1,"layout");
		level_background->getChildByName("layout")->setAnchorPoint(Vec2(0.5, 1));
		level_background->getChildByName("layout")->setPosition(Vec2(innerWidth / 2,
			-level_background->getContentSize().height / 2));
		level_background->addTouchEventListener(CC_CALLBACK_2(LevelEditor::showSubContent,this,i));
		subContents.push_back(std::make_pair(level_background, false));
		scroll_view->addChild(level_background, 1, buttons_name[i]);
	}

	((ui::Button*)subContents[subContents.size() - 1].first)->setEnabled(false);

	std::string path = "res/levels/background/";
	//std::vector<std::string> files = read_directory(path);
	std::vector<std::string> files = geFilenamesFromDirectory(path + "files.json");
	//double width_image = scroll_view->getContentSize().width * 2 / 5;
	double width_image = subContents[0].first->getContentSize().width * 1 / 3;
	double offset = width_image / 4;
	for (unsigned int i(0); i < files.size(); ++i) {
		if (files[i].find(".png") != std::string::npos) {
			auto checkbox_image = cocos2d::ui::CheckBox::create("res/buttons/checkbox.png", "res/buttons/checkbox.png",
				"res/buttons/filled.png", "res/buttons/checkbox.png", "res/buttons/checkbox.png");
			checkbox_image->setPosition(Vec2(0, i * width_image + width_image / 2));
			checkbox_image->addTouchEventListener(CC_CALLBACK_2(LevelEditor::handleBackground, this,
				"res/levels/background/" + files[i], checkbox_image));
			checkbox_image->setScale(0.5);
			ui::Button* drag_image = ui::Button::create("res/levels/background/" + files[i]);
			drag_image->setScaleX(width_image / drag_image->getContentSize().width);
			drag_image->setScaleY(width_image / drag_image->getContentSize().height);
			drag_image->setPosition(Vec2(scroll_view->getContentSize().width * 2 / 7 / 
				(visibleSize.width / 960), i * width_image + width_image / 2));
			drag_image->addTouchEventListener(CC_CALLBACK_2(LevelEditor::handleBackground, this, "res/levels/background/" + files[i], checkbox_image));
			Label* image_name = Label::createWithTTF(files[i], "fonts/arial.ttf", 15);
			image_name->setDimensions(0.95*width_image, width_image);
			image_name->setAlignment(TextHAlignment::CENTER,TextVAlignment::CENTER);
			image_name->setPosition(Vec2(scroll_view->getContentSize().width * 2 / 3 / 
				(visibleSize.width / 960), i * width_image + width_image / 2));
			image_name->setColor(Color3B::BLACK);
			subContents[0].first->getChildByName("layout")->addChild(checkbox_image);
			subContents[0].first->getChildByName("layout")->addChild(drag_image);
			subContents[0].first->getChildByName("layout")->addChild(image_name);
		}
	}


	// Create object menu
	path = "res/levels/objects/";
	files = geFilenamesFromDirectory(path + "files.json");
	double lastVPos = offset * 2;
	for (unsigned int i(0); i < files.size(); ++i) {
		if (files[i].find(".png") != std::string::npos) {
			std::string plist = files[i];
			plist = plist.replace(plist.length() - 3, plist.length() - 1, "plist");
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path +
				plist, path + files[i]);
			std::string fullPath = FileUtils::getInstance()->fullPathForFilename(path + plist);
			ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
			for (auto iter = dict["frames"].asValueMap().begin(); iter != dict["frames"].asValueMap().end(); ++iter)
			{
				ui::Button* drag_image = ui::Button::create(iter->first, iter->first, iter->first, cocos2d::ui::Widget::TextureResType::PLIST);
				drag_image->setScaleX(width_image / drag_image->getContentSize().width);
				drag_image->setScaleY(width_image / drag_image->getContentSize().height);
				drag_image->setPosition(Vec2(scroll_view->getContentSize().width * 2 / 7 / 
					(visibleSize.width / 960), lastVPos));
				drag_image->addTouchEventListener(CC_CALLBACK_2(LevelEditor::addObject, this, iter->first));
				Label* image_name = Label::createWithTTF(iter->first, "fonts/arial.ttf", 15);
				image_name->setPosition(Vec2(scroll_view->getContentSize().width * 2 / 3 / 
					(visibleSize.width / 960), lastVPos));
				image_name->setDimensions(0.95*width_image, width_image);
				image_name->setColor(Color3B::BLACK);
				image_name->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
				subContents[1].first->getChildByName("layout")->addChild(drag_image);
				subContents[1].first->getChildByName("layout")->addChild(image_name);
				lastVPos = lastVPos + width_image;
			}
		}
	}

	// Create menu for path
	ui::Button* add_path = ui::Button::create("res/buttons/wood_new.png", "res/buttons/wood_new.png");
	add_path->setPosition(Vec2(subContents[2].first->getContentSize().width / 2, 
		add_path->getContentSize().height*add_path->getScaleY() / 2));
	add_path->addTouchEventListener(CC_CALLBACK_2(LevelEditor::createPath, this));
	subContents[2].first->getChildByName("layout")->addChild(add_path,0,"add_path");


	// Create menu for waves generator
	ui::Button* add_wave = ui::Button::create("res/buttons/wood_new.png", "res/buttons/wood_new.png");
	add_wave->setPosition(Vec2(innerWidth / 4, 0));
	add_wave->addTouchEventListener(CC_CALLBACK_2(LevelEditor::createWave, this));
	subContents[3].first->getChildByName("layout")->addChild(add_wave, 0, "add_wave");

	// Create menu for dialogues
	ui::Button* add_dialogue = ui::Button::create("res/buttons/wood_new.png", "res/buttons/wood_new.png");
	add_dialogue->setPosition(Vec2(innerWidth / 4, 0));
	add_dialogue->addTouchEventListener(CC_CALLBACK_2(LevelEditor::showDialogueBox, this));
	subContents[4].first->getChildByName("layout")->addChild(add_dialogue, 0, "add_dialogue");

	// Create enemy selection menu
	ui::Layout* select_enemy = ui::Layout::create();
	addChild(select_enemy, 2, "select_enemy");
	select_enemy->setVisible(false);
	auto create_enemy = ui::Button::create("res/buttons/buttonAddEnemy.png", "res/buttons/buttonAddEnemy.png", "res/buttons/buttonAddEnemy_disable.png");
	create_enemy->setScale(visibleSize.width / 5 / create_enemy->getContentSize().width);
	create_enemy->setEnabled(false);
	create_enemy->addTouchEventListener(CC_CALLBACK_2(LevelEditor::createEnemy, this));
	auto edit_enemy = ui::Button::create("res/buttons/buttonEditEnemy.png", "res/buttons/buttonEditEnemy.png", "res/buttons/buttonEditEnemy_disable.png");
	edit_enemy->setScale(visibleSize.width / 5 / edit_enemy->getContentSize().width);
	edit_enemy->setVisible(false);
	edit_enemy->addTouchEventListener(CC_CALLBACK_2(LevelEditor::editEnemyValues, this));
	auto button_cancel_creation = ui::Button::create("res/buttons/buttonCancel.png", "res/buttons/buttonCancel.png");
	button_cancel_creation->setScale(visibleSize.width / 5 / button_cancel_creation->getContentSize().width);
	button_cancel_creation->addTouchEventListener(CC_CALLBACK_2(LevelEditor::hideEnemyBox, this));
	auto middle_panel_enemy = Sprite::create("res/buttons/centralMenuPanel.png");
	middle_panel_enemy->setScaleY(visibleSize.height * 0.7 / middle_panel_enemy->getContentSize().height);
	middle_panel_enemy->setScaleX(visibleSize.width * 0.65 /
		middle_panel_enemy->getContentSize().width);
	auto enemy_layout = ui::ScrollView::create();
	enemy_layout->setContentSize(Size(0.90*middle_panel_enemy->getContentSize().width * middle_panel_enemy->getScaleX(),
		middle_panel_enemy->getContentSize().height * middle_panel_enemy->getScaleY() * 0.50));
	enemy_layout->setPosition(Vec2(-enemy_layout->getContentSize().width / 2, -enemy_layout->getContentSize().height / 2));
	Label* title_enemy = Label::createWithTTF("SELECT THE ENNEMY", "fonts/LICABOLD.ttf", 40 * (visibleSize.width / 1280));
	title_enemy->setAlignment(TextHAlignment::CENTER);
	title_enemy->setColor(Color3B::BLACK);
	title_enemy->setAnchorPoint(Vec2(0.5, 1));
	title_enemy->setPosition(Vec2(0, 0.90*middle_panel_enemy->getContentSize().height * middle_panel_enemy->getScaleY() / 2));
	Label* delay_label = Label::createWithTTF("Delay", "fonts/LICABOLD.ttf", round(20 * visibleSize.width / 960));
	delay_label->setAlignment(TextHAlignment::RIGHT);
	delay_label->setColor(Color3B::BLACK);
	ui::TextField* enemy_time = ui::TextField::create("", "fonts/LICABOLD.ttf", round(20 * visibleSize.width / 960));
	enemy_time->setString("1.0");
	enemy_time->setColor(Color3B::BLACK);
	enemy_time->setMaxLengthEnabled(true);
	enemy_time->setTouchAreaEnabled(true);
	enemy_time->setSwallowTouches(true);
	enemy_time->setMaxLength(4);
	enemy_time->setAnchorPoint(Vec2(0.5, 0.5));
	auto event_enemy_listener = EventListenerKeyboard::create();
	event_enemy_listener->onKeyReleased = [enemy_time,this](EventKeyboard::KeyCode keyCode, Event* event) {
		enemy_time->setString(purgeCharactersFromString(enemy_time->getString()));
		if (Value(enemy_time->getString()).asDouble() > 30) {
			enemy_time->setString("30");
		}
		enemy_timer = Value(enemy_time->getString()).asDouble();

	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(event_enemy_listener, enemy_time);
	select_enemy->addChild(Sprite::create("res/buttons/wood_textfield.png"), 1, "background");
	select_enemy->getChildByName("background")->setScaleX(
		middle_panel_enemy->getContentSize().width * middle_panel_enemy->getScaleX() / 8 /
		select_enemy->getChildByName("background")->getContentSize().width);
	select_enemy->getChildByName("background")->setScaleY(
		middle_panel_enemy->getContentSize().height * middle_panel_enemy->getScaleY() / 12 /
		select_enemy->getChildByName("background")->getContentSize().height);
	enemy_time->setTouchSize(Size(general_infos->getChildByName("background")->getContentSize().width *
		select_enemy->getChildByName("background")->getScaleX(),
		select_enemy->getChildByName("background")->getContentSize().height *
		select_enemy->getChildByName("background")->getScaleX()));
	select_enemy->getChildByName("background")->setPosition(Vec2(-middle_panel_enemy->getContentSize().width * middle_panel_enemy->getScaleX() / 4,
		-middle_panel_enemy->getContentSize().height * middle_panel_enemy->getScaleY() / 3));
	enemy_time->setPosition(select_enemy->getChildByName("background")->getPosition());
	delay_label->setPosition(select_enemy->getChildByName("background")->getPosition() - 
		Vec2(select_enemy->getChildByName("background")->getContentSize().width * select_enemy->getChildByName("background")->getScaleX(),0));

	path = "res/levels/dangos/";
	files = geFilenamesFromDirectory(path + "files.json");
	unsigned int counter(0);
	width_image = middle_panel_enemy->getContentSize().width * middle_panel_enemy->getScaleX() / 8;
	for (unsigned int i(0); i < files.size(); ++i) {
		if (files[i].find(".png") != std::string::npos) {
			ui::Button* drag_image = ui::Button::create(path + files[i], path + files[i]);
			drag_image->setScaleX(width_image / drag_image->getContentSize().width);
			drag_image->setScaleY(width_image / drag_image->getContentSize().width);
			drag_image->setAnchorPoint(Vec2(0.5, 0));
			drag_image->setPosition(Vec2(i * (width_image + width_image / 4) + width_image * 3 / 4,
				width_image/4));
			
			auto checkbox_image = cocos2d::ui::CheckBox::create("res/buttons/checkbox_enemy.png", "res/buttons/checkbox_enemy.png",
				"res/buttons/checkbox_enemy_filled.png", "res/buttons/checkbox_enemy.png", "res/buttons/checkbox_enemy.png");
			checkbox_image->setPosition(Vec2(i * (width_image + width_image / 4) + width_image * 3 / 4, width_image / 4));
			checkbox_image->setScaleX((width_image + width_image / 4) / checkbox_image->getContentSize().width);
			checkbox_image->setScaleY(width_image * 3 / 2 / checkbox_image->getContentSize().height);
			checkbox_image->setAnchorPoint(Vec2(0.5, 0));
			
			files[i] = files[i].substr(0, files[i].find(".png"));
			int level_dango = Value(files[i].substr(files[i].size() - 1, 1)).asInt() - 1;
			files[i] = files[i].substr(0, files[i].size() -  1);
			drag_image->addTouchEventListener(CC_CALLBACK_2(LevelEditor::setCurrentEnemy, this, files[i], level_dango, checkbox_image));
			checkbox_image->addTouchEventListener(CC_CALLBACK_2(LevelEditor::setCurrentEnemy, this, files[i], level_dango, checkbox_image));
			Label* image_name = Label::createWithTTF(files[i] + "\n Lvl " + Value(level_dango + 1).asString(), 
				"fonts/LICABOLD.ttf", 20 * visibleSize.width / 1280);
			image_name->setAlignment(TextHAlignment::CENTER);
			image_name->setColor(Color3B::BLACK);
			image_name->setPosition(Vec2(i * (width_image + width_image / 4) + width_image * 3 / 4, width_image * 3 / 2));
			image_name->setAnchorPoint(Vec2(0.5, 0));
			std::string chekboxname = "checkbox_" + files[i] + Value(level_dango).asString();
			enemy_layout->addChild(checkbox_image, 0, "checkbox_"+ files[i] + Value(level_dango).asString());
			enemy_layout->addChild(drag_image, 0, "drag_image");
			enemy_layout->addChild(image_name, 0, "image_name");
			++counter;
		}
	}
	enemy_layout->setInnerContainerSize(Size(counter * (width_image + width_image / 4), width_image * 3 / 2));
	if (enemy_layout->getInnerContainerSize().width <= enemy_layout->getContentSize().width) {
		enemy_layout->setScrollBarEnabled(false);
	}
	enemy_layout->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	ui::ScrollView* path_layout = ui::ScrollView::create();
	path_layout->setDirection(ui::ScrollView::Direction::HORIZONTAL);
	path_layout->setContentSize(Size(width_image * 2, width_image));
	path_layout->setPosition(Vec2(middle_panel_enemy->getContentSize().width * middle_panel_enemy->getScaleX() / 4
		- path_layout->getContentSize().width / 2,
		delay_label->getPosition().y - path_layout->getContentSize().height / 4));
	Label* path_label = Label::createWithTTF("Path", "fonts/LICABOLD.ttf", round(20 * visibleSize.width / 960));
	path_label->setAlignment(TextHAlignment::RIGHT);
	path_label->setColor(Color3B::BLACK);
	path_label->setPosition(Vec2(path_layout->getPosition().x -
		path_label->getContentSize().width / 2,
		delay_label->getPosition().y));
	select_enemy->addChild(middle_panel_enemy, 0, "middle_panel");
	select_enemy->addChild(create_enemy, 1, "create_enemy");
	select_enemy->addChild(edit_enemy, 1, "edit_enemy");
	select_enemy->addChild(path_layout, 1, "path_layout");
	select_enemy->addChild(path_label, 1, "path_label");
	select_enemy->addChild(button_cancel_creation, 1, "button_cancel");
	select_enemy->addChild(enemy_layout, 1, "enemy_layout");
	select_enemy->addChild(title_enemy, 1, "title_enemy");
	select_enemy->addChild(enemy_time, 1, "enemy_time");
	select_enemy->addChild(delay_label, 1, "delay_label");
	button_cancel_creation->setPosition(Vec2(middle_panel_enemy->getContentSize().width * middle_panel_enemy->getScaleX() / 4,
		-middle_panel_enemy->getContentSize().height * middle_panel_enemy->getScaleY() / 2));
	create_enemy->setPosition(Vec2(-middle_panel_enemy->getContentSize().width * middle_panel_enemy->getScaleX() / 4,
		-middle_panel_enemy->getContentSize().height * middle_panel_enemy->getScaleY() / 2));
	edit_enemy->setPosition(Vec2(-middle_panel_enemy->getContentSize().width * middle_panel_enemy->getScaleX() / 4,
		-middle_panel_enemy->getContentSize().height * middle_panel_enemy->getScaleY() / 2));

	// Panel for dialogues
	ui::Layout* select_dialogue = ui::Layout::create();
	addChild(select_dialogue, 2, "select_dialogue");
	select_dialogue->setVisible(false);
	auto create_dialogue = ui::Button::create("res/buttons/buttonAddDialogue.png", "res/buttons/buttonAddDialogue.png", "res/buttons/buttonAddDialogue_disable.png");
	create_dialogue->setEnabled(false);
	create_dialogue->addTouchEventListener(CC_CALLBACK_2(LevelEditor::createDialogue, this));
	auto edit_dialogue = ui::Button::create("res/buttons/buttonEditDialogue.png", "res/buttons/buttonEditDialogue.png", "res/buttons/buttonEditDialogue_disable.png");
	edit_dialogue->setVisible(false);
	edit_dialogue->addTouchEventListener(CC_CALLBACK_2(LevelEditor::editEnemyValues, this));
	auto button_cancel_dialogue = ui::Button::create("res/buttons/buttonCancel.png", "res/buttons/buttonCancel.png");
	button_cancel_dialogue->addTouchEventListener(CC_CALLBACK_2(LevelEditor::hideDialogueBox, this));
	auto middle_panel_dialogue = Sprite::create("res/buttons/centralMenuPanel.png");
	middle_panel_dialogue->setScaleY(visibleSize.height / 2 / middle_panel_dialogue->getContentSize().height);
	middle_panel_dialogue->setScaleX((1.5 * create_dialogue->getContentSize().width * create_dialogue->getScale() +
		create_dialogue->getContentSize().width * 2 / 5 * create_dialogue->getScale()) /
		create_dialogue->getContentSize().width);

	select_dialogue->addChild(middle_panel_dialogue, 0, "middle_panel");
	select_dialogue->addChild(create_dialogue, 1, "create_dialogue");
	select_dialogue->addChild(edit_dialogue, 1, "edit_dialogue");
	select_dialogue->addChild(button_cancel_dialogue, 1, "button_cancel");
	button_cancel_dialogue->setPosition(Vec2(middle_panel_dialogue->getContentSize().width * middle_panel_dialogue->getScaleX() / 4,
		-middle_panel_dialogue->getContentSize().height * middle_panel_dialogue->getScaleY() / 2));
	create_dialogue->setPosition(Vec2(-middle_panel_dialogue->getContentSize().width * middle_panel_dialogue->getScaleX() / 4,
		-middle_panel_dialogue->getContentSize().height * middle_panel_dialogue->getScaleY() / 2));
	edit_dialogue->setPosition(Vec2(-middle_panel_dialogue->getContentSize().width * middle_panel_dialogue->getScaleX() / 4,
		-middle_panel_dialogue->getContentSize().height * middle_panel_dialogue->getScaleY() / 2));

	// Add the interface composed by the zoom icons, the panel on the left, the scroll view (sub menus)and the 
	// main menu which is made of save button, create new level, delete level...
	getChildByName("menu_left")->addChild(interface_manager, 0, "interface_manager");
	getChildByName("menu_left")->addChild(scroll_view, 1, "scroll_menu");
	getChildByName("menu_left")->addChild(main_menu, 1, "main_menu");
	getChildByName("menu_left")->addChild(general_infos, 2, "general_infos");
	scroll_view->setVisible(false);
	interface_manager->setVisible(false);
	general_infos->setVisible(false);
	
	// Place the inteface manager, the main menu and the scrolling menu
	getChildByName("menu_left")->getChildByName("interface_manager")->setPosition(0, visibleSize.height / 2);
	getChildByName("menu_left")->getChildByName("main_menu")->setPosition(visibleSize.width / 8, visibleSize.height * 0.95);
	getChildByName("menu_left")->getChildByName("scroll_menu")->setPosition(Vec2(visibleSize.width / 8, visibleSize.height / 2));
	getChildByName("menu_left")->getChildByName("general_infos")->setPosition(Vec2(visibleSize.width / 8, visibleSize.height * 0.87));

	// Repositionning of the panels and submenus after adding all the different layouts.
	// Compute the size of the layout based on its children position, size, scale and anchor points.
	updateDisplayScrollingMenu();
	
	addChild(Sprite::create("res/background/crissXcross.png"), 0, "background");
	getChildByName("background")->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	getChildByName("background")->setScaleX(visibleSize.width / getChildByName("background")->getContentSize().width);
	getChildByName("background")->setScaleY(visibleSize.height / getChildByName("background")->getContentSize().height);

	getChildByName("level")->setPosition(Vec2(visibleSize.width * 5 / 8, visibleSize.height / 2));
	getChildByName("level")->setAnchorPoint(Vec2(0, 0));
	getChildByName("level")->setScale(0.8f);

	getChildByName("select_level")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("select_enemy")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("select_dialogue")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	getChildByName("confirm_deletion")->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	ui::Layout* selection_box = ui::Layout::create();
	getChildByName("level")->addChild(selection_box, 3, "selection_box");
	
	addEvents();
	updateDisplayWaveMenu();
	updateDisplayPathMenu();
	updateDisplayScrollingMenu();

	return true;
}

void LevelEditor::addEvents()
{
	auto listener_keyboard = cocos2d::EventListenerKeyboard::create();
	listener_keyboard->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		if (textfield_selected == nullptr) {
			switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_DELETE:
				if (selected_object != nullptr) {
					bool isBackground = false;
					for (auto background : backgrounds) {
						if (selected_object == background.first) {
							isBackground = true;
						}
					}
					if (!isBackground) {
						unsigned int deletion = 0;
						for (unsigned int i(0); i < objects.size(); ++i) {
							if (selected_object == objects[i].first) {
								deletion = i;
							}
						}
						objects.erase(objects.begin() + deletion);
						getChildByName("level")->getChildByName("objects")->removeChild(selected_object);
						getChildByName("menu_left")->getChildByName("interface_manager")->getChildByName("selected_sprite_interface")->setVisible(false);
						selected_object = nullptr;
					}
				}
				break;
			case EventKeyboard::KeyCode::KEY_F:
				if (selected_object != nullptr) {
					((Sprite*)selected_object)->setFlippedX(!((Sprite*)selected_object)->isFlippedX());
				}
				break;
			case EventKeyboard::KeyCode::KEY_S:
				if (selected_object != nullptr) {
					scaleObject(nullptr, ui::Widget::TouchEventType::ENDED);
				}
				break;
			case EventKeyboard::KeyCode::KEY_R:
				if (selected_object != nullptr) {
					((Sprite*)selected_object)->setRotation(((Sprite*)selected_object)->getRotation() + 45);
				}
				break;
			case EventKeyboard::KeyCode::KEY_KP_PLUS:
				if (selected_object != nullptr) {
					((Sprite*)selected_object)->setScale(((Sprite*)selected_object)->getScale() + 0.01);
				}
				else {
					zoomIn(nullptr, ui::Widget::TouchEventType::ENDED);
				}
				break;
			case EventKeyboard::KeyCode::KEY_KP_MINUS:
				if (selected_object != nullptr) {
					((Sprite*)selected_object)->setScale(((Sprite*)selected_object)->getScale() - 0.01);
				}
				else {
					zoomOut(nullptr, ui::Widget::TouchEventType::ENDED);
				}
				break;
			case EventKeyboard::KeyCode::KEY_SPACE:
				if (selected_object != nullptr) {
					getChildByName("menu_left")->getChildByName("interface_manager")->getChildByName("selected_sprite_interface")->setVisible(
						!getChildByName("menu_left")->getChildByName("interface_manager")->isVisible());
				}
				getChildByName("menu_left")->getChildByName("interface_manager")->setVisible(
					!getChildByName("menu_left")->getChildByName("interface_manager")->isVisible());
				break;
			}
		}
		else {
			if (keyCode == EventKeyboard::KeyCode::KEY_ENTER || 
				keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
				textfield_selected->setFocused(false);
				textfield_selected->getChildByName("cursor")->stopAllActions();
				textfield_selected->getChildByName("cursor")->setVisible(false);
				textfield_selected = nullptr;
			}
		}
	};

	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event) {
		
			if (textfield_selected != nullptr) {
				textfield_selected->setFocused(false);
				textfield_selected->getChildByName("cursor")->stopAllActions();
				textfield_selected->getChildByName("cursor")->setVisible(false);
				textfield_selected = nullptr;
			}
			cocos2d::Vec2 p = touch->getLocation();
			cocos2d::Rect rect = this->getBoundingBox();
			cocos2d::Rect rectleftpanel = getChildByName("menu_left")->getBoundingBox();

			if (!rectleftpanel.containsPoint(p)) {
				if (selecting_cell) {
					p = Vec2((p.x - getChildByName("level")->getPosition().x) / getChildByName("level")->getScale(),
						(p.y - getChildByName("level")->getPosition().y) / getChildByName("level")->getScale());
					Vec2 grid_pos = p;
					int x_index = round((grid_pos.x + size_cell / 2.0) / size_cell);
					int y_index = round((grid_pos.y + size_cell / 2.0) / size_cell);
					grid_pos.x = x_index * size_cell - grid_offset.x - size_cell / 2.0;
					grid_pos.y = y_index * size_cell - grid_offset.y - size_cell / 2.0;

					int element_to_remove = -1;
					for (unsigned int i(0); i < locked_cells.size(); ++i) {
						if (locked_cells[i].first == grid_pos) {
							getChildByName("level")->getChildByName("grid")->removeChild(locked_cells[i].second);
							locked_cells[i].second = nullptr;
							element_to_remove = i;
						}
					}
					if (element_to_remove != -1) {
						locked_cells.erase(locked_cells.begin() + element_to_remove);
					}
					else {
						Node* mask = Sprite::create("res/buttons/mask.png");
						mask->setScaleX(Cell::getCellWidth() / mask->getContentSize().width);
						mask->setScaleY(Cell::getCellWidth() / mask->getContentSize().height);
						mask->setPosition(grid_pos);
						getChildByName("level")->getChildByName("grid")->addChild(mask,200);
						locked_cells.push_back(std::make_pair(grid_pos, mask));
					}

				}
				else {
					if (path_selected != -1) {
						p = Vec2((p.x - getChildByName("level")->getPosition().x) / getChildByName("level")->getScale(),
							(p.y - getChildByName("level")->getPosition().y) / getChildByName("level")->getScale());
						addPath(p);
					}
					else {
						Sprite* selectedSprite = nullptr;
						getChildByName("level")->getChildByName("selection_box")->removeAllChildren();
						p = Vec2((p.x - getChildByName("level")->getPosition().x) / getChildByName("level")->getScale(),
							(p.y - getChildByName("level")->getPosition().y) / getChildByName("level")->getScale());
						if (getChildByName("level")->getChildByName("objects")->isVisible()) {
							for (auto object : objects) {
								cocos2d::Rect n_rect = object.first->getBoundingBox();
								if (n_rect.containsPoint(p)) {
									if (selectedSprite != nullptr && object.first->getLocalZOrder() > selectedSprite->getLocalZOrder()) {
										selectedSprite = object.first;
										getChildByName("menu_left")->getChildByName("interface_manager")->
											getChildByName("selected_sprite_interface")->setVisible(true);
									}
									else if (selectedSprite == nullptr) {
										selectedSprite = object.first;
										getChildByName("menu_left")->getChildByName("interface_manager")->
											getChildByName("selected_sprite_interface")->setVisible(true);
									}
								}
							}
						}
						if (getChildByName("level")->getChildByName("backgrounds")->isVisible()) {
							for (auto background : backgrounds) {
								cocos2d::Rect n_rect = background.first->getBoundingBox();
								if (background.first->getBoundingBox().containsPoint(p)) {
									if (selectedSprite != nullptr && background.first->getLocalZOrder() > selectedSprite->getLocalZOrder()) {
										selectedSprite = background.first;
										getChildByName("menu_left")->getChildByName("interface_manager")->
											getChildByName("selected_sprite_interface")->setVisible(true);
									}
									else if (selectedSprite == nullptr) {
										selectedSprite = background.first;
										getChildByName("menu_left")->getChildByName("interface_manager")->
											getChildByName("selected_sprite_interface")->setVisible(true);
									}
								}
							}
						}
						selected_object = selectedSprite;
						getChildByName("level")->getChildByName("selection_box")->removeAllChildren();
						updateSelectionBox();
						if (selected_object == nullptr) {
							getChildByName("menu_left")->getChildByName("interface_manager")->
								getChildByName("selected_sprite_interface")->setVisible(false);
						}
					}
				}
				return true; // to indicate that we have consumed it.
			}
		
		return false; // we did not consume this event, pass thru.
	};

	listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event) {
	};

	listener->onTouchMoved = [&](cocos2d::Touch* touch, cocos2d::Event* event) {
		if (path_selected != -1) {
			cocos2d::Vec2 p = touch->getLocation();
			p = Vec2((p.x - getChildByName("level")->getPosition().x) / getChildByName("level")->getScale(),
				(p.y - getChildByName("level")->getPosition().y) / getChildByName("level")->getScale());
			addPath(p);
		}
		else {
			if (selected_object != nullptr) {
				if (move_by_cell) {
					cocos2d::Vec2 p = (touch->getLocationInView() - getChildByName("level")->getPosition()) / getChildByName("level")->getScale();
					cocos2d::Vec2 origin = touch->getStartLocation() / getChildByName("level")->getScale();
					selected_object->setPosition(getPositionInGrid(p));
				}
				else {
					cocos2d::Vec2 p = (touch->getLocationInView() - getChildByName("level")->getPosition()) / getChildByName("level")->getScale();
					cocos2d::Vec2 origin = touch->getStartLocation() / getChildByName("level")->getScale();
					selected_object->setPosition(Vec2(p.x, -p.y));
				}
				updateSelectionBox();
			}
		}
		
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener_keyboard,this);
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,this);
}

void LevelEditor::onEnterTransitionDidFinish() {
	Scene::onEnterTransitionDidFinish();
	// Let's the game begin: we schedule the update of MyGame
	scheduleUpdate();
}

void LevelEditor::onExitTransitionDidStart() {
	Scene::onExitTransitionDidStart();
}


/**	Fonction permettant de retourner dans le la scène "Selection des niveaux"
*	@param Ref sender : Scène qui va être changée.
*/
void LevelEditor::returnToGame(Ref* sender){
	SceneManager::getInstance()->setScene(SceneManager::LEVELS);
}

void LevelEditor::zoomIn(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		getChildByName("level")->setScale(getChildByName("level")->getScale() + 0.2);
	}
}

void LevelEditor::zoomOut(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		getChildByName("level")->setScale(getChildByName("level")->getScale() - 0.2);
	}
}

void LevelEditor::showGrid(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		getChildByName("level")->getChildByName("grid")->setVisible(!getChildByName("level")->getChildByName("grid")->isVisible());
		getChildByName("level")->getChildByName("center")->setVisible(!getChildByName("level")->getChildByName("center")->isVisible());
	}
}
void LevelEditor::showResolutions(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		getChildByName("level")->getChildByName("resolutions")->setVisible(!getChildByName("level")->getChildByName("resolutions")->isVisible());
	}
}
void LevelEditor::showBackgrounds(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		getChildByName("level")->getChildByName("backgrounds")->setVisible(!getChildByName("level")->getChildByName("backgrounds")->isVisible());
	}
}
void LevelEditor::showObjects(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		getChildByName("level")->getChildByName("objects")->setVisible(!getChildByName("level")->getChildByName("objects")->isVisible());
	}
}
void LevelEditor::showPath(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		getChildByName("level")->getChildByName("paths")->setVisible(!getChildByName("level")->getChildByName("paths")->isVisible());
	}
}
void LevelEditor::scaleObject(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		if (selected_object != nullptr) {
			selected_object->setScale(size_cell / selected_object->getContentSize().width);
			updateSelectionBox();
		}
	}
}
void LevelEditor::rotateObject(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		if (selected_object != nullptr) {
			((Sprite*)selected_object)->setRotation(((Sprite*)selected_object)->getRotation() + 45);
 		}
	}
}
void LevelEditor::flipObject(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		if (selected_object != nullptr) {
			((Sprite*)selected_object)->setFlippedX(!((Sprite*)selected_object)->isFlippedX());
		}
	}
}
void LevelEditor::flipObjectY(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		if (selected_object != nullptr) {
			((Sprite*)selected_object)->setFlippedY(!((Sprite*)selected_object)->isFlippedY());
		}
	}
}
void LevelEditor::setMoveByCell(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		move_by_cell = !move_by_cell;
	}
}

void LevelEditor::hideShowMenu(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		if (menuHidden) {
			getChildByName("menu_left")->runAction(EaseBounceOut::create(MoveTo::create(0.5f,
				Vec2(0, 0))));
			getChildByName("level")->runAction(MoveTo::create(0.25f, Vec2(visibleSize.width * 5 / 8, visibleSize.height / 2)));
		}
		else {
			getChildByName("menu_left")->runAction(EaseBounceOut::create(MoveTo::create(0.5f, Vec2(
				-getChildByName("menu_left")->getChildByName("panel")->getContentSize().width *
				getChildByName("menu_left")->getChildByName("panel")->getScaleX(), 0))));
			getChildByName("level")->runAction(MoveTo::create(0.25f, Vec2(visibleSize.width / 2, visibleSize.height / 2)));
		}
		menuHidden = !menuHidden;
	}
}

void LevelEditor::createLevel(Ref* sender) {
	resetLevel();

	Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave();

	if (save_root.isMember("local_editor_levels")) {
		level_settings["id"] = save_root["local_editor_levels"][save_root["local_editor_levels"].size() - 1]["id"].asInt() - 1;
	}
	else {
		level_settings["id"] = -2;
	}
	level_settings["id_bdd"] = -1;
	level_settings["name"] = "LevelName";
	level_settings["local"] = true;
	time_t now = time(0);
	tm *ltm = gmtime(&now);

	level_settings["updated"] = Json::Value(1900 + ltm->tm_year).asString() +
		"-" + Json::Value(ltm->tm_mon + 1).asString() +
		"-" + Json::Value(ltm->tm_mday).asString() +
		" " + Json::Value(ltm->tm_hour).asString() +
		":" + Json::Value(ltm->tm_min).asString() +
		":" + Json::Value(ltm->tm_sec).asString();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	DrawNode* resolutions = DrawNode::create();
	double ratio = visibleSize.width / visibleSize.height;
	double width = visibleSize.width * 3 / 4;
	double height = visibleSize.height;

	// 16:9
	double resolution = 16.0 / 9.0;
	drawResolutions(resolutions, resolution, Color4F::MAGENTA);

	// 16:10
	resolution = 16.0 / 10.0;
	drawResolutions(resolutions, resolution, Color4F::BLUE);

	// 5:3
	resolution = 5.0 / 3.0;
	drawResolutions(resolutions, resolution, Color4F::YELLOW);

	// 3:2
	resolution = 3.0 / 2.0;
	drawResolutions(resolutions, resolution, Color4F::ORANGE);

	// 4:3
	resolution = 4.0 / 3.0;
	drawResolutions(resolutions, resolution, Color4F::GREEN);

	DrawNode* grid = DrawNode::create();
	double min_width_ratio = 4.0 / 3.0;
	double min_height_ratio = 16.0 / 9.0;
	int nb_cells_width	= 12;
	size_cell	= (3.0/4.0) * visibleSize.width * sqrt(min_width_ratio / ratio) / nb_cells_width;
	int nb_cells_height	= visibleSize.height * sqrt(ratio / min_height_ratio) / size_cell;
	double max_height	= nb_cells_height * size_cell / 2.0;
	
	double offset_x = (nb_cells_width % 2) * size_cell / 2.0;
	for (int i(0); i < nb_cells_width / 2 + 1 - nb_cells_width % 2; ++i) {
		for (int j(-1); j <= 1; j += 2) {
			grid->drawLine(Vec2(j*i*size_cell + j * offset_x, -max_height), Vec2(j*i*size_cell + j * offset_x, max_height), Color4F::WHITE);
		}
	}
	double offset_y = (nb_cells_height % 2) * size_cell / 2.0;
	grid_offset = Vec2(offset_x,offset_y);
	for (int i(0); i < nb_cells_height / 2.0 + 1 - nb_cells_height % 2; ++i) {
		for (int j(-1); j <= 1; j += 2) {
			grid->drawLine(Vec2(-visibleSize.width * 3 / 8 * sqrt(min_width_ratio / ratio), j*i*size_cell + j * offset_y),
				Vec2(visibleSize.width * 3 / 8 * sqrt(min_width_ratio / ratio), j*i*size_cell + j * offset_y), Color4F::WHITE);
		}
	}
	
	DrawNode* center_ind = DrawNode::create();
	center_ind->drawCircle(Vec2(0, 0), 30, 0, 60, true, Color4F::GREEN);
	getChildByName("level")->addChild(grid, 1000, "grid");
	getChildByName("level")->addChild(center_ind, 1000, "center");
	getChildByName("level")->addChild(resolutions, 1000, "resolutions");

	
	((ui::TextField*)getChildByName("menu_left")->getChildByName("general_infos")->getChildByName("level_name"))->setString("LevelName");

	getChildByName("menu_left")->getChildByName("interface_manager")->setVisible(true);
	getChildByName("menu_left")->getChildByName("scroll_menu")->setVisible(true);
	getChildByName("menu_left")->getChildByName("general_infos")->setVisible(true);
	((MenuItem*)getChildByName("menu_left")->getChildByName("main_menu")->getChildByName("save"))->setEnabled(true);
	((MenuItem*)getChildByName("menu_left")->getChildByName("main_menu")->getChildByName("del"))->setEnabled(true);

	loaded = true;
}

void LevelEditor::createPath(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		getChildByName("menu_left")->getChildByName("interface_manager")->getChildByName("selected_path_interface")->setVisible(true);
		getChildByName("menu_left")->getChildByName("interface_manager")->getChildByName("selected_sprite_interface")->setVisible(false);
		selected_object = nullptr;

		
		unsigned int next_id = 0;
		for (unsigned int i(0); i < id_paths.size(); ++i) {
			if (id_paths[i] > next_id) {
				next_id = id_paths[i];
			}
		}
		++next_id;
		id_paths.push_back(next_id);
		path_selected = paths.size();

		std::vector<Sprite*> new_path;
		std::string path_name = "path_" + Value((int)next_id).asString();
		paths.push_back(std::make_pair(new_path, path_name));
		ui::Layout* layout = ui::Layout::create();
		ui::Button* edit = ui::Button::create("res/buttons/edit.png");
		edit->addTouchEventListener(CC_CALLBACK_2(LevelEditor::selectPath, this, next_id));
		ui::Button* delete_path = ui::Button::create("res/buttons/delete.png");
		delete_path->addTouchEventListener(CC_CALLBACK_2(LevelEditor::removePath, this, next_id));
		Node* add_path = subContents[2].first->getChildByName("layout")->getChildByName("add_path");

		ui::TextField* path_title = ui::TextField::create("", "Arial", 20);
		path_title->setString(path_name);
		path_title->setMaxLength(20);
		path_title->setColor(Color3B::BLACK);
		path_title->setPosition(Vec2(path_title->getContentSize().width / 2.0,
			path_title->getContentSize().height / 2.0));
		layout->setAnchorPoint(Vec2(0.5, 0.50));
		layout->addChild(path_title,1,"path_title");
		layout->addChild(edit, 1, "edit");
		layout->addChild(delete_path, 1, "delete_path");
		layout->setContentSize(path_title->getContentSize());
		subContents[2].first->getChildByName("layout")->addChild(layout,1, path_name);
		path_title->setMaxLengthEnabled(true);
		path_title->setTouchAreaEnabled(true);
		path_title->setSwallowTouches(true);
		path_title->setMaxLength(10);
		path_title->addChild(Sprite::create("res/buttons/cursor.png"), 0, "cursor");
		path_title->getChildByName("cursor")->setVisible(false);
		path_title->getChildByName("cursor")->setPosition(path_title->getContentSize().width +
			path_title->getChildByName("cursor")->getContentSize().width / 2, 
			path_title->getChildByName("cursor")->getContentSize().height / 2);
		path_title->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				((ui::TextField*)sender)->getChildByName("cursor")->setVisible(false);
				((ui::TextField*)sender)->getChildByName("cursor")->runAction(RepeatForever::create(Blink::create(1.0f, 1)));
				((ui::TextField*)sender)->getChildByName("cursor")->setPosition(((ui::TextField*)sender)->getContentSize().width +
					((ui::TextField*)sender)->getChildByName("cursor")->getContentSize().width / 2,
					((ui::TextField*)sender)->getChildByName("cursor")->getContentSize().height / 2);
				textfield_selected = ((ui::TextField*)sender);
			}
			else if (type == ui::Widget::TouchEventType::CANCELED) {
				textfield_selected = nullptr;
				((ui::TextField*)sender)->getChildByName("cursor")->setVisible(false);
			}
		});
		auto eventListener = EventListenerKeyboard::create();
		eventListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
			if (textfield_selected != nullptr) {
				textfield_selected->getChildByName("cursor")->setPosition(textfield_selected->getContentSize().width +
					textfield_selected->getChildByName("cursor")->getContentSize().width / 2,
					textfield_selected->getChildByName("cursor")->getContentSize().height / 2);
			}
		};
		layout->addChild(Sprite::create("res/buttons/wood_textfield.png"), 0, "background");
		layout->getChildByName("background")->setScaleX(subContents[2].first->getContentSize().width / 2 / 
			layout->getChildByName("background")->getContentSize().width);
		layout->getChildByName("background")->setScaleY(path_title->getContentSize().height * 1.5 / 
			layout->getChildByName("background")->getContentSize().height);
		layout->getChildByName("background")->setPosition(Vec2(path_title->getContentSize().width *
			path_title->getScale() / 2.0,
			path_title->getContentSize().height *
			path_title->getScale() / 2.0));
		path_title->setTouchSize(Size(layout->getChildByName("background")->getContentSize().width *
			layout->getChildByName("background")->getScaleX(),
			layout->getChildByName("background")->getContentSize().height *
			layout->getChildByName("background")->getScaleY()));
		delete_path->setPosition(Vec2(path_title->getContentSize().width *
			path_title->getScale() * 1.75 + delete_path->getContentSize().width * delete_path->getScaleX() / 2,
			path_title->getContentSize().height *
			path_title->getScale() / 2));
		edit->setPosition(Vec2(-path_title->getContentSize().width *
			path_title->getScale() *0.75 - edit->getContentSize().width * edit->getScaleX() / 2,
			path_title->getContentSize().height *
			path_title->getScale() / 2));
		cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, path_title);

		updateDisplayPathMenu();
	}
}

void LevelEditor::createWave(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		Size visibleSize = Director::getInstance()->getVisibleSize();

		waves_id[wave_id] = generator->getNbWaves();

		ui::Button* wave_button = ui::Button::create("res/buttons/wood_pulldown.png", "res/buttons/wood_pulldown.png");
		wave_button->setScale(0.90f);
		wave_button->setTitleText("Wave " + Value((int)wave_id).asString());
		wave_button->setTitleFontSize(20);
		wave_button->setTitleFontName("fonts/Love Is Complicated Again.ttf");

		ui::Button* delete_wave = ui::Button::create("res/buttons/delete.png", "res/buttons/delete.png");
		delete_wave->addTouchEventListener(CC_CALLBACK_2(LevelEditor::removeWave, this, wave_id));

		wave_button->addChild(Sprite::create("res/buttons/plus.png"), 1, "plus");
		wave_button->addChild(Sprite::create("res/buttons/minus.png"), 1, "minus");
		wave_button->addChild(delete_wave, 1, "delete");

		delete_wave->setPosition(Vec2(0.95*wave_button->getContentSize().width -
			delete_wave->getContentSize().width / 2, wave_button->getContentSize().height / 2));
		wave_button->getChildByName("plus")->setPosition(Vec2(0.05*wave_button->getContentSize().width +
			wave_button->getChildByName("plus")->getContentSize().width / 2,
			wave_button->getContentSize().height / 2));
		wave_button->getChildByName("minus")->setPosition(Vec2(0.05*wave_button->getContentSize().width +
			wave_button->getChildByName("minus")->getContentSize().width / 2,
			wave_button->getContentSize().height / 2));
		wave_button->getChildByName("minus")->setVisible(false);
		wave_button->addChild(ui::Layout::create(), 1, "layout");
		wave_button->getChildByName("layout")->setAnchorPoint(Vec2(0.5, 1));
		//double innerWidth = ((ui::ListView*)getChildByName("select_level")->getChildByName("list_levels"))->getContentSize().width;
		double innerWidth = ((ui::ScrollView*)getChildByName("menu_left")->getChildByName("scroll_menu"))->getContentSize().width;
		wave_button->setPosition(Vec2(innerWidth / 4,
			-wave_button->getContentSize().height / 2));
		wave_button->setAnchorPoint(Vec2(0.5, 0.5));
		wave_button->getChildByName("layout")->setPosition(Vec2(wave_button->getContentSize().width / wave_button->getScaleX() / 2,
			-wave_button->getContentSize().height / 2));

		ui::Button* add_enemy = ui::Button::create("res/buttons/wood_new.png", "res/buttons/wood_new.png");
		add_enemy->setPosition(Vec2(0, -add_enemy->getContentSize().height / 2));
		add_enemy->addTouchEventListener(CC_CALLBACK_2(LevelEditor::showEnemyBox, this, wave_id));
		Label* delay = Label::createWithTTF("Delay","fonts/LICABOLD.ttf", 16);
		Label* name = Label::createWithTTF("Enemy", "fonts/LICABOLD.ttf", 16);
		Label* lvl = Label::createWithTTF("Lvl", "fonts/LICABOLD.ttf", 16);
		Label* path = Label::createWithTTF("Path", "fonts/LICABOLD.ttf", 16);
		delay->setColor(Color3B::BLACK);
		name->setColor(Color3B::BLACK);
		lvl->setColor(Color3B::BLACK);
		path->setColor(Color3B::BLACK);

		wave_button->getChildByName("layout")->addChild(add_enemy, 0, "add_enemy");
		wave_button->getChildByName("layout")->addChild(delay, 0, "delay_label");
		wave_button->getChildByName("layout")->addChild(name, 0, "enemy_label");
		wave_button->getChildByName("layout")->addChild(lvl, 0, "lvl_label");
		wave_button->getChildByName("layout")->addChild(path, 0, "path_label");

		wave_button->getChildByName("layout")->setVisible(false);
		
		wave_button->addTouchEventListener(CC_CALLBACK_2(LevelEditor::showWave, this, wave_id));
		waves.push_back(std::make_pair(wave_button, false));
		subContents[3].first->getChildByName("layout")->addChild(wave_button, 1, "wave_" + Value((int)wave_id).asString());
		updateDisplayWaveMenu();
		generator->addWave();
		showWave(this,cocos2d::ui::Widget::TouchEventType::ENDED, wave_id);
		++wave_id;
	}
}

void LevelEditor::updateDisplayPathButtons() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto path_layout = (ui::ScrollView*)getChildByName("select_enemy")->getChildByName("path_layout");
	path_layout->removeAllChildren();
	Size size(0,0);
	for (unsigned int i(0); i < id_paths.size(); ++i){
		ui::CheckBox* path_button = ui::CheckBox::create("res/buttons/wood.png", "res/buttons/wood.png",
			"res/buttons/wood_selected.png", "res/buttons/wood.png", "res/buttons/wood.png");
		path_button->setScale(path_layout->getContentSize().width / 3 /
			path_button->getContentSize().width);
		path_button->addTouchEventListener([&, path_button, i](Ref* sender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				auto path_layout = (ui::ScrollView*)getChildByName("select_enemy")->getChildByName("path_layout");
				for (auto checkbox : path_layout->getChildren()) {
					((ui::CheckBox*)checkbox)->setSelected(false);
				}
				path_button->setSelected(true);
				enemy_path = i;
				if (enemy_name != "") {
					((ui::Button*)getChildByName("select_enemy")->getChildByName("create_enemy"))->setEnabled(true);
					((ui::Button*)getChildByName("select_enemy")->getChildByName("edit_enemy"))->setEnabled(true);
				}
			}
		});
		Label* path_number = Label::createWithTTF(Value((int)id_paths[i]).asString(), "fonts/LICABOLD.ttf", 20);
		path_number->setColor(Color3B::BLACK);
		path_number->setPosition(path_button->getContentSize().width / 2, path_button->getContentSize().height / 2);
		path_button->addChild(path_number);
		path_button->setPosition(Vec2(path_button->getContentSize().width * path_button->getScale() * (i + 1), 
			path_button->getContentSize().height * path_button->getScale() / 2));
		path_layout->addChild(
			path_button,1,"path_button" + Value((int)id_paths[i]).asString());
		size = path_button->getContentSize()*path_button->getScale();
	}
	if (id_paths.size() > 0) {
		((ui::ScrollView*)path_layout)->setInnerContainerSize(Size(size.width *
			(id_paths.size() + 1), size.height));
	}
	if (path_layout->getInnerContainerSize().width <= path_layout->getContentSize().width) {
		path_layout->setScrollBarEnabled(false);
	}
	else {
		path_layout->setScrollBarEnabled(true);
	}
}

void LevelEditor::showEnemyBox(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int wave) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		current_wave = waves_id[wave];
		getChildByName("black_mask")->setVisible(true);
		getChildByName("select_enemy")->getChildByName("edit_enemy")->setVisible(false);
		getChildByName("select_enemy")->getChildByName("create_enemy")->setVisible(true);
		getChildByName("select_enemy")->setVisible(true);
		enemy_timer = Value(((ui::TextField*)getChildByName("select_enemy")->getChildByName("enemy_time"))->getString()).asDouble();
		updateDisplayPathButtons();
	}
}
void LevelEditor::hideEnemyBox(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		if (enemy_checkbox != nullptr) {
			enemy_checkbox->setSelected(false);
		}
		enemy_name = "";
		enemy_level = -1;
		enemy_path = -1;
		c_enemy_id = -1;
		enemy_checkbox = nullptr;
		((ui::Button*)getChildByName("select_enemy")->getChildByName("create_enemy"))->setEnabled(false);
		getChildByName("select_enemy")->setVisible(false);
		getChildByName("black_mask")->setVisible(false);
	}
}

void LevelEditor::showDialogueBox(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		getChildByName("select_dialogue")->setVisible(true);
	}
}
void LevelEditor::hideDialogueBox(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		getChildByName("select_dialogue")->setVisible(false);
	}
}

void LevelEditor::createEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {

		double enemy_delay = enemy_timer;
		int next_id = enemies_id + 1;
		
		enemies[next_id] = std::make_pair(generator->getNbSteps(current_wave), current_wave);
		generator->addStep(enemy_name + Value(enemy_level).asString(), enemy_delay, enemy_path, current_wave);
		createEnemyDisplay(next_id);
		++enemies_id;
		hideEnemyBox(sender, cocos2d::ui::Widget::TouchEventType::ENDED);
	}
}

void LevelEditor::createEnemyDisplay(int id) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Node* delay_label = waves[current_wave].first->getChildByName("layout")->getChildByName("delay_label");
	Node* enemy_label = waves[current_wave].first->getChildByName("layout")->getChildByName("enemy_label");
	Node* lvl_label = waves[current_wave].first->getChildByName("layout")->getChildByName("lvl_label");
	Node* path_label = waves[current_wave].first->getChildByName("layout")->getChildByName("path_label");

	double enemy_delay = enemy_timer;
	if (enemy_name == "dangosimple") {
		enemy_name = "Simple";
	}
	else if (enemy_name == "dangobese") {
		enemy_name = "Obese";
	}
	ui::Layout* enemy = ui::Layout::create();
	char str[10]; //Change the size to meet your requirement
	sprintf(str, "%.2lf", enemy_delay);
	Label* delay = Label::createWithTTF(Value(str).asString(), "fonts/Love Is Complicated Again.ttf", 16);
	Label* name = Label::createWithTTF(enemy_name, "fonts/Love Is Complicated Again.ttf", 16);
	Label* lvl = Label::createWithTTF(Value(enemy_level + 1).asString(), "fonts/Love Is Complicated Again.ttf", 16);
	Label* path = Label::createWithTTF(Value((int)id_paths[enemy_path]).asString(), "fonts/Love Is Complicated Again.ttf", 16);
	delay->setColor(Color3B::BLACK);
	name->setColor(Color3B::BLACK);
	lvl->setColor(Color3B::BLACK);
	path->setColor(Color3B::BLACK);
	delay->setPosition(delay_label->getPosition().x, 0);
	name->setPosition(enemy_label->getPosition().x, 0);
	lvl->setPosition(lvl_label->getPosition().x, 0);
	path->setPosition(path_label->getPosition().x, 0);

	ui::Button* edit = ui::Button::create("res/buttons/edit.png");
	edit->addTouchEventListener(CC_CALLBACK_2(LevelEditor::editEnemy, this, id));
	ui::Button* duplicate = ui::Button::create("res/buttons/duplicate.png");
	duplicate->addTouchEventListener(CC_CALLBACK_2(LevelEditor::duplicateEnemy, this, id));
	ui::Button* delete_enemy = ui::Button::create("res/buttons/delete.png");
	delete_enemy->addTouchEventListener(CC_CALLBACK_2(LevelEditor::removeEnemy, this, id));
	delete_enemy->setScale(2.0f / 3.0f);
	delete_enemy->setPosition(Vec2(path_label->getPosition().x + path_label->getContentSize().width / 2 +
		delete_enemy->getContentSize().width * delete_enemy->getScaleX() / 2, 0));
	edit->setScale(2.0f / 3.0f);
	edit->setPosition(Vec2(delay->getPosition().x - delay->getContentSize().width / 2 -
		edit->getContentSize().width * edit->getScaleX() / 2, 0));
	duplicate->setScale(2.0f / 3.0f);
	duplicate->setPosition(Vec2(edit->getPosition().x - edit->getContentSize().width * 
		edit->getScaleX() / 2 -
		duplicate->getContentSize().width * duplicate->getScaleX() / 2, 0));

	enemy->addChild(name, 1, "name");
	enemy->addChild(delay, 1, "delay");
	enemy->addChild(lvl, 1, "lvl");
	enemy->addChild(path, 1, "path");
	enemy->addChild(delete_enemy, 1, "delete");
	enemy->addChild(duplicate, 1, "duplicate");
	enemy->addChild(edit, 1, "edit");
	waves[current_wave].first->getChildByName("layout")->addChild(enemy, 1, Value(id).asString());		
	updateDisplayWaveMenu(current_wave);
}

void LevelEditor::duplicateEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int enemy_id) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		double time = generator->getTime(enemies[enemy_id].first, enemies[enemy_id].second);
		std::string name = generator->getDango(enemies[enemy_id].first, enemies[enemy_id].second);
		enemy_path = generator->getPath(enemies[enemy_id].first, enemies[enemy_id].second);
		enemy_level = Value(name.substr(name.size() - 1, 1)).asInt();
		enemy_name = name.substr(0, name.size() - 1);
		
		int enemy_wave = enemies[enemy_id].second;
		int enemy_step = enemies[enemy_id].first;
		for (auto& enemy : enemies) {
			if (enemy.second.second == enemy_wave && enemy_step < enemy.second.first) {
				++enemy.second.first;
			}
		}
		generator->duplicateStep(enemy_step, enemy_wave);
		int next_id = enemies_id + 1;
		enemies[next_id] = std::make_pair(enemy_step + 1, enemy_wave);
		std::vector<std::pair<Node*,std::string>> wave_children;
		cocos2d::Vector<Node*> wave_all_children = waves[current_wave].first->getChildByName("layout")->getChildren();

		for (auto& enemy : wave_all_children) {
			if (enemy->getName() != "add_enemy" && enemy->getName() != "delay_label"
				&& enemy->getName() != "enemy_label" && enemy->getName() != "lvl_label" && enemy->getName() != "path_label") {
				int id = Value(enemy->getName()).asInt();
				if (enemies[id].first > enemy_step) {
					wave_children.push_back(std::make_pair(enemy, enemy->getName()));
					enemy->retain();
					waves[current_wave].first->getChildByName("layout")->removeChild(enemy);
				}
			}
		}
		createEnemyDisplay(next_id);
		for (auto& enemy : wave_children) {
			waves[current_wave].first->getChildByName("layout")->addChild(enemy.first, 1, enemy.second);
		}
		updateDisplayWaveMenu(current_wave);
		++enemies_id;
		enemy_name = "";
		enemy_level = -1;
		enemy_path = -1;
		c_enemy_id = -1;
		enemy_checkbox = nullptr;
	}
}
void LevelEditor::createDialogue(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {

}

void LevelEditor::editEnemyValues(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		double enemy_delay = enemy_timer;
		ui::Layout* enemy = (ui::Layout*)waves[current_wave].first->getChildByName("layout")->getChildByName(Value(c_enemy_id).asString());

		generator->setTime(enemy_delay, enemies[c_enemy_id].first, enemies[c_enemy_id].second);
		generator->setDango(enemy_name + Value(enemy_level).asString(), enemies[c_enemy_id].first,
			enemies[c_enemy_id].second);
		generator->setPath(enemy_path, enemies[c_enemy_id].first, enemies[c_enemy_id].second);
		
		char str[5]; //Change the size to meet your requirement
		sprintf(str, "%.2lf", enemy_delay);
		if (enemy_name == "dangosimple") {
			enemy_name = "Simple";
		}
		else if (enemy_name == "dangobese") {
			enemy_name = "Obese";
		}

		((Label*)enemy->getChildByName("delay"))->setString(Value(str).asString());
		((Label*)enemy->getChildByName("lvl"))->setString(Value(enemy_level + 1).asString());
		((Label*)enemy->getChildByName("name"))->setString(enemy_name);
		((Label*)enemy->getChildByName("path"))->setString(Value((int)id_paths[enemy_path]).asString());
		((Label*)enemy->getChildByName("path"))->setColor(Color3B::BLACK);
		hideEnemyBox(sender, type);
	}
}

void LevelEditor::editEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int enemy_id) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		updateDisplayPathButtons();
		c_enemy_id = enemy_id;
		double time = generator->getTime(enemies[enemy_id].first, enemies[enemy_id].second);
		std::string name = generator->getDango(enemies[enemy_id].first, enemies[enemy_id].second);
		enemy_path = generator->getPath(enemies[enemy_id].first, enemies[enemy_id].second);
		ui::TextField* enemy_time = ((ui::TextField*)getChildByName("select_enemy")->getChildByName("enemy_time"));
		enemy_time->setString(purgeCharactersFromString(Value(time).asString()));
		if (Value(enemy_time->getString()).asDouble() > 30) {
			enemy_time->setString("30");
		}
		getChildByName("select_enemy")->getChildByName("edit_enemy")->setVisible(true);
		getChildByName("select_enemy")->getChildByName("create_enemy")->setVisible(false);
		if (id_paths.size() > 0) {
			((ui::CheckBox*)getChildByName("select_enemy")->getChildByName("path_layout")->
				getChildByName("path_button" + Value((int)id_paths[enemy_path]).asString()))->setSelected(true);
		}
		else {
			((ui::Button*)getChildByName("select_enemy")->getChildByName("edit_enemy"))->setEnabled(false);
			enemy_path = -1;
		}

		enemy_checkbox = ((ui::CheckBox*)getChildByName("select_enemy")->getChildByName("enemy_layout")->getChildByName("checkbox_" + name));
		int level = Value(name.substr(name.size() - 1, 1)).asInt();
		name = name.substr(0, name.size() - 1);

		setCurrentEnemy(sender, type, name, level, enemy_checkbox);
		getChildByName("select_enemy")->setVisible(true);
		getChildByName("black_mask")->setVisible(true);
	}
}

void LevelEditor::removeEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int enemy_id) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		int enemy_wave = enemies[enemy_id].second;
		int enemy_step = enemies[enemy_id].first;
		for (auto& enemy : enemies) {
			if (enemy.second.second == enemy_wave && enemy_step < enemy.second.first ) {
				--enemy.second.first;
			}
		}
		enemies.erase(enemies.find(enemy_id));
		generator->removeStep(enemy_step, enemy_wave);
		waves[current_wave].first->getChildByName("layout")->removeChildByName(Value((int)enemy_id).asString());
		updateDisplayWaveMenu(enemy_wave);
	}
}

void LevelEditor::removeWave(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int wave) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		int enemy_wave = waves_id[wave];
		std::vector<int> enemy_to_delete;
		for (auto& wave : waves_id) {
			if (wave.second > enemy_wave) {
				--wave.second;
			}
		}
		for (auto& enemy : enemies) {
			if (enemy.second.second == enemy_wave) {
				enemy_to_delete.push_back(enemy.first);
			}
			if (enemy.second.second > enemy_wave) {
				--enemy.second.second;
			}
		}
		for (unsigned int i(0); i < enemy_to_delete.size(); ++i) {
			enemies.erase(enemies.find(enemy_to_delete[i]));
		}
		
		generator->removeWave(enemy_wave);
		subContents[3].first->getChildByName("layout")->removeChildByName("wave_" + Value((int)wave).asString());
		waves.erase(waves.begin() + waves_id[wave]);
		waves_id.erase(waves_id.find(wave));
		updateDisplayWaveMenu();
	}
}

void LevelEditor::saveLevelIntoDB(Ref* sender) {
	Json::StyledWriter writer;
	Json::Value root = generateJsonSave();
	std::string outputSave = writer.write(root);
	NetworkController* network = ((AppDelegate*)Application::getInstance())->getConfigClass()->getNetworkController();
	std::string postData = "&name=" + ((ui::TextField*)getChildByName("menu_left")->
		getChildByName("general_infos")->getChildByName("level_name"))->
		getString() + "&id_game=" + ((AppDelegate*)Application::getInstance())->getSave()["id_player"].asString() +
		"&content=" + outputSave;
	
	if (level_settings["id_bdd"].asInt() < 0) {
		postData = "action=create_level" + postData + "&id_level=" + level_settings["id_bdd"].asString();
		network->sendNewRequest(NetworkController::Request::LEVEL_EDITOR, postData,
			[&](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
			if (!response || !response->isSucceed()) {
				log("response failed");
				log("error buffer: %s", response->getErrorBuffer());
				return;
			}
			else {
				std::vector<char> *buffer = response->getResponseData();
				std::string str(buffer->begin(), buffer->end());
				level_settings["id_bdd"] = Value(str).asInt();
				level_settings["local"] = false;
				saveLevelIntoDB(nullptr);
			}
			saveLevelInFile(nullptr);
		}, "POST createLevel");
	}
	else {
		postData = "action=update" + postData + "&id_level=" + level_settings["id_bdd"].asString();
		network->sendNewRequest(NetworkController::Request::LEVEL_EDITOR, postData,
			[&](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
			if (!response || !response->isSucceed()) {
				log("response failed");
				log("error buffer: %s", response->getErrorBuffer());
				return;
			}
			else {
				std::vector<char> *buffer = response->getResponseData();
				std::string str(buffer->begin(), buffer->end());
				log("response: %s", str.c_str());
				log("level saved and updated");
				getChildByName("level_saved_label")->runAction(Sequence::create(FadeIn::create(0.1f),
					DelayTime::create(1.f), FadeOut::create(0.5f), nullptr));
			}
			saveLevelInFile(nullptr);
		}, "POST updateLevel");
	}
}

Json::Value LevelEditor::generateJsonSave() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Json::Value root;

	root["level_name"] = ((ui::TextField*)getChildByName("menu_left")->
		getChildByName("general_infos")->getChildByName("level_name"))->
		getString();

	root["resolution"]["width"] = visibleSize.width;
	root["resolution"]["height"] = visibleSize.height;

	for (unsigned int i(0); i < backgrounds.size(); ++i) {
		root["backgrounds"][i]["image_name"] = backgrounds[i].second;
		root["backgrounds"][i]["position"][0] = backgrounds[i].first->getPosition().x;
		root["backgrounds"][i]["position"][1] = backgrounds[i].first->getPosition().y;
		root["backgrounds"][i]["scale"][0] = backgrounds[i].first->getScaleX();
		root["backgrounds"][i]["scale"][1] = backgrounds[i].first->getScaleY();
		root["backgrounds"][i]["zorder"] = backgrounds[i].first->getLocalZOrder();
		root["backgrounds"][i]["rotation"] = backgrounds[i].first->getRotation();
		root["backgrounds"][i]["flipped"][0] = backgrounds[i].first->isFlippedX();
		root["backgrounds"][i]["flipped"][1] = backgrounds[i].first->isFlippedY();
	}
	for (unsigned int i(0); i < objects.size(); ++i) {
		root["objects"][i]["image_name"] = objects[i].second;
		root["objects"][i]["position"][0] = objects[i].first->getPosition().x;
		root["objects"][i]["position"][1] = objects[i].first->getPosition().y;
		root["objects"][i]["scale"][0] = objects[i].first->getScaleX();
		root["objects"][i]["scale"][1] = objects[i].first->getScaleY();
		root["objects"][i]["zorder"] = objects[i].first->getLocalZOrder();
		root["objects"][i]["rotation"] = objects[i].first->getRotation();
		root["objects"][i]["flipped"][0] = objects[i].first->isFlippedX();
		root["objects"][i]["flipped"][1] = objects[i].first->isFlippedY();
	}
	for (unsigned int i(0); i < paths.size(); ++i) {
		for (unsigned int j(0); j < paths[i].first.size(); ++j) {
			root["paths"][i]["path"][j][0] = paths[i].first[j]->getPosition().x;
			root["paths"][i]["path"][j][1] = paths[i].first[j]->getPosition().y;
		}
		root["paths"][i]["path_name"] = paths[i].second;
	}
	for (unsigned int i(0); i < locked_cells.size(); ++i) {
		root["locked_cell"][i]["pos"][0] = locked_cells[i].first.x;
		root["locked_cell"][i]["pos"][1] = locked_cells[i].first.y;
	}
	generator->saveInRoot(root);

	return root;
}

void LevelEditor::saveLevelInFile(Ref* sender) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	time_t now = time(0);
	tm *ltm = gmtime(&now);
	level_settings["local"] = true;
	level_settings["updated"] = Json::Value(1900 + ltm->tm_year).asString() +
		"-" + Json::Value(ltm->tm_mon).asString() +
		"-" + Json::Value(ltm->tm_mday).asString() +
		" " + Json::Value(ltm->tm_hour).asString() +
		":" + Json::Value(ltm->tm_min).asString() +
		":" + Json::Value(ltm->tm_sec).asString();

	Json::StyledWriter writer;
	Json::Value root = generateJsonSave();
	std::string outputSave = writer.write(root);

	std::string save_filename = ((ui::TextField*)getChildByName("menu_left")->
		getChildByName("general_infos")->getChildByName("level_name"))->
		getString() + ".json";
	bool level_folder_exist = FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "Levels/");
	if (!level_folder_exist) {
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "Levels/");
	}
	std::string path = FileUtils::getInstance()->getWritablePath() + "Levels/" + save_filename;
	bool succeed = FileUtils::getInstance()->writeStringToFile(outputSave, path);
	if (succeed) {
		Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave();
		if (save_root.isMember("local_editor_levels")) {
			int pos = -1;
			for (unsigned int i(0); i < save_root["local_editor_levels"].size(); ++i) {
				if (save_root["local_editor_levels"][i]["id"].asInt() == level_settings["id"].asInt()) {
					pos = i;
					break;
				}
			}
			if (pos != -1) {
				save_root["local_editor_levels"][pos] = level_settings;
			}
			else {
				save_root["local_editor_levels"][save_root["local_editor_levels"].size()] = level_settings;
			}
		}
		else {
			save_root["local_editor_levels"][0] = level_settings;
		}
		((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(save_root);
		((AppDelegate*)Application::getInstance())->getConfigClass()->save();

		log("Saved File in %s", path.c_str());
		getChildByName("level_saved_label")->runAction(Sequence::create(FadeIn::create(0.1f), 
			DelayTime::create(1.f), FadeOut::create(0.5f), nullptr));
	}
	else {
		log("error saving file %s", path.c_str());
	}
}

void LevelEditor::loadLevel(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == ui::Widget::TouchEventType::ENDED) {
		loaded = true;
		createLevel(this);
		level_settings = t_level_settings;
		if (level_settings["local"].asInt()) {
			loadLevelFromFileName();
		}
		else {
			loadLevelFromDB();
		}
		resetTemporaryLevelChoice();
		hideExistingLevels(sender, type);
	}
}

void LevelEditor::loadLevelFromFileName() {
	if (level_settings["name"].asString() != "") {
		auto fileUtils = FileUtils::getInstance();
		std::string path = fileUtils->getWritablePath();
		std::string level_config = fileUtils->getStringFromFile(path + "Levels/" + level_settings["name"].asString() + ".json");
		Json::Reader reader;
		Json::Value root;
		bool parsingConfigSuccessful = reader.parse(level_config, root, false);
		if (!parsingConfigSuccessful) {
			// report to the user the failure and their locations in the document.
			std::string error = reader.getFormattedErrorMessages();
		}
		else {
			loadLevelFromJson(root);
		}
	}
}

void LevelEditor::loadLevelFromDB() {
	NetworkController* network = ((AppDelegate*)Application::getInstance())->getConfigClass()->getNetworkController();

	network->sendNewRequest(NetworkController::Request::LEVEL_EDITOR, "action=get_level&id=" + Value(level_settings["id_bdd"].asString()).asString(),
		[&](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
		if (!response || !response->isSucceed()) {
			log("response failed");
			log("error buffer: %s", response->getErrorBuffer());
			return;
		}
		else {
			std::vector<char> *buffer = response->getResponseData();
			std::string str(buffer->begin(), buffer->end());
			Json::Reader reader;
			Json::Value root;
			bool parsingConfigSuccessful = reader.parse(str, root, false);
			if (!parsingConfigSuccessful) {
				// report to the user the failure and their locations in the document.
				std::string error = reader.getFormattedErrorMessages();
			}
			else {
				loadLevelFromJson(root);
			}
		}
	}, "POST getLevel");
}


void LevelEditor::loadLevelFromJson(Json::Value root) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	((ui::TextField*)getChildByName("menu_left")->getChildByName("general_infos")->
		getChildByName("level_name"))->setString(root["level_name"].asString());
	double ratio1 = sqrt((16.0 / 9.0) / (visibleSize.width / visibleSize.height));
	double ratio2 = sqrt((16.0 / 9.0) / (root["resolution"]["width"].asDouble() / root["resolution"]["height"].asDouble()));

	double ratio = (visibleSize.width * ratio1) / (root["resolution"]["width"].asDouble() * ratio2);
	for (unsigned int i(0); i < root["backgrounds"].size(); ++i) {
		Sprite* background = Sprite::create(root["backgrounds"][i]["image_name"].asString());
		background->setRotation(root["backgrounds"][i]["rotation"].asDouble());
		background->setPosition(root["backgrounds"][i]["position"][0].asDouble() * ratio,
			root["backgrounds"][i]["position"][1].asDouble() * ratio);
		background->setScaleX(root["backgrounds"][i]["scale"][0].asDouble() * ratio);
		background->setScaleY(root["backgrounds"][i]["scale"][1].asDouble() * ratio);
		background->setLocalZOrder(root["backgrounds"][i]["zorder"].asInt());
		background->setFlippedX(root["backgrounds"][i]["flipped"][0].asBool());
		background->setFlippedY(root["backgrounds"][i]["flipped"][1].asBool());
		backgrounds.push_back(std::make_pair(background, root["backgrounds"][i]["image_name"].asString()));
		getChildByName("level")->getChildByName("backgrounds")->addChild(background);
	}
	for (unsigned int i(0); i < root["objects"].size(); ++i) {
		std::string name = root["objects"][i]["image_name"].asString();
		Sprite* object = Sprite::createWithSpriteFrameName(root["objects"][i]["image_name"].asString());
		object->setRotation(root["objects"][i]["rotation"].asDouble());
		object->setPosition(root["objects"][i]["position"][0].asDouble() * ratio,
			root["objects"][i]["position"][1].asDouble() * ratio);
		object->setScaleX(root["objects"][i]["scale"][0].asDouble() * ratio);
		object->setScaleY(root["objects"][i]["scale"][1].asDouble() * ratio);
		object->setLocalZOrder(root["objects"][i]["zorder"].asInt());
		object->setFlippedX(root["objects"][i]["flipped"][0].asBool());
		object->setFlippedY(root["objects"][i]["flipped"][1].asBool());
		objects.push_back(std::make_pair(object, root["objects"][i]["image_name"].asString()));
		getChildByName("level")->getChildByName("objects")->addChild(object);
	}
	for (unsigned int i(0); i < root["paths"].size(); ++i) {
		createPath(this, ui::Widget::TouchEventType::ENDED);
		std::string path_name = root["paths"][i]["path_name"].asString();
		paths[i].second = path_name;
		for (unsigned int j(0); j < root["paths"][i]["path"].size(); ++j) {
			addPath(Vec2(root["paths"][i]["path"][j][0].asFloat() * ratio,
				root["paths"][i]["path"][j][1].asFloat() * ratio));
		}
	}
	for (unsigned int i(0); i < root["locked_cell"].size(); ++i) {
		Vec2 cell_pos = Vec2(root["locked_cell"][i]["pos"][0].asFloat() * ratio,
			root["locked_cell"][i]["pos"][1].asFloat() * ratio);
		Node* mask = Sprite::create("res/buttons/mask.png");
		mask->setScaleX(Cell::getCellWidth() / mask->getContentSize().width);
		mask->setScaleY(Cell::getCellWidth() / mask->getContentSize().height);
		mask->setPosition(cell_pos);
		getChildByName("level")->getChildByName("grid")->addChild(mask, 200);
		locked_cells.push_back(std::make_pair(cell_pos, mask));
	}
	for (int i(0); i < root["nbwaves"].asInt(); ++i) {
		createWave(this, cocos2d::ui::Widget::TouchEventType::ENDED);
		for (unsigned int j(0); j < root["dangosChain"][i].size(); ++j) {
			enemy_level = Value(root["dangosChain"][i][j].asString().substr(
				root["dangosChain"][i][j].asString().size() - 1,
				root["dangosChain"][i][j].asString().size())).asInt();
			enemy_name = root["dangosChain"][i][j].asString().substr(0, root["dangosChain"][i][j].asString().size() - 1);
			enemy_timer = root["dangosTime"][i][j].asDouble();
			enemy_path = root["dangosPath"][i][j].asDouble();
			current_wave = i;
			createEnemy(this, cocos2d::ui::Widget::TouchEventType::ENDED);
		}
	}
	path_selected = -1;
	textfield_selected = nullptr;
	getChildByName("select_level")->setVisible(false);
	((ui::Button*)getChildByName("select_level")->getChildByName("load_level"))->setEnabled(false);
	getChildByName("menu_left")->getChildByName("interface_manager")->getChildByName("selected_path_interface")->setVisible(false);
	getChildByName("menu_left")->getChildByName("interface_manager")->getChildByName("selected_sprite_interface")->setVisible(false);
}

void LevelEditor::setSelectedLevel(Ref* sender, cocos2d::ui::Widget::TouchEventType type, Json::Value nlevel_settings) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		if (nlevel_settings["id"].asInt() != t_level_settings["id"].asInt() || 
			nlevel_settings["id_bdd"].asInt() != t_level_settings["id_bdd"].asInt() || 
			nlevel_settings["local"].asBool() != t_level_settings["local"].asBool()) {
			t_level_settings = nlevel_settings;
			((ui::Widget*)getChildByName("select_level")->getChildByName("load_level"))->setEnabled(true);
		}
		else {
			resetTemporaryLevelChoice();
			((ui::Widget*)getChildByName("select_level")->getChildByName("load_level"))->setEnabled(false);
		}
		
	}
}

void LevelEditor::setCurrentEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type,
	std::string name, unsigned int level, cocos2d::ui::CheckBox* checkbox) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		if (name == enemy_name && level == enemy_level) {
			enemy_name = "";
			enemy_level = -1;
			checkbox->setSelected(false);
			((ui::Button*)getChildByName("select_enemy")->getChildByName("create_enemy"))->setEnabled(false);
			((ui::Button*)getChildByName("select_enemy")->getChildByName("edit_enemy"))->setEnabled(false);
		}
		else {
			if (enemy_checkbox != nullptr) {
				enemy_checkbox->setSelected(false);
			}
			checkbox->setSelected(true);
			enemy_checkbox = checkbox;
			enemy_name = name;
			enemy_level = level;
			if (enemy_path != -1) {
				((ui::Button*)getChildByName("select_enemy")->getChildByName("create_enemy"))->setEnabled(true);
				((ui::Button*)getChildByName("select_enemy")->getChildByName("edit_enemy"))->setEnabled(true);
			}
		}
	}
}

void LevelEditor::showExistingLevels(Ref* sender) {
	if (!getChildByName("select_level")->isVisible()) {
		updateLocalLevelsList();
		updateServerLevelsList();
		getChildByName("select_level")->setVisible(true);
		getChildByName("black_mask")->setVisible(true);
	}
	else {
		getChildByName("select_level")->setVisible(false);
		getChildByName("black_mask")->setVisible(false);
	}
}

void LevelEditor::updateLocalLevelsList() {
	Json::Value in_game_levels = getListLocalLevels();
	ui::ListView* scroll_view = ((ui::ListView*)getChildByName("select_level")->getChildByName("list_local_levels"));
	scroll_view->removeAllItems();
	double innerWidth = scroll_view->getContentSize().width;

	for (unsigned int i(0); i < in_game_levels.size(); ++i) {
		auto checkbox_level = cocos2d::ui::CheckBox::create("res/buttons/list_level_checkbox.png", "res/buttons/list_level_checkbox.png",
			"res/buttons/list_level_checkbox_selected.png", "res/buttons/list_level_checkbox.png", "res/buttons/list_level_checkbox.png");
		checkbox_level->setScaleX(innerWidth / checkbox_level->getContentSize().width * 0.9f);
		checkbox_level->setScaleY(scroll_view->getContentSize().height / checkbox_level->getContentSize().height * 0.20f);
		double height = checkbox_level->getContentSize().height * checkbox_level->getScaleX() - checkbox_level->getContentSize().height +
			checkbox_level->getContentSize().height * checkbox_level->getScaleY() / 5;

		scroll_view->setItemsMargin(height);
		
		Json::Value level_ids = in_game_levels[i];
		checkbox_level->addTouchEventListener([&, level_ids, checkbox_level](Ref* sender, ui::Widget::TouchEventType type) {
			setSelectedLevel(sender, type, level_ids);
			if (type == cocos2d::ui::Widget::TouchEventType::CANCELED) {
				checkbox_level->setSelected(!checkbox_level->isSelected());
			}
			else if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
				for (auto level : ((ui::ListView*)getChildByName("select_level")->getChildByName("list_local_levels"))->getItems()) {
					if (((ui::CheckBox*)level) != checkbox_level) {
						((ui::CheckBox*)level)->setSelected(false);
					}
				}
				for (auto level : ((ui::ListView*)getChildByName("select_level")->getChildByName("list_server_levels"))->getItems()) {
					if (((ui::CheckBox*)level) != checkbox_level) {
						((ui::CheckBox*)level)->setSelected(false);
					}
				}
			}
		});
		Label* level_name = Label::createWithTTF(in_game_levels[i]["name"].asString(), "fonts/arial.ttf", 35);
		level_name->setColor(Color3B::BLACK);
		level_name->setAlignment(TextHAlignment::CENTER);
		level_name->setPosition(Vec2(checkbox_level->getContentSize().width / 2, checkbox_level->getContentSize().height / 2.0f + 
			level_name->getContentSize().height / 2.0f));
		checkbox_level->addChild(level_name, 1, "level_name");
		Label* level_update = Label::createWithTTF(in_game_levels[i]["updated"].asString(), "fonts/arial.ttf", 20);
		level_update->setColor(Color3B(100, 100, 100));
		level_update->setAlignment(TextHAlignment::CENTER);
		level_update->setPosition(Vec2(checkbox_level->getContentSize().width / 2, level_name->getPosition().y - 
			level_name->getContentSize().height / 2.0f - level_update->getContentSize().height / 2.0f));
		checkbox_level->addChild(level_update, 1, "level_update");
		scroll_view->pushBackCustomItem(checkbox_level);
	}
}

void LevelEditor::updateServerLevelsList() {
	NetworkController* network = ((AppDelegate*)Application::getInstance())->getConfigClass()->getNetworkController();
	network->sendNewRequest(NetworkController::Request::LEVEL_EDITOR, "action=get_list_level",
		[&](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
		if (!response || !response->isSucceed()) {
			log("response failed");
			log("error buffer: %s", response->getErrorBuffer());
			return;
		}
		else {
			std::vector<char> *buffer = response->getResponseData();
			std::string str(buffer->begin(), buffer->end());
			Json::Reader reader;
			Json::Value root;
			bool parsingConfigSuccessful = reader.parse(str, root, false);
			if (!parsingConfigSuccessful) {
				// report to the user the failure and their locations in the document.
				std::string error = reader.getFormattedErrorMessages();
			}
			else {
				root = root["levels"];
				ui::ListView* scroll_view = ((ui::ListView*)getChildByName("select_level")->getChildByName("list_server_levels"));
				scroll_view->removeAllItems();
				double innerWidth = scroll_view->getContentSize().width;
				for (unsigned int i(0); i < root.size(); ++i) {
					auto checkbox_level = cocos2d::ui::CheckBox::create("res/buttons/list_level_checkbox.png", 
						"res/buttons/list_level_checkbox.png",
						"res/buttons/list_level_checkbox_selected.png", 
						"res/buttons/list_level_checkbox.png", 
						"res/buttons/list_level_checkbox.png");
					checkbox_level->setScaleX(innerWidth / checkbox_level->getContentSize().width * 0.9f);
					checkbox_level->setScaleY(scroll_view->getContentSize().height / 
						checkbox_level->getContentSize().height * 0.20f);
					double height = checkbox_level->getContentSize().height * checkbox_level->getScaleX() - 
						checkbox_level->getContentSize().height +
						checkbox_level->getContentSize().height * checkbox_level->getScaleY() / 5;

					scroll_view->setItemsMargin(height); 
					Json::Value level_settings;
					level_settings["id_bdd"] = root[i]["id"].asInt();
					level_settings["name"] = root[i]["name"].asString();
					level_settings["id"] = getLocalLevelIDWithBDDID(root[i]["id"].asInt());
					level_settings["local"] = false;
					level_settings["updated"] = root[i]["last_update"].asString();
					checkbox_level->addTouchEventListener([&, checkbox_level, level_settings](Ref* sender, ui::Widget::TouchEventType type) {
						setSelectedLevel(sender, type, level_settings);
						if (type == cocos2d::ui::Widget::TouchEventType::CANCELED) {
							checkbox_level->setSelected(!checkbox_level->isSelected());
						}
						else if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
							for (auto level : ((ui::ListView*)getChildByName("select_level")->getChildByName("list_local_levels"))->getItems()) {
								if (((ui::CheckBox*)level) != checkbox_level) {
									((ui::CheckBox*)level)->setSelected(false);
								}
							}
							for (auto level : ((ui::ListView*)getChildByName("select_level")->getChildByName("list_server_levels"))->getItems()) {
								if (((ui::CheckBox*)level) != checkbox_level) {
									((ui::CheckBox*)level)->setSelected(false);
								}
							}
						}
					});
					Label* level_name = Label::createWithTTF(root[i]["name"].asString(), "fonts/arial.ttf", 35);
					level_name->setColor(Color3B::BLACK);
					level_name->setAlignment(TextHAlignment::CENTER);
					level_name->setPosition(Vec2(checkbox_level->getContentSize().width / 2, checkbox_level->getContentSize().height / 2.0f + 
						level_name->getContentSize().height / 2.0f));
					checkbox_level->addChild(level_name, 1, "level_name");
					Label* level_update = Label::createWithTTF(level_settings["updated"].asString(), "fonts/arial.ttf", 20);
					level_update->setColor(Color3B(100,100,100));
					level_update->setAlignment(TextHAlignment::CENTER);
					level_update->setPosition(Vec2(checkbox_level->getContentSize().width / 2, level_name->getPosition().y -
						level_name->getContentSize().height / 2.0f - level_update->getContentSize().height / 2.0f));
					checkbox_level->addChild(level_update, 1, "level_update");
					scroll_view->pushBackCustomItem(checkbox_level);
				}
			}
		}
	}, "POST createLevel");
}

Json::Value LevelEditor::getListLocalLevels() {
	auto save = ((AppDelegate*)Application::getInstance())->getSave();
	return save["local_editor_levels"];
}

void LevelEditor::hideExistingLevels(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		getChildByName("select_level")->setVisible(false);
		((ui::Button*)getChildByName("select_level")->getChildByName("load_level"))->setEnabled(false);
		getChildByName("black_mask")->setVisible(false);
		resetTemporaryLevelChoice();
	}
}

void LevelEditor::hideDeleteConfirm(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		getChildByName("confirm_deletion")->setVisible(false);
	}
}
void LevelEditor::deleteCurrentLevel(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		hideDeleteConfirm(sender, type);
		FileUtils::getInstance()->removeFile(FileUtils::getInstance()->getWritablePath() + "Levels/" + level_settings["name"].asString() + ".json");
		
		if (level_settings["id_bdd"].asInt() >= 0) {
			NetworkController* network = ((AppDelegate*)Application::getInstance())->getConfigClass()->getNetworkController();
			network->sendNewRequest(NetworkController::Request::LEVEL_EDITOR, "action=delete_level&id_level="+level_settings["id_bdd"].asString(),
				[&](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
				if (!response || !response->isSucceed()) {
					log("response failed");
					log("error buffer: %s", response->getErrorBuffer());
					return;
				}
				else {
					std::vector<char> *buffer = response->getResponseData();
					std::string str(buffer->begin(), buffer->end());
				}
			}, "POST deleteLevel");
		}
		Json::Value save_root = ((AppDelegate*)Application::getInstance())->getSave();
		if (save_root.isMember("local_editor_levels")) {
			Json::Value new_save_levels;
			for (unsigned int i(0); i < save_root["local_editor_levels"].size(); ++i) {
				if (save_root["local_editor_levels"][i]["id"].asInt() != level_settings["id"].asInt()) {
					new_save_levels["local_editor_levels"].append(save_root["local_editor_levels"][i]);
				}
			}
			save_root["local_editor_levels"] = new_save_levels["local_editor_levels"];
		}
		((AppDelegate*)Application::getInstance())->getConfigClass()->setSave(save_root);
		((AppDelegate*)Application::getInstance())->getConfigClass()->save();

		resetLevel();
		loaded = false;
		getChildByName("menu_left")->getChildByName("interface_manager")->setVisible(false);
		getChildByName("menu_left")->getChildByName("scroll_menu")->setVisible(false);
		getChildByName("menu_left")->getChildByName("general_infos")->setVisible(false);
		((MenuItem*)getChildByName("menu_left")->getChildByName("main_menu")->getChildByName("save"))->setEnabled(false);
		((MenuItem*)getChildByName("menu_left")->getChildByName("main_menu")->getChildByName("del"))->setEnabled(false);
	}
}
void LevelEditor::showDeleteConfirm(Ref* sender) {
	getChildByName("confirm_deletion")->setVisible(true);
	std::string level_name = level_settings["name"].asString();
	level_name.replace(0, level_name.find_last_of('/') + 1, "");
	((Label*)getChildByName("confirm_deletion")->getChildByName("question"))->setString("Do you want to delete the level :\n " + level_name);
}

void LevelEditor::showWave(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int id) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		for (unsigned int i(0); i < waves.size(); ++i) {
			if (i != waves_id[id]) {
				waves[i].first->getChildByName("minus")->setVisible(false);
				waves[i].first->getChildByName("plus")->setVisible(true);
				waves[i].second = false;
				waves[i].first->getChildByName("layout")->setVisible(false);
				current_wave = -1;
			}
		}
		if (waves[waves_id[id]].second) {
			waves[waves_id[id]].first->getChildByName("minus")->setVisible(false);
			waves[waves_id[id]].first->getChildByName("plus")->setVisible(true);
			waves[waves_id[id]].second = false;
			waves[waves_id[id]].first->getChildByName("layout")->setVisible(false);
			current_wave = -1;
		}
		else {
			waves[waves_id[id]].first->getChildByName("minus")->setVisible(true);
			waves[waves_id[id]].first->getChildByName("plus")->setVisible(false);
			waves[waves_id[id]].second = true;
			waves[waves_id[id]].first->getChildByName("layout")->setVisible(true);
			current_wave = waves_id[id];
		}
		updateDisplayWaveMenu(waves_id[id]);
	}
}

void LevelEditor::showSubContent(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int id) {
	switch (type)
	{
	case ui::Widget::TouchEventType::BEGAN:
		break;
	case ui::Widget::TouchEventType::ENDED:
		for (unsigned int i(0); i < subContents.size(); ++i) {
			if (i != id) {
				subContents[i].first->getChildByName("minus")->setVisible(false);
				subContents[i].first->getChildByName("plus")->setVisible(true);
				subContents[i].second = false;
			}
		}
		if (subContents[id].second) {
			subContents[id].first->getChildByName("minus")->setVisible(false);
			subContents[id].first->getChildByName("plus")->setVisible(true);
			subContents[id].second = false;
		}
		else {
			subContents[id].first->getChildByName("minus")->setVisible(true);
			subContents[id].first->getChildByName("plus")->setVisible(false);
			subContents[id].second = true;
		}
		updateDisplayScrollingMenu();
		break;
	default:
		break;
	}
}

void LevelEditor::allowLockingCell(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		selecting_cell = !selecting_cell;
	}
}

void LevelEditor::update(float dt) {
	if (loaded) {
		reorder();
	}
}


void LevelEditor::updateDisplayScrollingMenu() {
	ui::ScrollView* scroll_view = ((ui::ScrollView*)getChildByName("menu_left")->getChildByName("scroll_menu"));
	double innerWidth = scroll_view->getInnerContainerSize().width;
	double offset = innerWidth * 2 / 20;
	double innerHeight = 0;
	for (unsigned int i(0); i < subContents.size(); ++i) {
		if (subContents[i].second) {
			double scale = subContents[i].first->getScaleY();
			double height_temp = getNodeHeight(subContents[i].first->getChildByName("layout"));
			double height_panel = subContents[i].first->getContentSize().height;
			innerHeight += getNodeHeight(subContents[i].first->getChildByName("layout")) * subContents[i].first->getScaleY() +
				subContents[i].first->getContentSize().height * subContents[i].first->getScaleY() +
				offset / 2;
		}
		else {
			double scale = subContents[i].first->getScaleY();
			double height_temp = subContents[i].first->getContentSize().height;
			innerHeight += subContents[i].first->getContentSize().height * subContents[i].first->getScaleY() + 
				offset / 2;
		}
	}
	//innerHeight *= 2;
	innerHeight = innerHeight > scroll_view->getContentSize().height ?
		innerHeight : scroll_view->getContentSize().height;
	scroll_view->setInnerContainerSize(Size(innerWidth, innerHeight));
	for (unsigned int i(0); i < subContents.size(); ++i) {
		double height = getNodeHeight(subContents[i].first->getChildByName("layout"));
		subContents[i].first->getChildByName("layout")->setContentSize(Size(innerWidth, height));
		if (i > 0) {
			if (subContents[i - 1].second) {
				((ui::Button*)subContents[i].first)->setPosition(Vec2(innerWidth / 2,
					subContents[i - 1].first->getPosition().y -
					subContents[i - 1].first->getContentSize().height * subContents[i].first->getScaleY() +
					subContents[i - 1].first->getChildByName("layout")->getPosition().y -
					subContents[i - 1].first->getChildByName("layout")->getContentSize().height * subContents[i - 1].first->getScaleY() -
					offset / 2));
			}
			else {
				((ui::Button*)subContents[i].first)->setPosition(Vec2(innerWidth / 2,
					subContents[i - 1].first->getPosition().y -
					subContents[i - 1].first->getContentSize().height * subContents[i - 1].first->getScaleY() -
					offset / 2));
			}
		}
		else {
			subContents[i].first->setPosition(Vec2(innerWidth / 2, innerHeight - offset / 2));
		}
		subContents[i].first->getChildByName("layout")->setVisible(subContents[i].second);
	}
}

void LevelEditor::updateDisplayWaveMenu(unsigned int wave_index) {
	Node* add_enemy = waves[wave_index].first->getChildByName("layout")->getChildByName("add_enemy");
	Node* delay_label = waves[wave_index].first->getChildByName("layout")->getChildByName("delay_label");
	Node* enemy_label = waves[wave_index].first->getChildByName("layout")->getChildByName("enemy_label");
	Node* lvl_label = waves[wave_index].first->getChildByName("layout")->getChildByName("lvl_label");
	Node* path_label = waves[wave_index].first->getChildByName("layout")->getChildByName("path_label");

	double offset = add_enemy->getContentSize().height * add_enemy->getScaleY() / 4;
	
	Vec2 lastPos = Vec2(0, -offset);
	for (auto& child : waves[wave_index].first->getChildByName("layout")->getChildren()) {
		if (child != add_enemy && child != delay_label && child != enemy_label && child && lvl_label && child != path_label) {
			child->setPosition(lastPos.x, lastPos.y - getNodeHeight(child) / 2 - offset / 10);
			lastPos = Vec2(child->getPosition().x, child->getPosition().y - getNodeHeight(child) / 2 -
				offset / 10);
		}
	}
	add_enemy->setPosition(-offset, lastPos.y - 
		add_enemy->getContentSize().height * add_enemy->getScaleY() / 2);
	double total_label_width = delay_label->getContentSize().width / 2 + enemy_label->getContentSize().width +
		lvl_label->getContentSize().width + path_label->getContentSize().width + 3 * offset;
	delay_label->setPosition(-total_label_width / 2, 0);
	enemy_label->setPosition(delay_label->getPosition().x + delay_label->getContentSize().width / 2 + 
		enemy_label->getContentSize().width / 2 + offset, 0);
	lvl_label->setPosition(enemy_label->getPosition().x + enemy_label->getContentSize().width / 2 +
		lvl_label->getContentSize().width / 2 + offset, 0);
	path_label->setPosition(lvl_label->getPosition().x + lvl_label->getContentSize().width / 2 +
		path_label->getContentSize().width / 2 + offset, 0);


	Node* add_wave = subContents[3].first->getChildByName("layout")->getChildByName("add_wave");

	ui::Layout* view = ((ui::Layout*)subContents[3].first->getChildByName("layout"));
	double innerWidth = view->getContentSize().width;
	offset = innerWidth * 2 / 20;

	innerWidth = view->getContentSize().width;
	offset = add_wave->getContentSize().height * add_wave->getScaleY() / 4;
	Vec2 last_pos = Vec2(subContents[3].first->getContentSize().width / 2, 
		add_wave->getContentSize().height * add_wave->getScaleY() + offset / 2);
	for (unsigned int i(0); i < waves.size(); ++i) {
		double height = getNodeHeight(waves[i].first->getChildByName("layout"));
		//waves[i].first->getChildByName("layout")->setContentSize(Size(innerWidth, height));
		if (waves[i].second) {
			((ui::Button*)waves[i].first)->setPosition(Vec2(last_pos.x,
				last_pos.y + getNodeHeight(waves[i].first) / 2 + height + offset / 10));
			last_pos = Vec2(waves[i].first->getPosition().x, last_pos.y +
				getNodeHeight(waves[i].first) + height +
				offset / 10);
		}
		else {
			((ui::Button*)waves[i].first)->setPosition(Vec2(last_pos.x,
				last_pos.y + getNodeHeight(waves[i].first) / 2 + offset / 10));
			last_pos = Vec2(waves[i].first->getPosition().x, last_pos.y +
				getNodeHeight(waves[i].first) + offset / 10);
		}
	}
	waves[wave_index].first->getChildByName("layout")->setVisible(waves[wave_index].second);
	add_wave->setPosition(last_pos.x, add_wave->getContentSize().height * add_wave->getScaleY() / 2);

	updateDisplayScrollingMenu();
}


void LevelEditor::handleBackground(Ref* sender, cocos2d::ui::Widget::TouchEventType type, std::string image_name, ui::CheckBox* checkbox) {
	switch (type)
	{
	case ui::Widget::TouchEventType::BEGAN:
		break;
	case ui::Widget::TouchEventType::ENDED:
		Size visibleSize = Director::getInstance()->getVisibleSize();

		int deletion(-1);
		for (unsigned int i(0); i < backgrounds.size(); ++i) {
			if (backgrounds[i].second == image_name) {
				getChildByName("level")->getChildByName("backgrounds")->removeChild(backgrounds[i].first);
				deletion = i;
			}
		}
		if (deletion < 0) {
			Sprite* background = Sprite::create(image_name);
			double ratio = visibleSize.width / visibleSize.height;
			double max_width_resolution = 16.0 / 9.0;
			background->setScale((3.0 / 4.0) * visibleSize.width * sqrt((16.0 / 9.0) / ratio) / background->getContentSize().width);
			backgrounds.push_back(std::make_pair(background,image_name));
			getChildByName("level")->getChildByName("backgrounds")->addChild(background);
			checkbox->setSelected(true);
		}
		else {
			backgrounds.erase(backgrounds.begin() + deletion);
			checkbox->setSelected(false);
		}
		break;
	}
}
void LevelEditor::addObject(Ref* sender, cocos2d::ui::Widget::TouchEventType type, std::string image_name) {
	switch (type)
	{
	case ui::Widget::TouchEventType::BEGAN: 
		
		break;
	case ui::Widget::TouchEventType::ENDED:
		{
			Sprite* image = Sprite::createWithSpriteFrameName(image_name);
			selected_object = image;
			objects.push_back(std::make_pair(image, image_name));
			getChildByName("level")->getChildByName("objects")->addChild(image);
			getChildByName("menu_left")->getChildByName("interface_manager")->
				getChildByName("selected_sprite_interface")->setVisible(true);
		}
		break;
	default:
		break;
	}
}


void LevelEditor::drawResolutions(DrawNode* node, double resolution, Color4F color) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	double ratio = visibleSize.width / visibleSize.height;
	double width = visibleSize.width * 3 / 4;
	double height = visibleSize.height;
	node->drawLine(Vec2(-width * sqrt(resolution / ratio) / 2, -height * sqrt(ratio / resolution) / 2),
		Vec2(width * sqrt(resolution / ratio) / 2, -height * sqrt(ratio / resolution) / 2), color);
	node->drawLine(Vec2(-width * sqrt(resolution / ratio) / 2, height *  sqrt(ratio / resolution) / 2),
		Vec2(width *  sqrt(resolution / ratio) / 2, height *  sqrt(ratio / resolution) / 2), color);
	node->drawLine(Vec2(width *  sqrt(resolution / ratio) / 2, -height *  sqrt(ratio / resolution) / 2),
		Vec2(width *  sqrt(resolution / ratio) / 2, height *  sqrt(ratio / resolution) / 2), color);
	node->drawLine(Vec2(-width *  sqrt(resolution / ratio) / 2, height *  sqrt(ratio / resolution) / 2),
		Vec2(-width * sqrt(resolution / ratio) / 2, -height *  sqrt(ratio / resolution) / 2), color);
}

Vec2 LevelEditor::getPositionInGrid(Vec2 pos, bool half) {
	Vec2 grid_pos = pos;
	int x_index = round(grid_pos.x / size_cell *  (2 - !half));
	int y_index = -round(grid_pos.y / size_cell * (2 - !half));
	int width = 12 / (2 - half);
	int height = 10 / (2 - half);
	if (abs(x_index) < 12) {
		grid_pos.x = x_index * size_cell / 2 - grid_offset.x;
	}
	else {
		grid_pos.x = (!std::signbit(grid_pos.x) * 2 - 1) * width * size_cell / (2 - !half) - grid_offset.x;
	}
	if (abs(y_index) < 10) {
		grid_pos.y = y_index * size_cell / 2 - grid_offset.y ;
	}
	else {
		grid_pos.y = (std::signbit(grid_pos.y) * 2 - 1) * height * size_cell / (2 - !half) - grid_offset.y;
	}
	return grid_pos;
}

void LevelEditor::reorder() {
	std::vector<Node*> elements;
	for (auto& object : objects) {
		if (object.first != nullptr)
			elements.push_back(object.first);
	}
	std::stable_sort(elements.begin(), elements.end(), sortZOrder);
	int i = 0;
	for (auto& object : objects) {
		object.first->setLocalZOrder(i);
		++i;
	}
}

void LevelEditor::resetLevel() {
	getChildByName("level")->getChildByName("backgrounds")->removeAllChildren();
	getChildByName("level")->getChildByName("objects")->removeAllChildren();
	getChildByName("level")->getChildByName("paths")->removeAllChildren();
	getChildByName("level")->removeChildByName("grid");
	getChildByName("level")->removeChildByName("center");
	getChildByName("level")->removeChildByName("resolutions");
	auto add_path = ((ui::Button*)subContents[2].first->getChildByName("layout")->getChildByName("add_path"))->clone();
	auto add_wave = ((ui::Button*)subContents[3].first->getChildByName("layout")->getChildByName("add_wave"))->clone();
	subContents[2].first->getChildByName("layout")->removeAllChildren();
	subContents[2].first->getChildByName("layout")->addChild(add_path);
	subContents[3].first->getChildByName("layout")->removeAllChildren();
	subContents[3].first->getChildByName("layout")->addChild(add_wave);
	updateDisplayPathMenu();

	backgrounds.clear();
	objects.clear();
	paths.clear();
	waves.clear();
	subWavesContent.clear();
	enemies.clear();
	waves_id.clear();
	id_paths.clear();
	generator->empty();
	locked_cells.clear();

	resetLevelSettings();
	enemy_level = -1;
	enemy_name = "";
	enemy_timer = -1;
	enemy_checkbox = nullptr;
	current_wave = -1;
	enemies_id = 0;
	wave_id = 0;
	c_enemy_id = -1;
}

void LevelEditor::addPath(Vec2 p) {
	getChildByName("menu_left")->getChildByName("interface_manager")->
		getChildByName("selected_path_interface")->setVisible(true);

	Vec2 grid_pos = p;
	int x_index = round((grid_pos.x + size_cell / 2.0 )/ size_cell);
	int y_index = round((grid_pos.y + size_cell / 2.0)/ size_cell);
	grid_pos.x = x_index * size_cell - grid_offset.x - size_cell / 2.0;
	grid_pos.y = y_index * size_cell - grid_offset.y - size_cell / 2.0;
	
	bool is_in_path(false);
	for (unsigned int i(0); i < paths[path_selected].first.size(); ++i) {
		if (paths[path_selected].first[i]->getPosition() == grid_pos) {
			is_in_path = true;
		}
	}
	if (is_in_path) {
		if (paths[path_selected].first.size() > 3) {
			if(paths[path_selected].first[paths[path_selected].first.size() - 1]->getPosition() != grid_pos &&
				paths[path_selected].first[paths[path_selected].first.size() - 2]->getPosition() != grid_pos &&
				paths[path_selected].first[paths[path_selected].first.size() - 3]->getPosition() != grid_pos) {
				is_in_path = false;
			}
		}
	}
	if (!is_in_path) {
		bool is_next_cell(false);
		if (paths[path_selected].first.size() > 0) {
			for (int i(-1); i <= 1; i += 1) {
				for (int j(-1); j <= 1; j += 1) {
					if ((i == 0 && j != 0) || (i != 0 && j == 0)) {
						if (abs(grid_pos.x - paths[path_selected].first[paths[path_selected].first.size() - 1]->getPosition().x + i*size_cell) < 1 &&
							abs(grid_pos.y - paths[path_selected].first[paths[path_selected].first.size() - 1]->getPosition().y + j*size_cell) < 1) {
							is_next_cell = true;
						}
					}

				}
			}
		}
		else {
			is_next_cell = true;
		}
		
		if (is_next_cell) {
			Sprite* path = Sprite::create("res/buttons/path_arrow.png");
			path->setScale(size_cell / path->getContentSize().width);
			path->setPosition(grid_pos);
			getChildByName("level")->getChildByName("paths")->addChild(path);
			paths[path_selected].first.push_back(path);
			bool turn = false;
			if (paths[path_selected].first.size() > 0) {
				if (paths[path_selected].first.size() > 2) {
					Direction direction_pp = getDirectionFromPos(paths[path_selected].first[paths[path_selected].first.size() - 3]->getPosition(),
						paths[path_selected].first[paths[path_selected].first.size() - 2]->getPosition());
					Direction direction_p = getDirectionFromPos(paths[path_selected].first[paths[path_selected].first.size() - 2]->getPosition(),
						paths[path_selected].first[paths[path_selected].first.size() - 1]->getPosition());
					if (direction_pp != direction_p) {
						turn = true;
						Sprite* npath = Sprite::create("res/buttons/path_turn.png");
						npath->setScale(size_cell / npath->getContentSize().width);
						npath->setPosition(paths[path_selected].first[paths[path_selected].first.size() - 2]->getPosition());
						getChildByName("level")->getChildByName("paths")->removeChild(paths[path_selected].first[paths[path_selected].first.size() - 2]);
						getChildByName("level")->getChildByName("paths")->addChild(npath);
						paths[path_selected].first[paths[path_selected].first.size() - 2] = npath;
						if ((direction_pp == RIGHT && direction_p == UP) || (direction_pp == DOWN && direction_p == LEFT)) {
							npath->setFlippedY(true);
						}
						else if ((direction_pp == LEFT && direction_p == DOWN) || 
							(direction_pp == UP && direction_p == RIGHT)){
							npath->setFlippedX(true);
						}
						else if ((direction_pp == LEFT && direction_p == UP) ||
							(direction_pp == DOWN && direction_p == RIGHT)) {
							npath->setFlippedX(true);
							npath->setFlippedY(true);
						}
					}
				}
				if (paths[path_selected].first.size() > 1) {
					Direction direction_p = getDirectionFromPos(paths[path_selected].first[paths[path_selected].first.size() - 2]->getPosition(),
						paths[path_selected].first[paths[path_selected].first.size() - 1]->getPosition());
					if (direction_p == DOWN) {
						path->setRotation(90.0f);
					}
					else if (direction_p == UP) {
						path->setRotation(-90.0f);
					}
					else if (direction_p == LEFT) {
						path->setFlippedX(true);
					}
					if (!turn) {
						Sprite* npath = Sprite::create("res/buttons/path_straight.png");
						npath->setScale(size_cell / npath->getContentSize().width);
						npath->setPosition(paths[path_selected].first[paths[path_selected].first.size() - 2]->getPosition());
						getChildByName("level")->getChildByName("paths")->removeChild(paths[path_selected].first[paths[path_selected].first.size() - 2]);
						getChildByName("level")->getChildByName("paths")->addChild(npath);
						paths[path_selected].first[paths[path_selected].first.size() - 2] = npath;
						if (direction_p == DOWN) {
							npath->setRotation(90.0f);
						}
						else if (direction_p == UP) {
							npath->setRotation(-90.0f);
						}
						else if (direction_p == LEFT) {
							npath->setFlippedX(true);
						}
					}
				}
				((ui::Button*)getChildByName("menu_left")->getChildByName("interface_manager")->
					getChildByName("selected_path_interface")->getChildByName("remove_path"))->setEnabled(true);
			}
		}
	}
}

void LevelEditor::validatePath(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		getChildByName("menu_left")->getChildByName("interface_manager")->getChildByName("selected_path_interface")->setVisible(false);
		path_selected = -1;
	}
}
void LevelEditor::removePathTile(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		if (paths[path_selected].first.size() > 0) {
			getChildByName("level")->getChildByName("paths")->removeChild(paths[path_selected].first[paths[path_selected].first.size() - 1]);
			paths[path_selected].first.pop_back();
			if (paths[path_selected].first.size() == 0) {
				((ui::Button*)getChildByName("menu_left")->getChildByName("interface_manager")->
					getChildByName("selected_path_interface")->getChildByName("remove_path"))->setEnabled(false);
			}
		}
	}
}
void LevelEditor::removePath(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int id) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		if (id != -1) {
			for (unsigned int i(0); i < id_paths.size(); ++i) {
				if (id_paths[i] == id) {
					path_selected = i;
				}
			}
		}
		for (unsigned int i(0); i < paths[path_selected].first.size(); ++i) {
			getChildByName("level")->getChildByName("paths")->removeChild(paths[path_selected].first[i]);
		}
		for (auto enemy: enemies) {
			if (id_paths[generator->getPath(enemy.second.first, enemy.second.second)] == id) {
				((Label*)waves[waves_id[enemy.second.second]].first->getChildByName("layout")->
					getChildByName(Value(enemy.first).asString())->getChildByName("path"))->setColor(Color3B::RED);
			}
			else if (id_paths[generator->getPath(enemy.second.first, enemy.second.second)] > id) {
				generator->setPath(generator->getPath(enemy.second.first, enemy.second.second) - 1, enemy.second.first, enemy.second.second);
			}
		}

		subContents[2].first->getChildByName("layout")->removeChildByName(paths[path_selected].second);
		updateDisplayPathMenu();
		updateDisplayScrollingMenu();
		paths.erase(paths.begin() + path_selected);
		id_paths.erase(id_paths.begin() + path_selected);
		path_selected = -1;
		getChildByName("menu_left")->getChildByName("interface_manager")->
			getChildByName("selected_path_interface")->setVisible(false);
	}
}
void LevelEditor::inversePath(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		std::vector<Sprite*> new_path;
		for (int i(paths[path_selected].first.size() - 1); i >= 0 ; --i) {
			new_path.push_back(paths[path_selected].first[i]);
		}
		paths[path_selected].first = new_path;
		if (paths[path_selected].first.size() > 0) {
			Sprite* npath = Sprite::create("res/buttons/path_straight.png");
			npath->setScale(size_cell / npath->getContentSize().width);
			npath->setPosition(paths[path_selected].first[0]->getPosition());
			getChildByName("level")->getChildByName("paths")->removeChild(paths[path_selected].first[0]);
			getChildByName("level")->getChildByName("paths")->addChild(npath);
			paths[path_selected].first[0] = npath;
			if (paths[path_selected].first.size() > 1) {
				Direction direction_p = getDirectionFromPos(paths[path_selected].first[0]->getPosition(),
					paths[path_selected].first[1]->getPosition());
				if (direction_p == DOWN) {
					npath->setRotation(90.0f);
				}
				else if (direction_p == UP) {
					npath->setRotation(-90.0f);
				}
				else if (direction_p == LEFT) {
					npath->setFlippedX(true);
				}
			}
		}
		if (paths[path_selected].first.size() > 1) {
			Sprite* npath = Sprite::create("res/buttons/path_arrow.png");
			npath->setScale(size_cell / npath->getContentSize().width);
			npath->setPosition(paths[path_selected].first[paths[path_selected].first.size() - 1]->getPosition());
			getChildByName("level")->getChildByName("paths")->removeChild(paths[path_selected].first[paths[path_selected].first.size() - 1]);
			getChildByName("level")->getChildByName("paths")->addChild(npath);
			paths[path_selected].first[paths[path_selected].first.size() - 1] = npath;
			Direction direction_p = getDirectionFromPos(paths[path_selected].first[paths[path_selected].first.size() - 2]->getPosition(),
				paths[path_selected].first[paths[path_selected].first.size() - 1]->getPosition());
			if (direction_p == DOWN) {
				npath->setRotation(90.0f);
			}
			else if (direction_p == UP) {
				npath->setRotation(-90.0f);
			}
			else if (direction_p == LEFT) {
				npath->setFlippedX(true);
			}
		}
		
	}
}
LevelEditor::Direction LevelEditor::getDirectionFromPos(Vec2 pos1, Vec2 pos2) {
	if (abs(pos2.x - pos1.x) < 1) {
		if (pos2.y - pos1.y > 0) {
			return UP;
		}
		else {
			return DOWN;
		}
	}
	else {
		if (pos2.x - pos1.x > 0) {
			return RIGHT;
		}
		else {
			return LEFT;
		}
	}
}

void LevelEditor::updateDisplayPathMenu() {
	Node* add_path = subContents[2].first->getChildByName("layout")->getChildByName("add_path");
	Vec2 lastPos = Vec2(subContents[2].first->getContentSize().width / 2, 0);
	double offset = add_path->getContentSize().height * add_path->getScaleY() / 4;
	for (auto& child : subContents[2].first->getChildByName("layout")->getChildren()) {
		if (child == add_path) {
			child->setPosition(lastPos.x, lastPos.y + add_path->getContentSize().height * add_path->getScaleY() / 2 + offset / 2);
			lastPos = Vec2(child->getPosition().x, child->getPosition().y + add_path->getContentSize().height * add_path->getScaleY() / 2 + offset / 2);
		}
		else {
			child->setPosition(lastPos.x, lastPos.y + getNodeHeight(child) / 2 + offset / 2);
			lastPos = Vec2(child->getPosition().x, child->getPosition().y + getNodeHeight(child) / 2 + offset / 2);
		}
	}
	updateDisplayScrollingMenu();
}

void LevelEditor::updateDisplayWaveMenu() {
	Node* add_wave = subContents[3].first->getChildByName("layout")->getChildByName("add_wave");
	
	double offset = add_wave->getContentSize().height * add_wave->getScaleY() / 4;
	Vec2 lastPos = Vec2(subContents[3].first->getContentSize().width / 2, 
		add_wave->getContentSize().height * add_wave->getScaleY() + offset / 2);
	for (auto& child : subContents[3].first->getChildByName("layout")->getChildren()) {
		if (child != add_wave) {
			child->setPosition(lastPos.x, lastPos.y + getNodeHeight(child) / 2 + offset / 10);
			lastPos = Vec2(child->getPosition().x, child->getPosition().y + getNodeHeight(child) / 2 + 
				offset / 10);
		}
	}
	add_wave->setPosition(lastPos.x,add_wave->getContentSize().height * add_wave->getScaleY() / 2);

	updateDisplayScrollingMenu();
}

void LevelEditor::selectPath(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int id) {
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
		for (unsigned int i(0); i < id_paths.size(); ++i) {
			if (id_paths[i] == id) {
				path_selected = i;
			}
		}
		getChildByName("menu_left")->getChildByName("interface_manager")->getChildByName("selected_path_interface")->setVisible(true);

	}
}

std::vector <std::string> geFilenamesFromDirectory(const std::string& path){
	std::vector <std::string> result;
	auto fileUtils = FileUtils::getInstance();

	std::string level_config = fileUtils->getStringFromFile(path);
	Json::Reader reader;
	Json::Value root;
	bool parsingConfigSuccessful = reader.parse(level_config, root, false);
	if (!parsingConfigSuccessful) {
		// report to the user the failure and their locations in the document.
		std::string error = reader.getFormattedErrorMessages();
	}
	else {
		for (auto filename : root["files"]) {
			result.push_back(filename.asString());
		}
	}
	std::sort(result.begin(), result.end());
	return result;
}

double getNodeHeight(Node* node) {
	Vec2 height;
	for (auto child : node->getChildren()) {
		double child_height(child->getContentSize().height);
		if (child->getChildrenCount() != 0) {
			//child_height = getNodeHeight(child);
		}
		Vec2 c_height = Vec2(child->getPosition().y - child_height*(1 - child->getAnchorPoint().y) * child->getScaleY(),
			child->getPosition().y + child_height*(1 - child->getAnchorPoint().y) * child->getScaleY());
		if (c_height.x < height.x) {
			height.x = c_height.x;
		}
		if (c_height.y > height.y) {
			height.y = c_height.y;
		}
	}
	return (height.y - height.x) * node->getScaleY();
}

std::string purgeCharactersFromString(std::string s) {
	s.erase(std::remove_if(s.begin(), s.end(), &isCharacter), s.end());
	return s;
}
bool isCharacter(char car) {
	if (((int)car >= 48 && (int)car < 58) || car == 46) {
		return false;
	}
	return true;
}

void LevelEditor::updateSelectionBox() {
	if (selected_object != nullptr) {
		if (getChildByName("level")->getChildByName("selection_box")->getChildren().size() == 0 || abs(getChildByName("level")->getChildByName("selection_box")->getChildByName("node")->getContentSize().width
			- selected_object->getScaleX()*selected_object->getContentSize().width) > 2 ||
			abs(getChildByName("level")->getChildByName("selection_box")->getChildByName("node")->getContentSize().height
				- selected_object->getScaleY()*selected_object->getContentSize().height) > 2 ) {

			getChildByName("level")->getChildByName("selection_box")->removeAllChildren();

			DrawNode* node = DrawNode::create();
			Vec2 rectangle[4];
			rectangle[0] = Vec2(-selected_object->getScaleX()*selected_object->getContentSize().width / 2,
				-selected_object->getScaleY()*selected_object->getContentSize().height / 2);
			rectangle[1] = Vec2(selected_object->getScaleX()*selected_object->getContentSize().width / 2,
				-selected_object->getScaleY()*selected_object->getContentSize().height / 2);
			rectangle[2] = Vec2(selected_object->getScaleX()*selected_object->getContentSize().width / 2,
				selected_object->getScaleY()*selected_object->getContentSize().height / 2);
			rectangle[3] = Vec2(-selected_object->getScaleX()*selected_object->getContentSize().width / 2,
				selected_object->getScaleY()*selected_object->getContentSize().height / 2);

			Color4F white(1, 1, 1, 0.1f);
			Color4F red(1, 0, 0, 1.f);

			node->drawPolygon(rectangle, 4, white, 1, red);
			getChildByName("level")->getChildByName("selection_box")->addChild(node, 1, "node");
		}
		if (selected_object->getPosition() != getChildByName("level")->getChildByName("selection_box")->getPosition()) {
			getChildByName("level")->getChildByName("selection_box")->setPosition(selected_object->getPosition());
		}
	}
	else {
		getChildByName("level")->getChildByName("selection_box")->removeAllChildren();
	}
}

int LevelEditor::getLocalLevelIDWithBDDID(int id_bdd) {
	auto save = ((AppDelegate*)Application::getInstance())->getSave();
	if (save.isMember("local_editor_levels")) {
		for (unsigned int i(0); i < save["local_editor_levels"].size(); ++i) {
			if (save["local_editor_levels"][i]["id_bdd"].asInt() == id_bdd) {
				return save["local_editor_levels"][i]["id"].asInt();
			}
		}
	}
	return -1;
}

void LevelEditor::resetTemporaryLevelChoice() {
	t_level_settings["id_bdd"] = -1;
	t_level_settings["name"] = "";
	t_level_settings["id"] = -1;
	t_level_settings["local"] = true;
}

void LevelEditor::resetLevelSettings() {
	level_settings["id_bdd"] = -1;
	level_settings["name"] = "";
	level_settings["id"] = -1;
	level_settings["local"] = true;
}

