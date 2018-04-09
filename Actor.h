#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;
class Projectile;

class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, StudentWorld * world, int startDirection, double size, int depth);

	virtual void doSomething() = 0;
	bool isAlive() const;
	StudentWorld* getWorld() const;

	virtual bool isAlien();
	virtual void moveTo(double x, double y);
	void setDead();

private:
	bool m_isAlive;
	StudentWorld* m_world;
};


///////////////////////////////////////////////////////////////////////////////////


class Star : public Actor
{
public:
	Star(double startX, double startY, StudentWorld * world);
	virtual void doSomething();
};


///////////////////////////////////////////////////////////////////////////////////


class Explosion : public Actor
{
public:
	Explosion(double startX, double startY, StudentWorld * world);
	virtual void doSomething();
private:
	int m_timer;
};


///////////////////////////////////////////////////////////////////////////////////


// Abstract **************are constructors too similar?
class Projectile : public Actor
{
public:
	Projectile(int imageID, double startX, double startY, double damage, double deltaX, bool rotates, StudentWorld * world, int startDirection);
	void doSomething();
	double getDamage() const;

private:
	virtual void checkCollision()=0;
	int m_damage;
	bool m_rotates;
	double m_deltaX;
};

class Cabbage : public Projectile
{
public:
	Cabbage(double startX, double startY, StudentWorld * world);

private:
	virtual void checkCollision();
};

class Turnip : public Projectile
{
public:
	Turnip(double startX, double startY, StudentWorld * world);
private:
	virtual void checkCollision();
};

class FlatulenceTorpedo : public Projectile
{
public:
	FlatulenceTorpedo(double startX, double startY, double deltaX, StudentWorld * world, int startDirection);
};

class PlayerTorpedo : public FlatulenceTorpedo
{
public:
	PlayerTorpedo(double startX, double startY, StudentWorld * world);

private:
	virtual void checkCollision();
};

class AlienTorpedo : public FlatulenceTorpedo
{
public:
	AlienTorpedo(double startX, double startY, StudentWorld * world);

private:
	virtual void checkCollision();
};


///////////////////////////////////////////////////////////////////////////////////


// Abstract (doSomething)
class Ship : public Actor
{
public:
	Ship(int imageID, double startX, double startY, double hp, StudentWorld * world, int startDirection = 0, double size = 1.0, int depth = 0);
	double getHP() const;
	virtual void increaseHP(double amt);
	void sufferDamage(double damage, int cause);

private:
	virtual void playHitSound(int cause) const = 0;
	double m_hitPoints;
};

class NachenBlaster : public Ship
{
public:
	NachenBlaster(StudentWorld * world);	// TODO: HP in constructor params?
	virtual void doSomething();
	virtual void increaseHP(double amt);
	void increaseTorpedo(int amt);

	int getHPPercent() const;
	int getCabbagePercent() const;
	int getTorpedoes() const;

private:
	virtual void playHitSound(int cause) const;
	int m_cabbageP;
	int m_torpedoes;
};


///////////////////////////////////////////////////////////////////////////////////


// Abstract
class Alien : public Ship
{
public:
	Alien(int imageID, double startX, double startY, double hp, double damage, double deltaX, double deltaY, double speed, unsigned int value, StudentWorld * world);
	virtual bool isAlien();
	virtual void doSomething();
	void collideWithProjectile(Projectile * p);

	void setFlightLength(int len);
	void setDeltaY(double dy);
	void setSpeed(double speed);

private:
	void checkCollision();
	virtual void playHitSound(int cause) const;
	void move();
	virtual void possiblyDropGoodie() = 0;
	virtual bool attackAI() = 0;	// returns true if we should end the current turn
	virtual void checkFlightPlan();
	void die();
	int m_flightLen;
	double m_speed;
	double m_deltaX, m_deltaY;
	double m_damage;
	unsigned int m_value;
};

class Smallgon : public Alien
{
public:
	Smallgon(double startX, double startY, StudentWorld * world);	
private:
	virtual void possiblyDropGoodie();
	virtual bool attackAI();
};

class Smoregon : public Alien
{
public:
	Smoregon(double startX, double startY, StudentWorld * world);
private:
	virtual void possiblyDropGoodie();
	virtual bool attackAI();
};

class Snagglegon : public Alien
{
public:
	Snagglegon(double startX, double startY, StudentWorld * world);
private:
	virtual void possiblyDropGoodie();
	virtual void checkFlightPlan();
	virtual bool attackAI();
};



// Abstract (doSomething)
class Goodie : public Actor
{
public:
	Goodie(int imageID, double startX, double startY, StudentWorld * world);
	virtual void doSomething();
private:
	virtual void checkCollision();
	virtual void grantReward(NachenBlaster* p) = 0;
};

class ExtraLifeGoodie : public Goodie
{
public:
	ExtraLifeGoodie(double startX, double startY, StudentWorld * world);
private:
	virtual void grantReward(NachenBlaster* p);
};

class RepairGoodie : public Goodie
{
public:
	RepairGoodie(double startX, double startY, StudentWorld * world);
private:
	virtual void grantReward(NachenBlaster* p);
};

class TorpedoGoodie : public Goodie
{
public:
	TorpedoGoodie(double startX, double startY, StudentWorld * world);
private:
	virtual void grantReward(NachenBlaster* p);
};
#endif // ACTOR_H_