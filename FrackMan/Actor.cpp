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
bool Actor::isProtester() const
{
	//return false for default because most actors aren't protesters; protesters will override this to return true
	return false;
}
bool Actor::isFrackMan() const
{
	//return false for default because most actors aren't frackman; frackman will override this to return true
	return false;
}
bool Actor::canFall() const
{
	//return false for default because most actors aren't boulder; boulder will override this to return true
	return false;
}
bool Actor::isDeadOrInactive() const
{
	return !m_isAliveorActive;
}
void Actor::setDeadOrInactive()
{
	m_isAliveorActive = false;
}
bool Actor::takeStepInDir(Direction dir)
{
	//check if step is valid first
	if (!getCurrentWorld()->canMoveIn(getX(), getY(), dir, isFrackMan())) return false;

	//take step
	switch (dir)
	{
	case up:
			moveTo(getX(), getY() + 1);
			return true;
		break;
	case down:
			moveTo(getX(), getY() - 1);
			return true;
		break;
	case left:
			moveTo(getX() - 1, getY());
			return true;
		break;
	case right:
			moveTo(getX() + 1, getY());
			return true;
		break;
	}
	return false;
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
int AnnoyableActor::getHitPoints() const
{
	return m_hitPoints;
}
void AnnoyableActor::changeHitPoints(int change)
{
	//negative change denotes decrease
	m_hitPoints += change;
}
AnnoyableActor::~AnnoyableActor()
{
}

//Frackman Implementations
FrackMan::FrackMan(StudentWorld* currentWorld)
	:AnnoyableActor(IID_PLAYER,30,60,10,currentWorld,right,1.0,0),
	m_waterAmmoCount(5),m_sonarChargeCount(1),m_goldNuggetCount(0)
{
	setVisible(true);
}
void FrackMan::doSomething()
{
	//do nothing if dead
	if (isDeadOrInactive()) return;
	//remove any dirt that's frackman's image is overlapping
	if (getCurrentWorld()->removeOverlapDirt(getX(), getY())) getCurrentWorld()->playSound(SOUND_DIG);

	//perform action based on user input
	int input;
	if (getCurrentWorld()->getKey(input))
	{
		switch (input)
		{
		case KEY_PRESS_ESCAPE:
			//give up level
			setDeadOrInactive();
			break;
		case KEY_PRESS_SPACE:
			//fire water squirt if possible
			if (m_waterAmmoCount >= 1)
			{
				getCurrentWorld()->playSound(SOUND_PLAYER_SQUIRT);
				switch (getDirection())
				{
				case up:
					if(getCurrentWorld()->canMoveIn(getX(),getY(),up,false))
					getCurrentWorld()->addObject(new Squirt(getX(), getY()+4, getDirection(), getCurrentWorld()));
					break;
				case down:
					if (getCurrentWorld()->canMoveIn(getX(), getY(), down, false))
					getCurrentWorld()->addObject(new Squirt(getX(), getY() - 4, getDirection(), getCurrentWorld()));
					break;
				case left:
					if (getCurrentWorld()->canMoveIn(getX(), getY(), left, false))
					getCurrentWorld()->addObject(new Squirt(getX() - 4, getY(), getDirection(), getCurrentWorld()));
					break;
				case right:
					if (getCurrentWorld()->canMoveIn(getX(), getY(), right, false))
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
			//otherwise attempt to take a normal step in desired direction
			else if (getCurrentWorld()->canMoveIn(getX(), getY(),down,true))
				moveTo(getX(), getY() - 1);
			break;
		case KEY_PRESS_LEFT:
			//change direction if current direction differs from input direction
			if (getDirection() != left)
				setDirection(left);
			//do not move but still play walking animation if attempting to walk out of bounds
			else if (getCurrentWorld()->isOutofBounds(getX()-1, getY())) moveTo(getX(), getY());
			//otherwise attempt to take a normal step in desired direction
			else if (getCurrentWorld()->canMoveIn(getX(), getY(), left, true))
				moveTo(getX() - 1, getY());
			break;
		case KEY_PRESS_RIGHT:
			//change direction if current direction differs from input direction
			if (getDirection() != right)
				setDirection(right);
			//do not move but still play walking animation if attempting to walk out of bounds
			else if (getCurrentWorld()->isOutofBounds(getX()+1, getY())) moveTo(getX(), getY());
			//otherwise attempt to take a normal step in desired direction
			else if (getCurrentWorld()->canMoveIn(getX(), getY(), right, true))
				moveTo(getX() + 1, getY());
			break;
		case KEY_PRESS_UP:
			//change direction if current direction differs from input direction
			if (getDirection() != up)
				setDirection(up);
			//do not move but still play walking animation if attempting to walk out of bounds
			else if (getCurrentWorld()->isOutofBounds(getX(), getY() + 1)) moveTo(getX(), getY());
			//otherwise attempt to take a normal step in desired direction
			else if (getCurrentWorld()->canMoveIn(getX(), getY(), up, true))
				moveTo(getX(), getY() + 1);
			break;
		case 'Z':
		case 'z':
			//activate sonar
			if (m_sonarChargeCount >= 1)
			{
				m_sonarChargeCount--;
				getCurrentWorld()->playSound(SOUND_SONAR);
				getCurrentWorld()->revealAllAround(getX(), getY());
			}
			break;
		case KEY_PRESS_TAB:
			//drop temporary nugget
			if (m_goldNuggetCount >= 1)
			{
				m_goldNuggetCount--;
				getCurrentWorld()->addObject(new GoldNugget(getX(), getY(),true,getCurrentWorld()));
			}
			break;
		}
	}
}
void FrackMan::getAnnoyed(int changeInHitPoints)
{
	//change hit points
	changeHitPoints(changeInHitPoints);
	//check if dead
	if (getHitPoints() <= 0 && !isDeadOrInactive())
	{
		setDeadOrInactive();
		getCurrentWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}
}
bool FrackMan::isFrackMan() const
{
	return true;
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
	m_sonarChargeCount += 2;
}
FrackMan::~FrackMan()
{
}

//Protester Implementations
Protester::Protester(StudentWorld * currentWorld, int imageID, int hitPoints)
	:AnnoyableActor(imageID, 60, 60, hitPoints, currentWorld, left, 1.0,0),
	m_readyToLeaveOilField(false), m_ticksToWaitBetweenMoves(0), m_ticksTillNextShout(0), m_ticksTillNextPerpendicularTurn(200)
{
	setVisible(true);
	m_maxSquaresToMoveInCurrDir = std::rand() % 53 + 8;
	m_squaresToMoveInCurrDir = m_maxSquaresToMoveInCurrDir;
	m_maxTicksToWaitBetweenMoves = std::max(0, static_cast<int>(3 - getCurrentWorld()->getLevel() / 4));
}
void Protester::doSomething()
{
	if (doCommonRoutinePart1())return;
	//this differentiated routine is available for hardcoreprotesters only
	if (useCellphoneSearchIfAvail())return;
	else doCommonRoutinePart2();
}
void Protester::getAnnoyed(int changeInHitPoints)
{
	//those who are already leaving can't be annoyed further
	if (m_readyToLeaveOilField) return;

	//change hit points
	changeHitPoints(changeInHitPoints);
	//check if sufficiently annoyed to leave
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
bool Protester::isProtester() const
{
	return true;
}
bool Protester::isReadyToLeave() const
{
	return m_readyToLeaveOilField;
}
void Protester::setReadyToLeave()
{
	m_readyToLeaveOilField = true;
}
void Protester::increaseWaitingTicks(int amount)
{
	m_ticksToWaitBetweenMoves += amount;
}
bool Protester::doCommonRoutinePart1()
{
	if (isDeadOrInactive())return true;

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
		if (getX() == 60 && getY() == 60) setDeadOrInactive();
		else
		{
			Direction dir = getCurrentWorld()->dirAndDistanceToFrackmanOrExit(getX(), getY(),true);
			setDirection(dir);
			takeStepInDir(dir);
		}
		return true;
	}

	//check if near frackman and facing frackman's direction
	else if (getCurrentWorld()->isNearFrackman(getX(), getY(), 4))
	{
		if (getCurrentWorld()->dirAndDistanceToFrackmanOrExit(getX(), getY(), false) == getDirection())
		{
			if (m_ticksTillNextShout <= 0)
			{
				//annoy FrackMan
				getCurrentWorld()->playSound(SOUND_PROTESTER_YELL);
				getCurrentWorld()->annoyFrackMan(-2);
				m_ticksTillNextShout = 15; //reset attack timer
				m_ticksToWaitBetweenMoves += 30;  //freeze for a few ticks after attack according to sample game
			}
		}
		//this part is clearly demonstrated in sample game, but I believe was neglected in the spec - just stand at a ditch 4 squares deep
		//with only the top open and notice that if a protester walks by facing left or right, he will CHANGE his direction to face down, then attack you.
		//Notice line of sight function does not apply here because in this situation, radial distance to frackman is already less than 4
		else setDirection(getCurrentWorld()->dirAndDistanceToFrackmanOrExit(getX(), getY(), false));
		return true;
	}
	return false;
}
void Protester::doCommonRoutinePart2()
{
	//check if FrackMan within line of sight and there'a  clear path to him
	if (getCurrentWorld()->hasLineOfSightToFrackMan(getX(), getY()) && !getCurrentWorld()->isNearFrackman(getX(), getY(), 4))
	{
		Direction newDir = getCurrentWorld()->dirAndDistanceToFrackmanOrExit(getX(), getY(), false);
		setDirection(newDir);
		takeStepInDir(newDir);
		m_squaresToMoveInCurrDir = 0;
		return;
	}
	//frackman not in sight, so take one arbitrary step
	else
	{
		m_squaresToMoveInCurrDir--;
		if (m_squaresToMoveInCurrDir <= 0)
		{
			//choose a new non-obstructed direction
			Direction randDir;
			for (;;)
			{
				randDir = static_cast<Direction>(std::rand() % 4 + 1);
				if (getCurrentWorld()->canMoveIn(getX(), getY(), randDir, isFrackMan())) break;
			}
			setDirection(randDir);
			m_maxSquaresToMoveInCurrDir = std::rand() % 53 + 8;
			m_squaresToMoveInCurrDir = m_maxSquaresToMoveInCurrDir;
		}
		//take perpendicular turn if appropriate
		else if (m_ticksTillNextPerpendicularTurn <= 0)
		{
			bool madeTurn = false;
			//check if at intersection and make perpendicular turn if appropriate
			if (getDirection() == up || getDirection() == down)
			{
				if (getCurrentWorld()->canMoveIn(getX(), getY(), left, isFrackMan()) && getCurrentWorld()->canMoveIn(getX(), getY(), right, isFrackMan()))
				{
					Direction randDir = static_cast<Direction>(std::rand() % 2 + 1);
					if (randDir == 1) setDirection(left);
					else setDirection(right);
					madeTurn = true;
				}
				else if (getCurrentWorld()->canMoveIn(getX(), getY(), left, isFrackMan()))
				{
					setDirection(left);
					madeTurn = true;
				}
				else if (getCurrentWorld()->canMoveIn(getX(), getY(), right, isFrackMan()))
				{
					setDirection(right);
					madeTurn = true;
				}
			}
			else
			{
				if (getCurrentWorld()->canMoveIn(getX(), getY(), up, isFrackMan()) && getCurrentWorld()->canMoveIn(getX(), getY(), down, isFrackMan()))
				{
					Direction randDir = static_cast<Direction>(std::rand() % 2 + 1);
					if (randDir == 1) setDirection(up);
					else setDirection(down);
					madeTurn = true;
				}
				else if (getCurrentWorld()->canMoveIn(getX(), getY(), up, isFrackMan()))
				{
					setDirection(up);
					madeTurn = true;
				}
				else if (getCurrentWorld()->canMoveIn(getX(), getY(), down, isFrackMan()))
				{
					setDirection(down);
					madeTurn = true;
				}
			}
			if (madeTurn)
			{
				m_squaresToMoveInCurrDir = std::rand() % 53 + 8;
				m_ticksTillNextPerpendicularTurn = 200;
			}
		}
		if (getCurrentWorld()->canMoveIn(getX(),getY(),getDirection(),false))
		{
			takeStepInDir(getDirection());
			return;
		}
		else m_squaresToMoveInCurrDir = 0;
	}

}

Protester::~Protester()
{
}

//RegularProtester Implementations
RegularProtester::RegularProtester(StudentWorld * currentWorld)
	:Protester(currentWorld, IID_PROTESTER, 5)
{
	setVisible(true);
}
void RegularProtester::increasePlayerScore()
{
	getCurrentWorld()->increaseScore(100);
}
void RegularProtester::getBribed()
{
	getCurrentWorld()->increaseScore(25);
	setReadyToLeave();
}
bool RegularProtester::useCellphoneSearchIfAvail()
{
	//RegularProtesters don't have Advanced search
	return false;
}
RegularProtester::~RegularProtester()
{
}

//HardcoreProtester Implementations
HardcoreProtester::HardcoreProtester(StudentWorld * currentWorld)
	:Protester(currentWorld, IID_HARD_CORE_PROTESTER, 20)
{
	setVisible(true);
}
void HardcoreProtester::increasePlayerScore()
{
	getCurrentWorld()->increaseScore(250);
}
void HardcoreProtester::getBribed()
{
	//hardcore protesters don't leave when bribed; they simply freeze in place for a short number of ticks
	getCurrentWorld()->increaseScore(50);
	increaseWaitingTicks(std::max(50, static_cast<int>(100 - getCurrentWorld()->getLevel() * 10)));
}
bool HardcoreProtester::useCellphoneSearchIfAvail()
{
	//check if FrackMan is within M steps away
	int M = 16 + getCurrentWorld()->getLevel() * 2;
	int distance;
	Direction toFrackMan = getCurrentWorld()->dirAndDistanceToFrackmanOrExit(getX(), getY(), false, &distance);
	if (distance <= M)
	{
		setDirection(toFrackMan);
		takeStepInDir(getDirection());
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
	setVisible(true);
}
void Dirt::doSomething()
{
	//dirt doesn't do anything
}
Dirt::~Dirt()
{
}

//BarrelOfOil Implementations
BarrelOfOil::BarrelOfOil(int startX, int startY, StudentWorld * currentWorld)
	:Actor(IID_BARREL, startX, startY, currentWorld, right, 1.0, 2),m_isHidden(true)
{
	setVisible(false);
}
void BarrelOfOil::doSomething()
{
	if (isDeadOrInactive()) return;
	//check if it should reveal itself
	else if (m_isHidden && getCurrentWorld()->isNearFrackman(getX(), getY(), 4))
	{
		setVisible(true);
		m_isHidden = false;
		return;
	}
	//check if it should be picked up
	else if (getCurrentWorld()->isNearFrackman(getX(), getY(), 3))
	{
		setDeadOrInactive();
		getCurrentWorld()->playSound(SOUND_FOUND_OIL);
		getCurrentWorld()->increaseScore(1000);
		getCurrentWorld()->decOilCount();
	}
}
BarrelOfOil::~BarrelOfOil()
{
}

//ExpirableObject Implementations
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
	if (isDeadOrInactive()) return;
	//start waiting to fall if all dirt under it was cleared
	if (m_currentState == BOULDER_STABLE)
	{
		if (!getCurrentWorld()->hasOverlapDirt(getX(),getY()-1))
			m_currentState = BOULDER_WAITING;
	}
	//start falling and play sound if waiting period has elapsed
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
		//remove boulder upon encountering dirt, another boulder, or boundary
		if (getCurrentWorld()->isOutofBounds(getX(), getY()-1)
			|| getCurrentWorld()->hasOverlapDirt(getX(), getY()-1)
			|| getCurrentWorld()->hasBoulder(getX(), getY()-1, this))
			setDeadOrInactive();
		getCurrentWorld()->causeAnnoyanceWithinRadius(getX(), getY());
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
}
void Squirt::doSomething()
{
	if (isDeadOrInactive()) return;
	//attempt to annoy protesters at current location
	if (getCurrentWorld()->annoyProtestersAt(getX(), getY()))
		setDeadOrInactive();
	//check if expired
	else if (getTicksTillExpire() <= 0) setDeadOrInactive();
	//continue moving forward and decrement ticks till expire
	else
	{
		if (getCurrentWorld()->canMoveIn(getX(),getY(),getDirection(),false))
		{
			takeStepInDir(getDirection());
			decTicksTillExpire();
		}
		else setDeadOrInactive();
	}
}
Squirt::~Squirt()
{
}

//GoldNugget Implementations
GoldNugget::GoldNugget(int startX, int startY, bool isTemporary, StudentWorld * currentWorld)
	:ExpirableObject(IID_GOLD, startX, startY, right, 2, currentWorld, 100), m_isTemporary(isTemporary)
{
	//temporary goldnugget is visible and can only be picked up by protester
	if (isTemporary)
	{
		setVisible(true);
	}
	//permanent goldnugget is not visible initially until approached, and can only be picked up by FrackMan
	else
	{
		setVisible(false);
	}
}

void GoldNugget::doSomething()
{
	//check if this is a nugget for frackman, and close enough to frackman to be picked up
	if (!m_isTemporary && getCurrentWorld()->isNearFrackman(getX(), getY(), 3))
	{
		setDeadOrInactive();
		getCurrentWorld()->playSound(SOUND_GOT_GOODIE);
		getCurrentWorld()->increaseScore(10);
		getCurrentWorld()->pickupFrackManGold();
	}
	//check if this is a nugget for frackman, and is close enough to FrackMan to reveal itself
	if (!m_isTemporary && getCurrentWorld()->isNearFrackman(getX(), getY(), 4))
	{
		setVisible(true);
		return;
	}
	else if (m_isTemporary)
	{
		//if this is a nugget for protesters, attempt to bribe protesters around it
		if (getCurrentWorld()->bribeProtesterAt(getX(), getY()))
		{
			getCurrentWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
			setDeadOrInactive();
		}
		//if bribing unsuccessful, decrease ticks until it gets removed
		else
		{
			if (getTicksTillExpire() <= 0) setDeadOrInactive();
			else decTicksTillExpire();
		}
	}
}

GoldNugget::~GoldNugget()
{
}

//ExpirableSingleStateGoodie Implementations
ExpirableSingleStateGoodie::ExpirableSingleStateGoodie(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld * currentWorld, int ticksUntilExpire)
	:ExpirableObject(imageID, startX, startY, dir, depth, currentWorld, ticksUntilExpire)
{
}

void ExpirableSingleStateGoodie::doSomething()
{
	if (isDeadOrInactive()) return;

	//check if pickupable by frackman at current location
	if (getCurrentWorld()->isNearFrackman(getX(), getY(), 3))
	{
		setDeadOrInactive();
		getCurrentWorld()->playSound(SOUND_GOT_GOODIE);
		//call unique auxiliary pickup function re-defined in subclasses, which tell FrackMan the specific score earned for picking up the type of goodie
		//and specific update to his inventory
		notifyWorldPickedUp();
	}

	//manage expiring timer
	if (getTicksTillExpire() <= 0) setDeadOrInactive();
	else decTicksTillExpire();
}

ExpirableSingleStateGoodie::~ExpirableSingleStateGoodie()
{
}

//WaterPool Implementations
WaterPool::WaterPool(int startX, int startY, StudentWorld * currentWorld)
	:ExpirableSingleStateGoodie(IID_WATER_POOL,startX,startY,right,2,currentWorld, std::max(100, static_cast<int>(300 - 10 * currentWorld->getLevel())))
{
	setVisible(true);
}
void WaterPool::notifyWorldPickedUp()
{
	//define unique change to FrackMan inventory and score update for picking up this goodie
	getCurrentWorld()->pickupFrackManWater();
	getCurrentWorld()->increaseScore(100);
}
WaterPool::~WaterPool()
{
}

//SonarKit Implementations
SonarKit::SonarKit(int startX, int startY, StudentWorld* currentWorld)
	:ExpirableSingleStateGoodie(IID_SONAR,startX,startY,right,2,currentWorld, std::max(100, static_cast<int>(300 - 10 * currentWorld->getLevel())))
{
	setVisible(true);
}
void SonarKit::notifyWorldPickedUp()
{
	//define unique change to FrackMan inventory and score update for picking up this goodie
	getCurrentWorld()->pickupFrackManSonar();
	getCurrentWorld()->increaseScore(75);
}
SonarKit::~SonarKit()
{
}


