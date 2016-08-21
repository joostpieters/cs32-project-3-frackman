#include "Actor.h"
#include "StudentWorld.h"
#include <cstdlib>
#include <algorithm>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//Actor Implementations
Actor::Actor(int imageID, int startX, int startY, StudentWorld* currentWorld, Direction dir, double size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_isAliveorActive(true),m_currentWorld(currentWorld)
{
	setVisible(true);
}
bool Actor::canBeAnnoyed() const
{
	return false;
}
bool Actor::canFall() const
{
	return false;
}
bool Actor::isDead() const
{
	return !m_isAliveorActive;
}
void Actor::setDead()
{
	m_isAliveorActive = false;
}
bool Actor::canMoveIn(Direction dir) const
{
	switch (dir)
	{
	case up:
		if (!getCurrentWorld()->isOutofBounds(getX(), getY() + 1)
			&& !getCurrentWorld()->hasOverlapDirt(getX(), getY() + 1)
			&& !getCurrentWorld()->hasBoulder(getX(), getY() + 1, false))
			return true;
		else return false;
		break;
	case down:
		if (!getCurrentWorld()->isOutofBounds(getX(), getY() - 1)
			&& !getCurrentWorld()->hasOverlapDirt(getX(), getY() - 1)
			&& !getCurrentWorld()->hasBoulder(getX(), getY() - 1, false))
			return true;
		else return false;
		break;
	case left:
		if (!getCurrentWorld()->isOutofBounds(getX() - 1, getY())
			&& !getCurrentWorld()->hasOverlapDirt(getX() - 1, getY())
			&& !getCurrentWorld()->hasBoulder(getX() - 1, getY(), false))
			return true;
		else return false;
		break;
	case right:
		if (!getCurrentWorld()->isOutofBounds(getX() + 1, getY())
			&& !getCurrentWorld()->hasOverlapDirt(getX() + 1, getY())
			&& !getCurrentWorld()->hasBoulder(getX() + 1, getY(), false))
			return true;
		else return false;
		break;
	case none:
		if (!getCurrentWorld()->isOutofBounds(getX(), getY())
			&& !getCurrentWorld()->hasOverlapDirt(getX(), getY())
			&& !getCurrentWorld()->hasBoulder(getX(), getY(), false))
			return true;
		else return false;
		break;
	}
	return false;
}
bool Actor::takeOneStepIn(Direction dir)
{
	switch (dir)
	{
	case up:
		if (!getCurrentWorld()->isOutofBounds(getX(), getY() + 1)
			&& !getCurrentWorld()->hasOverlapDirt(getX(), getY() + 1)
			&& !getCurrentWorld()->hasBoulder(getX(), getY() + 1, false))
		{
			moveTo(getX(), getY() + 1);
		}
		else return false;
		break;
	case down:
		if (!getCurrentWorld()->isOutofBounds(getX(), getY() - 1)
			&& !getCurrentWorld()->hasOverlapDirt(getX(), getY() - 1)
			&& !getCurrentWorld()->hasBoulder(getX(), getY() - 1, false))
		{
			moveTo(getX(), getY() - 1);
		}
		else return false;
		break;
	case left:
		if (!getCurrentWorld()->isOutofBounds(getX() - 1, getY())
			&& !getCurrentWorld()->hasOverlapDirt(getX() - 1, getY())
			&& !getCurrentWorld()->hasBoulder(getX() - 1, getY(), false))
		{
			moveTo(getX() - 1, getY());
		}
		else return false;
		break;
	case right:
		if (!getCurrentWorld()->isOutofBounds(getX() + 1, getY())
			&& !getCurrentWorld()->hasOverlapDirt(getX() + 1, getY())
			&& !getCurrentWorld()->hasBoulder(getX() + 1, getY(), false))
		{
			moveTo(getX() + 1, getY());
		}
		else return false;
		break;
	case none:
		return true;
		break;
	default:
		return false;
		break;
	}
	return true;
}
StudentWorld * Actor::getCurrentWorld() const
{
	return m_currentWorld;
}
Actor::~Actor()
{
}

//Annoyable Actor Implementations
AnnoyableActor::AnnoyableActor(int imageID, int startX, int startY, int initHitPoints, StudentWorld* currentWorld, Direction dir, double size, unsigned int depth)
	:Actor(imageID, startX, startY, currentWorld, dir, size, depth),m_hitPoints(initHitPoints)
{
	setVisible(true);
}
bool AnnoyableActor::canBeAnnoyed() const
{
	return true;
}
int AnnoyableActor::getHitPoints() const
{
	return m_hitPoints;
}
void AnnoyableActor::changeHitPoints(int change)
{
	m_hitPoints += change;
}
AnnoyableActor::~AnnoyableActor()
{
}

//Frackman Implementations
FrackMan::FrackMan(StudentWorld* currentWorld)
	:AnnoyableActor(IID_PLAYER,30,60,10,currentWorld),
	m_waterAmmoCount(5),m_sonarChargeCount(1),m_goldNuggetCount(0)
{
	setVisible(true);
}
void FrackMan::doSomething()
{
	//do nothing if dead
	if (isDead()) return;
	//remove any dirt that's frackman's image is overlapping
	if (getCurrentWorld()->removeOverlapDirt(getX(), getY())) getCurrentWorld()->playSound(SOUND_DIG);

	//perform action based on user input
	int input;
	if (getCurrentWorld()->getKey(input))
	{
		switch (input)
		{
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_SPACE:
			if (m_waterAmmoCount >= 1)
			{
				getCurrentWorld()->playSound(SOUND_PLAYER_SQUIRT);
				switch (getDirection())
				{
				case up:
					getCurrentWorld()->addObject(new Squirt(getX(), getY()+4, getDirection(), getCurrentWorld()));
					break;
				case down:
					getCurrentWorld()->addObject(new Squirt(getX(), getY() - 4, getDirection(), getCurrentWorld()));
					break;
				case left:
					getCurrentWorld()->addObject(new Squirt(getX() - 4, getY(), getDirection(), getCurrentWorld()));
					break;
				case right:
					getCurrentWorld()->addObject(new Squirt(getX() + 4, getY(), getDirection(), getCurrentWorld()));
					break;
				}
				m_waterAmmoCount--;
			}
			break;
		case KEY_PRESS_DOWN:
			//change direction if current direction differs from input direction 
			if (getDirection() != down)
				setDirection(down);
			//do not move but still play walking animation if attempting to walk out of bounds
			else if (getCurrentWorld()->isOutofBounds(getX(), getY() - 1)) moveTo(getX(), getY());
			//otherwise take a normal step in desired direction
			else if (!getCurrentWorld()->hasBoulder(getX(), getY()-1))
				moveTo(getX(), getY() - 1);
			break;
		case KEY_PRESS_LEFT:
			//change direction if current direction differs from input direction
			if (getDirection() != left)
				setDirection(left);
			//do not move but still play walking animation if attempting to walk out of bounds
			else if (getCurrentWorld()->isOutofBounds(getX()-1, getY())) moveTo(getX(), getY());
			//otherwise take a normal step in desired direction
			else if (!getCurrentWorld()->hasBoulder(getX()-1, getY()))
				moveTo(getX() - 1, getY());
			break;
		case KEY_PRESS_RIGHT:
			//change direction if current direction differs from input direction
			if (getDirection() != right)
				setDirection(right);
			//do not move but still play walking animation if attempting to walk out of bounds
			else if (getCurrentWorld()->isOutofBounds(getX()+1, getY())) moveTo(getX(), getY());
			//otherwise take a normal step in desired direction
			else if (!getCurrentWorld()->hasBoulder(getX()+1,getY()))
				moveTo(getX() + 1, getY());
			break;
		case KEY_PRESS_UP:
			//change direction if current direction differs from input direction
			if (getDirection() != up)
				setDirection(up);
			//do not move but still play walking animation if attempting to walk out of bounds
			else if (getCurrentWorld()->isOutofBounds(getX(), getY() + 1)) moveTo(getX(), getY());
			//otherwise take a normal step in desired direction
			else if (!getCurrentWorld()->hasBoulder(getX(), getY()+1))
				moveTo(getX(), getY() + 1);
			break;
		case 'Z':
		case 'z':
			if (m_sonarChargeCount >= 1)
			{
				m_sonarChargeCount--;
				getCurrentWorld()->revealAllAround(getX(), getY());
			}
			break;
		case KEY_PRESS_TAB:
			if (m_goldNuggetCount >= 1)
			{
				m_goldNuggetCount--;
				getCurrentWorld()->addObject(new GoldNugget(getX(), getY(),true,true,getCurrentWorld()));
			}
			break;
		}
	}
}
void FrackMan::getAnnoyed(int changeInHitPoints)
{
	changeHitPoints(changeInHitPoints);
	if (getHitPoints() <= 0 && !isDead())
	{
		setDead();
		getCurrentWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}
}
int FrackMan::getWaterAmmoCount() const
{
	return m_waterAmmoCount;
}
int FrackMan::getGoldNuggetCount() const
{
	return m_goldNuggetCount;
}
int FrackMan::getSonarChargeCount() const
{
	return m_sonarChargeCount;
}
void FrackMan::pickupWaterAmmo()
{
	m_waterAmmoCount += 5;
}
void FrackMan::incrementNugget()
{
	m_goldNuggetCount++;
}
void FrackMan::incrementSonar()
{
	m_sonarChargeCount++;
}
FrackMan::~FrackMan()
{
}

//Protester Implementations
Protester::Protester(StudentWorld * currentWorld, int imageID, int hitPoints)
	:AnnoyableActor(imageID, 60, 60, hitPoints, currentWorld, left, 1.0,0),
	m_readyToLeaveOilField(false), m_ticksToWaitBetweenMoves(0), m_ticksTillNextShout(0), m_ticksTillNextPerpendicularTurn(200)
{
	m_maxSquaresToMoveInCurrDir = std::rand() % 53 + 8;
	m_squaresToMoveInCurrDir = m_maxSquaresToMoveInCurrDir;
	m_maxTicksToWaitBetweenMoves = std::max(0, static_cast<int>(3 - getCurrentWorld()->getLevel() / 4));
}
void Protester::doSomething()
{
	if (doRoutinePart1())return;
	if (useAdvancedSearch())return;
	else doRoutinePart2();
}
void Protester::getAnnoyed(int changeInHitPoints)
{
	if (m_readyToLeaveOilField) return;
	changeHitPoints(changeInHitPoints);
	if (getHitPoints() <= 0)
	{
		m_readyToLeaveOilField = true;
		getCurrentWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_ticksToWaitBetweenMoves = 0;
		if (changeInHitPoints == -2) increasePlayerScore();
		else getCurrentWorld()->increaseScore(500);
	}
	else
	{
		getCurrentWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		m_ticksToWaitBetweenMoves = std::min(50, static_cast<int>(100 - getCurrentWorld()->getLevel() * 10));
	}
}
void Protester::increasePlayerScore()
{
	getCurrentWorld()->increaseScore(100);
}
bool Protester::isReadyToLeave() const
{
	return m_readyToLeaveOilField;
}
void Protester::setReadyToLeave()
{
	m_readyToLeaveOilField = true;
}
bool Protester::doRoutinePart1()
{
	if (isDead())return true;

	//check if protester is resting
	if (m_ticksToWaitBetweenMoves > 0)
	{
		m_ticksToWaitBetweenMoves--;
		return true;
	}

	//otherwise protester is active

	//reset rest counter
	m_ticksToWaitBetweenMoves = m_maxTicksToWaitBetweenMoves;
	//decrement attack counter
	m_ticksTillNextShout--;
	//decrement perpendicular turn counter
	m_ticksTillNextPerpendicularTurn--;
	
	//check if leaving oil field
	if (m_readyToLeaveOilField)
	{
		if (getX() == 60 && getY() == 60) setDead();
		else
		{
			Direction dir = getCurrentWorld()->dirAndDistanceToFrackmanOrExit(getX(), getY(),true);
			setDirection(dir);
			takeOneStepIn(dir);
		}
		return true;
	}

	//check if near frackman and facing frackman
	else if (getCurrentWorld()->isNearFrackman(getX(), getY(), 4)
		&& getCurrentWorld()->dirAndDistanceToFrackmanOrExit(getX(), getY(), false) == getDirection())
	{
		if (m_ticksTillNextShout <= 0)
		{
			//annoy FrackMan
			getCurrentWorld()->playSound(SOUND_PROTESTER_YELL);
			getCurrentWorld()->annoyFrackMan(-2);
			m_ticksTillNextShout = 15; //reset attack timer
		}
		return true;
	}
	return false;
}
void Protester::doRoutinePart2()
{
	if (getCurrentWorld()->hasLineOfSightToFrackMan(getX(), getY()) && !getCurrentWorld()->isNearFrackman(getX(), getY(), 4))
	{
		Direction newDir = getCurrentWorld()->dirAndDistanceToFrackmanOrExit(getX(), getY(), false);
		setDirection(newDir);
		takeOneStepIn(newDir);
		m_squaresToMoveInCurrDir = 0;
		return;
	}
	//frackman not in sight, so take one arbitrary step
	else
	{
		m_squaresToMoveInCurrDir--;
		if (m_squaresToMoveInCurrDir <= 0)
		{
			Direction randDir;
			for (;;)
			{
				randDir = static_cast<Direction>(std::rand() % 4 + 1);
				if (canMoveIn(randDir)) break;
			}
			setDirection(randDir);
			m_maxSquaresToMoveInCurrDir = std::rand() % 53 + 8;
			m_squaresToMoveInCurrDir = m_maxSquaresToMoveInCurrDir;
		}
		else if (m_ticksTillNextPerpendicularTurn <= 0)
		{
			//check if at intersection
			if (getDirection() == up || getDirection() == down)
			{
				if (canMoveIn(left) && canMoveIn(right))
				{
					Direction randDir = static_cast<Direction>(std::rand() % 2 + 1);
					if (randDir == 1) setDirection(left);
					else setDirection(right);
				}
				else if (canMoveIn(left)) setDirection(left);
				else if (canMoveIn(right))setDirection(right);
			}
			else
			{
				if (canMoveIn(up) && canMoveIn(down))
				{
					Direction randDir = static_cast<Direction>(std::rand() % 2 + 1);
					if (randDir == 1) setDirection(up);
					else setDirection(down);
				}
				else if (canMoveIn(up)) setDirection(up);
				else if (canMoveIn(down))setDirection(down);
			}
			m_squaresToMoveInCurrDir = std::rand() % 53 + 8;
			m_ticksTillNextPerpendicularTurn = 200;
		}
		if (takeOneStepIn(getDirection())) return;
		else m_squaresToMoveInCurrDir = 0;
	}

}
void Protester::increaseWaitingTicks(int amount)
{
	m_ticksToWaitBetweenMoves += amount;
}

Protester::~Protester()
{
}

//RegularProtester Implementations
RegularProtester::RegularProtester(StudentWorld * currentWorld)
	:Protester(currentWorld, IID_PROTESTER, 5)
{
}
void RegularProtester::increasePlayerScore()
{
	getCurrentWorld()->increaseScore(100);
}
void RegularProtester::getBribed()
{
	getCurrentWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getCurrentWorld()->increaseScore(25);
	setReadyToLeave();
}
bool RegularProtester::useAdvancedSearch()
{
	return false;
}
RegularProtester::~RegularProtester()
{
}

//HardcoreProtester Implementations
HardcoreProtester::HardcoreProtester(StudentWorld * currentWorld)
	:Protester(currentWorld, IID_HARD_CORE_PROTESTER, 20)
{
}
void HardcoreProtester::increasePlayerScore()
{
	getCurrentWorld()->increaseScore(250);
}
void HardcoreProtester::getBribed()
{
	getCurrentWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getCurrentWorld()->increaseScore(50);
	increaseWaitingTicks(std::max(50, static_cast<int>(100 - getCurrentWorld()->getLevel() * 10)));
}
bool HardcoreProtester::useAdvancedSearch()
{
	int M = 16 + getCurrentWorld()->getLevel() * 2;
	int distance;
	Direction toFrackMan = getCurrentWorld()->dirAndDistanceToFrackmanOrExit(getX(), getY(), false, &distance);
	if (distance <= M)
	{
		setDirection(toFrackMan);
		takeOneStepIn(getDirection());
		return true;
	}
	else return false;
}
HardcoreProtester::~HardcoreProtester()
{
}

//Dirt Implementations
Dirt::Dirt(int startX, int startY, StudentWorld* currentWorld)
	:Actor(IID_DIRT,startX,startY,currentWorld,right,0.25,3)
{
}
void Dirt::doSomething()
{
	//dirt doesn't do anything
}
Dirt::~Dirt()
{
}

//Temporary Actor Implementations
ExpirableObject::ExpirableObject(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* currentWorld, int ticksUntilExpire)
	:Actor(imageID, startX, startY, currentWorld, dir, 1.0, depth), m_ticksUntilExpire(ticksUntilExpire)
{
}
void ExpirableObject::decTicksTillExpire()
{
	m_ticksUntilExpire--;
}
int ExpirableObject::getTicksTillExpire() const
{
	return m_ticksUntilExpire;
}
ExpirableObject::~ExpirableObject()
{
}

//Boulder Implementations
Boulder::Boulder(int x, int y, StudentWorld* currentWorld)
	:ExpirableObject(IID_BOULDER,x,y,right,1,currentWorld,30),m_currentState(BOULDER_STABLE)
{
	setVisible(true);
}
void Boulder::doSomething()
{
	if (isDead()) return;
	if (m_currentState == BOULDER_STABLE)
	{
		if (!getCurrentWorld()->hasOverlapDirt(getX(),getY()-1))
			m_currentState = BOULDER_WAITING;
	}
	else if(m_currentState==BOULDER_WAITING)
	{
		if (getTicksTillExpire() <=0)
		{
			m_currentState = BOULDER_FALLING;
			getCurrentWorld()->playSound(SOUND_FALLING_ROCK);
		}
		else decTicksTillExpire();
	}
	//otherwise boulder is falling
	else
	{
		if (getCurrentWorld()->isOutofBounds(getX(), getY()-1)
			|| getCurrentWorld()->hasOverlapDirt(getX(), getY()-1)
			|| getCurrentWorld()->hasBoulder(getX(), getY()-1, this))
			setDead();
		getCurrentWorld()->causeAnnoyanceWithinRadius(getX(), getY(),-100);
		moveTo(getX(), getY() - 1);
	}
}
bool Boulder::canFall() const
{
	return true;
}
Boulder::~Boulder()
{
}

//Squirt Implementations
Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld* currentWorld)
	:ExpirableObject(IID_WATER_SPURT,startX,startY,dir,1,currentWorld,4)
{
	setVisible(true);
	if (!canMoveIn(none))
		setDead();
}
void Squirt::doSomething()
{
	if (isDead()) return;
	if (getCurrentWorld()->annoyProtestersAt(getX(), getY()))
		setDead();
	else if (getTicksTillExpire() <= 0) setDead();
	else
	{
		if (takeOneStepIn(getDirection()))
		{
			decTicksTillExpire();
		}
		else setDead();
	}
}
Squirt::~Squirt()
{
}

//WaterPool Implementations
WaterPool::WaterPool(int startX, int startY, StudentWorld * currentWorld)
	:ExpirableObject(IID_WATER_POOL,startX,startY,right,2,currentWorld, std::min(100, static_cast<int>(300 - 10 * currentWorld->getLevel())))
{
	setVisible(true);
}
void WaterPool::doSomething()
{
	if (isDead()) return;

	if (getCurrentWorld()->isNearFrackman(getX(), getY(),3))
	{
		setDead();
		getCurrentWorld()->playSound(SOUND_GOT_GOODIE);
		getCurrentWorld()->pickupFrackManWater();
		getCurrentWorld()->increaseScore(100);
	}
	else if (getTicksTillExpire() <= 0) setDead();
	else decTicksTillExpire();
}
WaterPool::~WaterPool()
{
}

//GoldNugget Implementations
GoldNugget::GoldNugget(int startX, int startY, bool isVisible, bool isTemporary, StudentWorld * currentWorld)
	:ExpirableObject(IID_GOLD,startX,startY,right,2,currentWorld,100),m_isTemporary(isTemporary)
{
	setVisible(isVisible);
}
void GoldNugget::doSomething()
{
	if (isDead()) return;
	if (!isVisible() && getCurrentWorld()->isNearFrackman(getX(), getY(), 4))
	{
		setVisible(true);
		return;
	}
	else if (!m_isTemporary && getCurrentWorld()->isNearFrackman(getX(), getY(), 3))
	{
		setDead();
		getCurrentWorld()->playSound(SOUND_GOT_GOODIE);
		getCurrentWorld()->increaseScore(10);
		getCurrentWorld()->pickupFrackManGold();
	}
	else if (m_isTemporary)
	{
		if(getCurrentWorld()->bribeProtesterAt(getX(), getY()))
		setDead();
		else
		{
			if (getTicksTillExpire() <= 0) setDead();
			else decTicksTillExpire();
		}
	}
}
GoldNugget::~GoldNugget()
{
}

//SonarKit Implementations
SonarKit::SonarKit(int startX, int startY, StudentWorld* currentWorld)
	:ExpirableObject(IID_SONAR,startX,startY,right,2,currentWorld, std::min(100, static_cast<int>(300 - 10 * currentWorld->getLevel())))
{
	setVisible(true);
}
void SonarKit::doSomething()
{
	if (isDead()) return;
	if (getCurrentWorld()->isNearFrackman(getX(), getY(), 3))
	{
		setDead();
		getCurrentWorld()->playSound(SOUND_GOT_GOODIE);
		getCurrentWorld()->pickupFrackManSonar();
		getCurrentWorld()->increaseScore(75);
	}
	if (getTicksTillExpire() <= 0) setDead();
	else decTicksTillExpire();
}
SonarKit::~SonarKit()
{
}

//BarrelOfOil Implementations
BarrelOfOil::BarrelOfOil(int startX, int startY, StudentWorld * currentWorld)
	:Actor(IID_BARREL,startX,startY,currentWorld,right,1.0,2)
{
	setVisible(false);
}
void BarrelOfOil::doSomething()
{
	if (isDead()) return;
	if (!isVisible() && getCurrentWorld()->isNearFrackman(getX(), getY(), 4))
	{
		setVisible(true);
		return;
	}
	else if (getCurrentWorld()->isNearFrackman(getX(), getY(), 3))
	{
		setDead();
		getCurrentWorld()->playSound(SOUND_FOUND_OIL);
		getCurrentWorld()->increaseScore(1000);
		getCurrentWorld()->decOilCount();
	}
}
BarrelOfOil::~BarrelOfOil()
{
}