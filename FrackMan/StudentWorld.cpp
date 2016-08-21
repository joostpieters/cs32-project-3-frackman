#include "StudentWorld.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <cmath>
#include <algorithm>
#include <random>
#include <queue>
#include<sstream>
#include<iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

//constructor implementation: initializes dirt pointer array and frackman pointer with nullptr
StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir)
{
	//initializing all pointer variables to nullptr so destructor doesn't cause crash
	//when user quits game on welcome screen (before level is initialized)
	m_FrackMan = nullptr;
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++)
			m_dirt[i][j] = nullptr;
}
//level lifecycle function implementation: initializes the current level
int StudentWorld::init()
{
	//fill the world with dirt
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			if (x >= 30 && x <= 33 && y >= 4 || y >= 60) //no dirt is placed in the mineshaft or the top 4 rows of the map
				m_dirt[x][y] = nullptr;
			else m_dirt[x][y] = new Dirt(x, y, this);
		}
	}

	//fill the world with boulders
	for (int i = 0; i < min(static_cast<int>(getLevel() / 2 + 2), 6); i++)
	{
		int randX, randY;
		generateRandValidCoords(randX, randY, true, true, false, true);
		m_objects.push_back(new Boulder(randX, randY, this));
		removeOverlapDirt(randX, randY);
	}

	//fill the world with gold nuggets
	for (int i = 0; i < max(static_cast<int>(5 - getLevel() / 2), 2); i++)
	{
		int randX, randY;
		generateRandValidCoords(randX, randY, true, true);
		m_objects.push_back(new GoldNugget(randX, randY, false, this));
	}

	//initialize barrel count for this level 
	m_barrelCount = min(static_cast<int>(2 + getLevel()), 20);
	//fill the world with barrels
	for (int i = 0; i < m_barrelCount; i++)
	{
		int randX, randY;
		generateRandValidCoords(randX, randY, true, true);
		m_objects.push_back(new BarrelOfOil(randX, randY, this));
	}

	//initialize the player
	m_FrackMan = new FrackMan(this);

	//initialize tick counter for interval between protester spawn and max ticks till next protester may spawn
	m_ticksTillNextProtester = 0;
	m_maxTicksTillNextProtester = max(25, static_cast<int>(200 - getLevel()));
	//initialize current number of protesters in world and target number to exist
	m_numProtesters = 0;
	m_targetNumProtesters = min(15, static_cast<int>(2 + getLevel()*1.5));
	//initialize probability variables
	m_probabilityGoodie = getLevel() * 25 + 300;
	m_percentProbabilityHardCoreProtester = min(90, static_cast<int>(getLevel() * 10 + 30));

	//contiue with game
	return GWSTATUS_CONTINUE_GAME;
}
//level lifecycle function implementation: process one tick in current level
int StudentWorld::move()
{
	//update game stats text at top of screen
	setGameStatText(formatGameStatText());
	//give each actor chance to do something
	for (vector<Actor*>::iterator iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		if (!(*iter)->isDeadOrInactive())
			(*iter)->doSomething();
		//check if action caused FrackMan to die
		if (m_FrackMan->isDeadOrInactive())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		//check if FrackMan has won the level
		if (m_barrelCount <= 0)
		{
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}
	}
	//give FrackMan chance to do something
	m_FrackMan->doSomething();

	//check if FrackMan either finished level or died
	if (m_FrackMan->isDeadOrInactive())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	if (m_barrelCount <= 0)
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

	//remove dead/inactive actors
	for (vector<Actor*>::iterator iter = m_objects.begin(); iter != m_objects.end();)
	{
		if ((*iter)->isDeadOrInactive())
		{
			if ((*iter)->isProtester()) m_numProtesters--;
			delete *iter;
			iter = m_objects.erase(iter);
		}
		else iter++;
	}

	//check if new protester needs to be added
	if (m_numProtesters < m_targetNumProtesters)
	{
		//check if it's been enough number of ticks since last one was added
		if (m_ticksTillNextProtester <= 0)
		{
			//probabilistically determine which protester to add
			int randNum = rand() % 100 + 1; // generate a number such that 1<=randNum<=100
			if (randNum <=m_percentProbabilityHardCoreProtester) //there is m_percentProbabilityHardCoreProtester% chance of HardCore Protester spawning
			{
				//spawn hardcore protester
				m_objects.push_back(new HardcoreProtester(this));
			}
			else //otherwise probability of hardcore protester not satisfied
			{
				//spawn regular protester
				m_objects.push_back(new RegularProtester(this));
			}
			m_ticksTillNextProtester = m_maxTicksTillNextProtester;
			m_numProtesters++; //update current number of protesters in game
		}
		else m_ticksTillNextProtester--;
	}

	//potentially add new water pool or sonar kit
	int randNum = rand() % m_probabilityGoodie + 1; //generates random integer 1<=randNum<=m_probabilityGoodie
	if (randNum == 1) //the chance of random number being 1 is 1/m_probabilityGoodie, so spawn either a water pool or sonar kit
	{
		//decide whether to spawn water pool or sonar
		int randNum2 = rand() % 5 + 1; //generates random integer 1<=randNum2<=5
		if (randNum2 == 5) //there is 1 in 5 chance of integer being 5, so spawn a sonar kit
		{
			m_objects.push_back(new SonarKit(0, 60, this));
		}
		else //only remaining possibilities for randNum2 is 1,2,3,4, which equates to the chance of 4/5, so spawn water pool
		{
			int randX, randY;
			generateRandValidCoords(randX, randY, false, false, true);
			m_objects.push_back(new WaterPool(randX, randY, this));
		}
	}

	//continue onto next tick
	return GWSTATUS_CONTINUE_GAME;
}
//level lifecycle function implementation: delete dynamically allocated objects in current level
void StudentWorld::cleanUp()
{
	//delete FrackMan
	delete m_FrackMan;
	m_FrackMan = nullptr;
	//delete all dynamically allocated dirt objects
	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 64; y++)
		{
			delete m_dirt[x][y];
			m_dirt[x][y] = nullptr;
		}
	//delete all dynamically allocated game objects (e.g. boulders,sonar kits etc.)
	for (vector<Actor*>::iterator iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		delete *iter;
	}
	m_objects.clear();
}
//virtual destructor implementation: delete any remaining dynamically allocated objects
StudentWorld::~StudentWorld()
{
	//delete frackman
	delete m_FrackMan;
	m_FrackMan = nullptr;
	//delete all dynamically allocated dirt objects
	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 64; y++)
		{
			delete m_dirt[x][y];
			m_dirt[x][y] = nullptr;
		}
	//delete all other game objects (e.g. boulders, sonar kits etc.)
	for (vector<Actor*>::iterator iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		delete *iter;
	}
	m_objects.clear();
}

//FrackMan related method implementations
void StudentWorld::pickupFrackManWater()
{
	m_FrackMan->pickupWaterAmmo();
}
void StudentWorld::pickupFrackManGold()
{
	m_FrackMan->incrementNugget();
}
void StudentWorld::pickupFrackManSonar()
{
	m_FrackMan->incrementSonar();
}
bool StudentWorld::isNearFrackman(int currX, int currY, int radius) const
{
	return calcRadialDistance(currX, currY, m_FrackMan->getX(), m_FrackMan->getY()) <= radius;
}
void StudentWorld::annoyFrackMan(int changeInHitPoints)
{
	m_FrackMan->getAnnoyed(changeInHitPoints);
}

//Protester related method implementations
GraphObject::Direction StudentWorld::dirAndDistanceToFrackmanOrExit(int currX, int currY, bool trueForExitFalseForFrackMan, int* distance)
{
	queue<AdvancedCoord> advancedCoordQueue;

	bool discovered[64][64] = { false };
	discovered[currX][currY] = true;
	if (distance != nullptr) *distance = 0;
	//if target destination is exit, check start coords against exit coords
	if (trueForExitFalseForFrackMan && currX == 60 && currY == 60) return GraphObject::none;
	//otherwise destination is frackman, so check current coords against frackman's coords 
	else if (!trueForExitFalseForFrackMan && currX == m_FrackMan->getX() && currY == m_FrackMan->getY()) return GraphObject::none;

	//push AdvancedCoordinates different directions of exploration
	if (!isOutofBounds(currX + 1, currY) && !hasBoulder(currX + 1, currY)&&!hasOverlapDirt(currX+1,currY))
	{
		discovered[currX + 1][currY] = true;
		advancedCoordQueue.push(AdvancedCoord(currX + 1, currY, GraphObject::right, 1));
	}
	if (!isOutofBounds(currX, currY - 1) && !hasBoulder(currX, currY - 1) && !hasOverlapDirt(currX, currY-1))
	{
		discovered[currX][currY - 1] = true;
		advancedCoordQueue.push(AdvancedCoord(currX, currY - 1, GraphObject::down, 1));
	}
	if (!isOutofBounds(currX - 1, currY) && !hasBoulder(currX - 1, currY) && !hasOverlapDirt(currX -1, currY))
	{
		discovered[currX - 1][currY] = true;
		advancedCoordQueue.push(AdvancedCoord(currX - 1, currY, GraphObject::left, 1));
	}
	if (!isOutofBounds(currX, currY + 1) && !hasBoulder(currX, currY + 1) && !hasOverlapDirt(currX, currY + 1))
	{
		discovered[currX][currY + 1] = true;
		advancedCoordQueue.push(AdvancedCoord(currX, currY + 1, GraphObject::up, 1));
	}

	//travel further along each of 4 directions until hitting target or hitting dead end everywhere
	while (!advancedCoordQueue.empty())
	{
		AdvancedCoord current = advancedCoordQueue.front();
		advancedCoordQueue.pop();
		if (distance != nullptr && current.getDistanceTraveled() > *distance) *distance = current.getDistanceTraveled();

		//if target destination is FrackMan, check if at frackman's location 
		if (!trueForExitFalseForFrackMan && current.getX()==m_FrackMan->getX() && current.getY()==m_FrackMan->getY()) return current.getDir();
		//(please first read note below) if not frackman, then any dirt in current location will definitely not be cleared by FrackMan in next tick, so it will be an obstacle
		//if current coord overlaps dirt, discontinue path in current direction
		if (hasOverlapDirt(current.getX(), current.getY())) continue;

		//if target destination is exit, check if at exit's location
		if (trueForExitFalseForFrackMan && current.getX() == 60 && current.getY() == 60) return current.getDir();

		//continue exploring along current paths by one step in each direction unless blocked by boundary, boulder or dirt 

		//NOTE: dirt overlap check delayed to after coords have been popped off and checked against FrackMan's location; this is
		//because the spec's algorithm (and the sample game's behavior) for FrackMan's dosomething has him clear dirt on the tick
		//after he moves, so any dirt that overlaps frackman should not be treated as an obstacle to FrackMan (since the spec has him clear it in virtually no time); 
		//without this tweak to the algorithm, it may never reach a digging frackman. (dirt on intermediate paths still count as obstacle and will discontinue path, as shown above)
		if (!discovered[current.getX() + 1][current.getY()]
			&& !isOutofBounds(current.getX() + 1, current.getY()) && !hasBoulder(current.getX() + 1, current.getY()) )
		{
			advancedCoordQueue.push(AdvancedCoord(current.getX() + 1, current.getY(), current.getDir(), current.getDistanceTraveled() + 1));
			discovered[current.getX() + 1][current.getY()] = true;
		}
		if (!discovered[current.getX()][current.getY() - 1]
			&& !isOutofBounds(current.getX(), current.getY() - 1) && !hasBoulder(current.getX(), current.getY() - 1) )
		{
			advancedCoordQueue.push(AdvancedCoord(current.getX(), current.getY() - 1, current.getDir(), current.getDistanceTraveled() + 1));
			discovered[current.getX()][current.getY() - 1] = true;
		}
		if (!discovered[current.getX() - 1][current.getY()]
			&& !isOutofBounds(current.getX() - 1, current.getY()) && !hasBoulder(current.getX() - 1, current.getY()) )
		{
			advancedCoordQueue.push(AdvancedCoord(current.getX() - 1, current.getY(), current.getDir(), current.getDistanceTraveled() + 1));
			discovered[current.getX() - 1][current.getY()] = true;
		}
		if (!discovered[current.getX()][current.getY() + 1]
			&& !isOutofBounds(current.getX(), current.getY() + 1) && !hasBoulder(current.getX(), current.getY() + 1))
		{
			advancedCoordQueue.push(AdvancedCoord(current.getX(), current.getY() + 1, current.getDir(), current.getDistanceTraveled() + 1));
			discovered[current.getX()][current.getY() + 1] = true;
		}
	}
	//could not find successful path to destination (should never apply in this game since there are no closed areas)
	if (distance != nullptr)*distance = -1;
	return GraphObject::none;
}
bool StudentWorld::hasLineOfSightToFrackMan(int currX, int currY)
{
	queue<AdvancedCoord> advancedCoordQueue;
	if (currX==m_FrackMan->getX()&&currY==m_FrackMan->getY()) return true;

	//push initial 4 directions onto queue
	advancedCoordQueue.push(AdvancedCoord(currX, currY, GraphObject::right));
	advancedCoordQueue.push(AdvancedCoord(currX, currY, GraphObject::down));
	advancedCoordQueue.push(AdvancedCoord(currX, currY, GraphObject::left));
	advancedCoordQueue.push(AdvancedCoord(currX, currY, GraphObject::up));
	
	//travel further in straight lines along each of 4 directions until hitting target or stopped by obstacle
	while (!advancedCoordQueue.empty())
	{
		AdvancedCoord current = advancedCoordQueue.front();
		advancedCoordQueue.pop();

		//check if current coord is at frackman's location
		if (current.getX()==m_FrackMan->getX() && current.getY()==m_FrackMan->getY()) return true;
		
		if (hasOverlapDirt(current.getX(), current.getY())) continue;

		//NOTE: dirt overlap check delayed to after coords have been popped off and checked against FrackMan's location; this is
		//because the spec's algorithm (and the sample game's behavior) for FrackMan's dosomething has him clear dirt on the tick
		//after he moves, so any dirt that overlaps frackman should not be treated as an obstacle to FrackMan (since the spec has him clear it in virtually no time); 
		//without this tweak to the algorithm, it may never reach a digging frackman. (dirt on intermediate paths still count as obstacle and will discontinue path, as shown above)
		switch (current.getDir())
		{
		case GraphObject::right:
			if (!isOutofBounds(current.getX() + 1, current.getY()) && !hasBoulder(current.getX() + 1, current.getY()))
				advancedCoordQueue.push(AdvancedCoord(current.getX() + 1, current.getY(), GraphObject::right));
			break;
		case GraphObject::down:
			if (!isOutofBounds(current.getX(), current.getY() - 1) && !hasBoulder(current.getX(), current.getY() - 1))
				advancedCoordQueue.push(AdvancedCoord(current.getX(), current.getY() - 1, GraphObject::down));
			break;
		case GraphObject::left:
			if (!isOutofBounds(current.getX() - 1, current.getY()) && !hasBoulder(current.getX() - 1, current.getY()))
				advancedCoordQueue.push(AdvancedCoord(current.getX() - 1, current.getY(), GraphObject::left));
			break;
		case GraphObject::up:
			if (!isOutofBounds(current.getX(), current.getY() + 1) && !hasBoulder(current.getX(), current.getY() + 1))
				advancedCoordQueue.push(AdvancedCoord(current.getX(), current.getY() + 1, GraphObject::up));
			break;
		}
	}
	return false;
}
bool StudentWorld::annoyProtestersAt(int targetX, int targetY)
{
	//attempt to annoy ANY protesters near target coordinates
	bool protestersWereAnnoyed = false;
	for (vector<Actor*>::iterator iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		if ((*iter)->isProtester() && calcRadialDistance(targetX, targetY, (*iter)->getX(), (*iter)->getY()) <= 3)
		{
			dynamic_cast<AnnoyableActor*>(*iter)->getAnnoyed(-2);
			protestersWereAnnoyed = true;
		}
	}
	return protestersWereAnnoyed;
}
bool StudentWorld::bribeProtesterAt(int targetX, int targetY)
{
	//attempt to bribe a single protester near target coordinates
	for (vector<Actor*>::iterator iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		if ((*iter)->isProtester() && !dynamic_cast<Protester*>(*iter)->isReadyToLeave() && calcRadialDistance(targetX, targetY, (*iter)->getX(), (*iter)->getY()) <= 3)
		{
			dynamic_cast<Protester*>(*iter)->getBribed();
			return true;
		}
	}
	return false;
}

//dirt related method implementations
bool StudentWorld::hasOverlapDirt(int x, int y) const
{
	//checks if current 4x4 tile starting from lower left corner at x,y has any dirt overlapping it
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (isDirt(x+i, y+j))
				return true;
	return false;
}
bool StudentWorld::removeOverlapDirt(int x, int y)
{
	//remove all overlapping dirt of 4x4 area, returning true if dirt removed and false otherwise
	bool dirtWasRemoved = false;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (isDirt(x+i, y+j))
			{
				delete m_dirt[x+i][y+j];
				m_dirt[x+i][y+j] = nullptr;
				dirtWasRemoved = true;
			}
	return dirtWasRemoved;
}

//barrelOfOil related method implementations
void StudentWorld::decOilCount()
{
	m_barrelCount--;
}

//boulder related method implementations
bool StudentWorld::hasBoulder(int x, int y, Boulder * current) const
{
	for (vector<Actor*>::const_iterator iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		//ensure a boulder is not checking against itself
		if (*iter == current) continue;
		if ((*iter)->canFall() && calcRadialDistance(x, y, (*iter)->getX(), (*iter)->getY()) <= 3)
			return true;
	}
	return false;
}
bool StudentWorld::causeAnnoyanceWithinRadius(int x, int y)
{
	bool annoyanceCaused = false;
	for (vector<Actor*>::iterator iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		//see if boulder hits FrackMan
		if (calcRadialDistance(x, y, m_FrackMan->getX(), m_FrackMan->getY()) <= 3)
			m_FrackMan->getAnnoyed(-100);
		//see if boulder hits any Protesters
		if ((*iter)->isProtester() && calcRadialDistance(x, y, (*iter)->getX(), (*iter)->getY()) <= 3)
		{
			dynamic_cast<AnnoyableActor*>(*iter)->getAnnoyed(-100);
			annoyanceCaused = true;
		}
	}
	return annoyanceCaused;
}

//sonar related method implementations
void StudentWorld::revealAllAround(int x, int y)
{
	//reveal all hidden actors within radius 12 of FrackMan
	for (vector<Actor*>::iterator iter = m_objects.begin(); iter != m_objects.end(); iter++)
	{
		if (calcRadialDistance(x, y, (*iter)->getX(), (*iter)->getY()) < 12) (*iter)->setVisible(true);
	}
}

//miscellaneous method implementations
bool StudentWorld::isOutofBounds(int x, int y) const
{
	//check if passed in coordinates are outside boundary
	if (x < 0 || x>60 || y < 0 || y > 60) return true;
	return false;
}
bool StudentWorld::canMoveIn(int currX, int currY, GraphObject::Direction dir, bool isFrackMan)
{
	//check if an actor can move in the direction indicated. FrackMan has special ability to move through dirt
	switch (dir)
	{
	case GraphObject::up:
		if (!isOutofBounds(currX, currY + 1)
			&& !hasBoulder(currX, currY + 1, false)
			&& (isFrackMan || !hasOverlapDirt(currX, currY + 1)))
			return true;
		else return false;
		break;
	case GraphObject::down:
		if (!isOutofBounds(currX, currY - 1)
			&& !hasBoulder(currX, currY - 1, false)
			&& (isFrackMan || !hasOverlapDirt(currX, currY - 1)))
			return true;
		else return false;
		break;
	case GraphObject::left:
		if (!isOutofBounds(currX - 1, currY)
			&& !hasBoulder(currX - 1, currY, false)
			&& (isFrackMan || !hasOverlapDirt(currX - 1, currY)))
			return true;
		else return false;
		break;
	case GraphObject::right:
		if (!isOutofBounds(currX + 1, currY)
			&& !hasBoulder(currX + 1, currY, false)
			&& (isFrackMan || !hasOverlapDirt(currX + 1, currY)))
			return true;
		else return false;
		break;
	}
	return false;
}
void StudentWorld::addObject(Actor* toAdd)
{
	m_objects.push_back(toAdd);
}

//PRIVATE (AUXILIARY) METHOD IMPLEMENTATIONS
string StudentWorld::formatGameStatText()
{
	//fill data fields with spaces or padded zeros as necessary
	ostringstream result;
	result.fill('0');
	result << "Scr: " << setw(6) << getScore() << "  ";

	result.fill(' ');
	result << "Lvl: " << setw(2) << getLevel() << "  ";
	result << "Lives: " << setw(1)<<getLives() << "  ";
	result << "Hlth: " << setw(3) << m_FrackMan->getHitPoints() * 100 / 10 << "%  ";
	result << "Wtr: " << setw(2) << m_FrackMan->getWaterAmmoCount() << "  ";
	result << "Gld: " << setw(2) << m_FrackMan->getGoldNuggetCount() << "  ";
	result << "Sonar: " << setw(2) << m_FrackMan->getSonarChargeCount() << "  ";
	result << "Oil Left: " << setw(2) << m_barrelCount;
	return result.str();
}
bool StudentWorld::isDirt(int x, int y) const
{
	return m_dirt[x][y] != nullptr;
}
double StudentWorld::calcRadialDistance(int x1, int y1, int x2, int y2) const
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}
void StudentWorld::generateRandValidCoords(int& x, int& y, bool proximityMatters, bool canBeInDirt, bool canBeInMineShaft, bool isBoulder)
{
	for (;;)
	{
		//generate random coordinates in range
		int randX = rand() % 61;
		int randY;
		if (isBoulder)
			randY = rand() % 37 + 20;
		else
			randY = rand() % 61;
		bool CoordsWithinRange = true;

		//ensure object's not in mineshaft or surface of oil field
		if (!canBeInMineShaft && randX > 26 && randX < 34 && randY >= 4) continue;
		if (randY > 56) continue;
		//check if AdvancedCoords overlap boulder
		if (hasBoulder(randX, randY)) continue;
		//check if AdvancedCoords are too close to other actors
		if (proximityMatters)
		{
			for (vector<Actor*>::iterator iter = m_objects.begin(); iter != m_objects.end(); iter++)
			{
				if (calcRadialDistance(randX, randY, (*iter)->getX(), (*iter)->getY()) <= 6)
				{
					CoordsWithinRange = false;
					break;
				}
			}
		}
		//if object can't be in dirt, check if it's in dirt
		if (!canBeInDirt)
		{
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					if (isDirt(randX + i, randY + j))
						CoordsWithinRange = false;
		}

		if (!CoordsWithinRange) continue;
		else
		{
			x = randX;
			y = randY;
			return;
		}
	}
}

int StudentWorld::AdvancedCoord::getX()
{
	return m_x;
}
int StudentWorld::AdvancedCoord::getY()
{
	return m_y;
}
GraphObject::Direction StudentWorld::AdvancedCoord::getDir()
{
	return m_dir;
}
int StudentWorld::AdvancedCoord::getDistanceTraveled()
{
	return m_distanceTraveled;
}
