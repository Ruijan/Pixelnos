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
	bool addStep(std::string dango, double time, int wave);
	void addWave();
	int getNbWaves();
	void nextWave();
	bool isWaveDone();
	bool isDone();
	void reset();

};

#endif

