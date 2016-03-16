#ifndef DANGOGENERATOR_HPP
#define DANGOGENERATOR_HPP

#include "cocos2d.h"
#include "../Dangos/Dangosimple.h"
#include "../Dangos/Dangobese.h"

class Level;

class DangoGenerator : public cocos2d::Layer
{
private:
	std::vector<std::vector<std::string>> sequenceDango;
	std::vector<std::vector<double>> sequenceTimer;
	double timer;
	unsigned int step;
	int cWave;

public:
	DangoGenerator();
	~DangoGenerator();
	static DangoGenerator* createWithFilename(std::string filename);
	void update(double dt, Level* level);
	bool addStep(std::string dango, double time, unsigned int wave);
	void setTime(double time, unsigned int step, unsigned int wave);
	void setDango(std::string name, int step, unsigned int wave);
	double getTime(unsigned int step, unsigned int wave);
	std::string getDango(unsigned int step, unsigned int wave);
	void addWave();
	void removeStep(unsigned int step, unsigned int wave);
	void removeWave(unsigned int wave);
	int getNbWaves();
	int getNbSteps(unsigned int step);
	void nextWave();
	bool isWaveDone();
	bool isDone();
	void reset();

};

#endif

