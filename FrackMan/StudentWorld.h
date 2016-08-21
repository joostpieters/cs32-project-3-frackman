#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include "Actor.h"
#include "GraphObject.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <queue>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:

	//constructor implementation: initializes dirt pointer array and frackman pointer with nullptr
	StudentWorld(std::string assetDir);
	//level lifecycle function: initializes the current level
	virtual int init();
	//level lifecycle function: process one tick in current level
	virtual	int move();
	//level lifecycle function: delete dynamically allocated objects in current level
	virtual void cleanUp();
	//virtual destructor: delete any remaining dynamically allocated objects
	virtual ~StudentWorld();

	//FrackMan related methods
	void pickupFrackManWater();
	void pickupFrackManGold();
	void pickupFrackManSonar();
	bool isNearFrackman(int currX, int currY, int radius) const;
	void annoyFrackMan(int changeInHitPoints);

	//Protester related methods
	GraphObject::Direction dirAndDistanceToFrackmanOrExit(int currX, int currY, bool trueForExitFalseForFrackMan = false, int* distance = nullptr);
	bool hasLineOfSightToFrackMan(int currX, int currY);
	bool annoyProtestersAt(int targetX, int targetY);
	bool bribeProtesterAt(int targetX, int targetY);

	//dirt related methods
	bool hasOverlapDirt(int x, int y) const;
	bool removeOverlapDirt(int x, int y);

	//barrelOfOil related methods
	void decOilCount();

	//boulder related methods
	bool hasBoulder(int x, int y, Boulder * current = nullptr) const;
	bool causeAnnoyanceWithinRadius(int x, int y);

	//sonar related methods
	void revealAllAround(int x, int y);

	//miscellaneous methods
	bool isOutofBounds(int x, int y) const;
	bool canMoveIn(int currX, int currY, GraphObject::Direction dir, bool isFrackMan);
	void addObject(Actor* toAdd);
private:
	//A data type that stores current coordinate, direction of travel, and (optionally) the distance that has been travelled 
	//in such direction to facilitate queue-based breadth first searching
	struct AdvancedCoord
	{
	public:
		AdvancedCoord(int startX, int startY, GraphObject::Direction initDirection, int initDistance = 0)
			:m_x(startX), m_y(startY), m_dir(initDirection), m_distanceTraveled(initDistance)
		{
		}
		int getX();
		int getY();
		GraphObject::Direction getDir();
		int getDistanceTraveled();
	private:
		int m_x;
		int m_y;
		GraphObject::Direction m_dir;
		int m_distanceTraveled;
	};
	//auxiliary functions
	std::string formatGameStatText();
	bool isDirt(int x, int y) const; //determine whether one single square is occupied dirt
	double calcRadialDistance(int x1, int y1, int x2, int y2) const;
	void generateRandValidCoords(int& x, int& y, bool proximityMatters, bool canBeInDirt, bool canBeInMineShaft = false, bool isBoulder = false);

	//data members
	FrackMan* m_FrackMan;
	std::vector<Actor*> m_objects;
	Dirt* m_dirt[VIEW_WIDTH][VIEW_HEIGHT];

	int m_barrelCount;
	int m_numProtesters;
	int m_targetNumProtesters;
	int m_ticksTillNextProtester;
	int m_maxTicksTillNextProtester;
	int m_probabilityGoodie;
	int m_percentProbabilityHardCoreProtester;
};

#endif // STUDENTWORLD_H_
