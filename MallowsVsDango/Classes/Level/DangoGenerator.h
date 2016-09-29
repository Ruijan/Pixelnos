#ifndef DANGOGENERATOR_HPP
#define DANGOGENERATOR_HPP

#include "cocos2d.h"
#include "../Config/json.h"

class Level;

class DangoGenerator : public cocos2d::Layer
{
private:
	std::vector<std::vector<std::string>> sequenceDango;
	std::vector<std::vector<double>> sequenceTimer;
	std::vector<std::vector<int>> sequencePath;

	double timer;
	unsigned int step;
	int cWave;
	int total_steps;
	int progress;

public:
	DangoGenerator();
	~DangoGenerator();

	static DangoGenerator* createWithFilename(std::string filename);
	static DangoGenerator* createWithRoot(Json::Value root);
	void update(double dt, Level* level);

	// Setters
	void setTime(double time, unsigned int step, unsigned int wave);
	void setDango(std::string name, unsigned int step, unsigned int wave);
	void setPath(unsigned int path, unsigned int step, unsigned int wave);

	// Getters
	double getTime(unsigned int step, unsigned int wave);
	int getPath(unsigned int step, unsigned int wave);
	std::string getDango(unsigned int step, unsigned int wave);

	//add or duplicate wave or step
	void addWave();
	bool addStep(std::string dango, double time, unsigned int path, unsigned int wave);
	void duplicateStep(unsigned int step, unsigned int wave);
	void duplicateWave(unsigned int wave);

	void removeStep(unsigned int step, unsigned int wave);
	void removeWave(unsigned int wave);
	int getNbWaves();
	int getNbSteps(unsigned int step);
	void nextWave();
	bool isWaveDone();
	bool isDone();
	void reset();
	void empty();
	void saveInRoot(Json::Value& root);
	float getProgress();
};

#endif

