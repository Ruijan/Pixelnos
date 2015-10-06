#ifndef DANGOGENERATOR_HPP
#define DANGOGENERATOR_HPP

#include "cocos2d.h"
#include "../Dangos/Dango.h"

class Level;

class DangoGenerator : public cocos2d::Layer
{
private:
	std::vector<std::vector<unsigned int>> sequenceDango;
	std::vector<std::vector<double>> sequenceTimer;
	double timer;
	unsigned int step;
	int cWave;

public:
	DangoGenerator();
	~DangoGenerator();
	static DangoGenerator* createWithFilename(std::string filename);
	static DangoGenerator* createWithDefaultValues();
	void update(double dt, Level* level);
	bool addStep(unsigned int dango, double time, int wave);
	void addWave();
	int getNbWaves();
	void nextWave();
	bool isWaveDone();
	bool isDone();
	void reset();

};

#endif

