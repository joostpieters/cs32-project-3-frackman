#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h" //using Direction enums from GraphObject.h
class StudentWorld;

//boulder state constants
const int BOULDER_STABLE = 0;
const int BOULDER_WAITING = 1;
const int BOULDER_FALLING = 2;

class Actor :public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, StudentWorld* currentWorld, Direction dir = right, double size = 1.0, unsigned int depth = 0);
	virtual void doSomething() = 0;
	virtual bool canBeAnnoyed() const;
	virtual bool canFall() const;
	bool isDead() const;
	void setDead();
	bool canMoveIn(Direction dir) const;
	bool takeOneStepIn(Direction dir);
	StudentWorld* getCurrentWorld() const;
	virtual ~Actor();
private:
	bool m_isAliveorActive;
	StudentWorld* m_currentWorld;
};

class AnnoyableActor :public Actor
{
public:
	AnnoyableActor(int imageID, int startX, int startY, int initHitPoints, StudentWorld* currentWorld, Direction dir = right, double size = 1.0, unsigned int depth = 0);
	virtual void doSomething() = 0;
	virtual void getAnnoyed(int changeInHitPoints) = 0;
	virtual bool canBeAnnoyed() const;
	int getHitPoints() const;
	void changeHitPoints(int change);

	virtual ~AnnoyableActor();
private:
	int m_hitPoints;
};

class FrackMan :public AnnoyableActor
{
public:
	FrackMan(StudentWorld* currentWorld);
	virtual void doSomething();
	virtual void getAnnoyed(int changeInHitPoints);
	int getWaterAmmoCount() const;
	int getGoldNuggetCount() const;
	int getSonarChargeCount() const;
	void pickupWaterAmmo();
	void incrementNugget();
	void incrementSonar();
	virtual ~FrackMan();
private:
	int m_waterAmmoCount;
	int m_sonarChargeCount;
	int m_goldNuggetCount;
};

class Protester :public AnnoyableActor
{
public:
	Protester(StudentWorld* currentWorld, int imageID, int hitPoints);
	virtual void doSomething();
	virtual void getAnnoyed(int changeInHitPoints);
	virtual void increasePlayerScore() = 0;
	virtual void getBribed() = 0;
	bool isReadyToLeave() const;
	void setReadyToLeave();
	bool doRoutinePart1();
	void doRoutinePart2();
	void increaseWaitingTicks(int amount);

	virtual ~Protester();
private:
	virtual bool useAdvancedSearch() = 0;
	bool m_readyToLeaveOilField;
	int m_squaresToMoveInCurrDir;
	int m_maxSquaresToMoveInCurrDir;
	int m_ticksToWaitBetweenMoves;
	int m_maxTicksToWaitBetweenMoves;
	int m_ticksTillNextShout;
	int m_ticksTillNextPerpendicularTurn;
};
class RegularProtester :public Protester
{
public:
	RegularProtester(StudentWorld* currentWorld);
	virtual void increasePlayerScore();
	virtual void getBribed();
	virtual ~RegularProtester();
private:
	virtual bool useAdvancedSearch();
};
class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* currentWorld);
	virtual void increasePlayerScore();
	virtual void getBribed();
	virtual ~HardcoreProtester();
private:
	virtual bool useAdvancedSearch();
};
class Dirt :public Actor
{
public:
	Dirt(int startX, int startY,StudentWorld* currentWorld);
	virtual void doSomething();
	virtual ~Dirt();
};

class BarrelOfOil : public Actor
{
public:
	BarrelOfOil(int startX, int startY, StudentWorld* currentWorld);
	virtual void doSomething();
	virtual ~BarrelOfOil();
};

class ExpirableObject : public Actor
{
public:
	ExpirableObject(int imageID, int startX, int startY, Direction dir, int depth, StudentWorld* currentWorld, int ticksUntilExpire);
	virtual void doSomething() = 0;
	void decTicksTillExpire();
	int getTicksTillExpire() const;
	virtual ~ExpirableObject();

private:
	int m_ticksUntilExpire;
};

class Boulder : public ExpirableObject
{
public:
	Boulder(int x, int y, StudentWorld* currentWorld);
	virtual void doSomething();
	virtual bool canFall() const;
	virtual ~Boulder();

private:
	int m_currentState;
};

class Squirt :public ExpirableObject
{
public:
	Squirt(int startX, int startY, Direction dir, StudentWorld* currentWorld);
	virtual void doSomething();
	virtual ~Squirt();
};

class WaterPool : public ExpirableObject
{
public:
	WaterPool(int startX, int startY, StudentWorld* currentWorld);
	virtual void doSomething();
	virtual ~WaterPool();
};
class GoldNugget : public ExpirableObject
{
public:
	GoldNugget(int startX, int startY, bool isVisible, bool isTemporary, StudentWorld* currentWorld);
	virtual void doSomething();
	virtual ~GoldNugget();
private:
	bool m_isTemporary;
};
class SonarKit : public ExpirableObject
{
public:
	SonarKit(int startX, int startY, StudentWorld* currentWorld);
	virtual void doSomething();
	virtual ~SonarKit();
};

#endif // ACTOR_H_
