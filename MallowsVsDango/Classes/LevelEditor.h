#ifndef LEVELEDITOR_HPP
#define LEVELEDITOR_HPP

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Level/DangoGenerator.h"
#include "Dialogue.h"

std::vector <std::string> read_directory(const std::string& path);
bool sortZOrder(cocos2d::Node* sprite1, cocos2d::Node* sprite2);
double getNodeHeight(cocos2d::Node* node);
std::string purgeCharactersFromString(std::string s);
bool isCharacter(char car);

class LevelEditor : public cocos2d::Scene
{
	enum Direction {
		LEFT,
		RIGHT,
		UP,
		DOWN
	};
public:
	virtual bool init();
	CREATE_FUNC(LevelEditor);
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();

	void returnToGame(Ref* sender);
	void hideShowMenu(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void zoomIn(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void zoomOut(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void showGrid(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void showResolutions(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void showBackgrounds(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void showObjects(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void showPath(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	
	virtual void update(float dt);

	void createLevel(Ref* sender);
	void createPath(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void createWave(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void createEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void createEnemyDisplay(int id);
	void duplicateEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int enemy_id);
	void createDialogue(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	void editEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int enemy_id);
	void editEnemyValues(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void removeEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int enemy_id);
	void removeWave(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int wave);

	void saveLevel(Ref* sender);
	void showExistingLevels(Ref* sender);
	void hideExistingLevels(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void showEnemyBox(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int wave);
	void hideEnemyBox(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void showDialogueBox(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void hideDialogueBox(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void hideDeleteConfirm(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void showDeleteConfirm(Ref* sender);
	void deleteCurrentLevel(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void showSubContent(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int id);
	void showWave(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int id);

	void updateDisplayScrollingMenu();
	void updateDisplayPathMenu();
	void updateDisplayWaveMenu();
	void updateDisplayWaveMenu(unsigned int wave_index);
	void updateDisplayPathButtons();

	void handleBackground(Ref* sender, cocos2d::ui::Widget::TouchEventType type, 
		std::string image_name, cocos2d::ui::CheckBox* checkbox);
	void addObject(Ref* sender, cocos2d::ui::Widget::TouchEventType type,
		std::string image_name);
	void scaleObject(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void rotateObject(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void flipObject(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void flipObjectY(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void setMoveByCell(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void loadLevelFromFileName(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void validatePath(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void removePathTile(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void removePath(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int id);
	void inversePath(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void selectPath(Ref* sender, cocos2d::ui::Widget::TouchEventType type, unsigned int id);
	cocos2d::Vec2 getPositionInGrid(cocos2d::Vec2 pos, bool half = true);
	void addPath(cocos2d::Vec2 pos);

	void addEvents();
	void drawResolutions(cocos2d::DrawNode* node, double resolution, cocos2d::Color4F color);
	void reorder();
	void resetLevel();
	//void setEnemyBox()

	void setSelectedLevel(Ref* sender, cocos2d::ui::Widget::TouchEventType type, std::string level);
	void setCurrentEnemy(Ref* sender, cocos2d::ui::Widget::TouchEventType type,
		std::string name, unsigned int level, cocos2d::ui::CheckBox* checkbox);
	Direction getDirectionFromPos(cocos2d::Vec2 pos1, cocos2d::Vec2 pos2);


private:
	std::vector<std::pair<cocos2d::Sprite*,std::string>> backgrounds;
	std::vector<std::pair<cocos2d::Sprite*, std::string>> objects;
	DangoGenerator* generator;
	Dialogue* dialogues;
	std::vector<std::pair<std::vector<cocos2d::Sprite*>, std::string>> paths;
	std::vector<unsigned int> id_paths;
	std::vector<std::pair<Node*,bool>> subContents;
	std::vector<std::pair<Node*, bool>> waves;
	std::vector<std::vector<std::pair<Node*, bool>>> subWavesContent;
	std::map<int,std::pair<int, int>> enemies;
	std::map<int,int> waves_id;

	double size_cell;
	cocos2d::Vec2 grid_offset;
	Node* selected_object;
	std::string path_level;

	
	bool initialized;
	bool loaded;
	bool menuHidden;
	bool move_by_cell;
	double idle_timer;
	int path_selected;
	cocos2d::ui::TextField* textfield_selected;

	int enemy_level;
	int enemy_path;
	std::string enemy_name;
	double enemy_timer;
	cocos2d::ui::CheckBox* enemy_checkbox;
	int current_wave;
	unsigned int enemies_id;
	unsigned int wave_id;
	int c_enemy_id;
};

#endif
