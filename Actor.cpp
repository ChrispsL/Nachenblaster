#include "Actor.h"
#include "StudentWorld.h"

const int HIT_BY_SHIP = 0;
const int HIT_BY_PROJECTILE = 0;

/*
*	
*	Actor
*
*/
Actor::Actor(int imageID, double startX, double startY, StudentWorld * world, int startDirection = 0, double size = 1.0, int depth = 0)
	: GraphObject(imageID, startX, startY, startDirection, size, depth), m_world(world), m_isAlive(true) {};
void Actor::setDead() {
	m_isAlive = false;
}
bool Actor::isAlive() const {
	return m_isAlive;
}
StudentWorld* Actor::getWorld() const {
	return m_world;
}
bool Actor::isAlien()
{
	return false;
}
void Actor::moveTo(double x, double y)
{
	if (x < 0 || x >= VIEW_WIDTH-1 || y < 0 || y > VIEW_HEIGHT-1)
	{
		setDead();
		return;
	}
	GraphObject::moveTo(x, y);
}


/*
*
*	Star
*
*/
Star::Star(double startX, double startY, StudentWorld * world)
	: Actor(IID_STAR, startX, startY, world, 0, static_cast<double>(randInt(5, 50)) / 100.0, 3) {}

void Star::doSomething()
{
	moveTo(getX() - 1, getY());
}


/*
*
*	Explosion
*
*/
Explosion::Explosion(double startX, double startY, StudentWorld * world)
	: Actor(IID_EXPLOSION, startX, startY, world, 0, 1, 0), m_timer(1) {}
void Explosion::doSomething()
{
	if (m_timer >= 4)
		setDead();
	if (!isAlive())
		return;

	setSize(getSize() * 1.5);

	m_timer++;
}


/*
*
*	Projectile
*
*/
Projectile::Projectile(int imageID, double startX, double startY, double damage, double deltaX, bool rotates, StudentWorld * world, int startDirection)
	: Actor(imageID, startX, startY, world, startDirection, 0.5, 1), m_damage(damage), m_rotates(rotates), m_deltaX(deltaX) { }
double Projectile::getDamage() const
{
	return m_damage;
}
void Projectile::doSomething()
{
	if (!isAlive())
		return;

	checkCollision();

	moveTo(getX() + m_deltaX, getY());
	if(m_rotates)
		setDirection(getDirection() + 20);

	checkCollision();
}

		// Cabbage //
Cabbage::Cabbage(double startX, double startY, StudentWorld * world)
	: Projectile(IID_CABBAGE, startX, startY, 2, 8, true, world, 0) { }
void Cabbage::checkCollision()
{
	Actor* hit = getWorld()->getOneCollidingAlien(this);
	if (hit != nullptr)
	{
		static_cast<Alien*>(hit)->collideWithProjectile(this);
		setDead();
	}
}

		// Turnip //
Turnip::Turnip(double startX, double startY, StudentWorld * world)
: Projectile(IID_TURNIP, startX, startY, 2, -6, true, world, 0) { }
void Turnip::checkCollision()
{
	NachenBlaster* hit = getWorld()->getCollidingPlayer(this);
	if (hit != nullptr)
	{
		hit->sufferDamage(getDamage(), HIT_BY_PROJECTILE);
		setDead();
	}
}
		// Flatulence Torpedoes //
FlatulenceTorpedo::FlatulenceTorpedo(double startX, double startY, double deltaX, StudentWorld * world, int startDirection)
	: Projectile(IID_TORPEDO, startX, startY, 8, deltaX, false, world, startDirection) {}
PlayerTorpedo::PlayerTorpedo(double startX, double startY, StudentWorld * world)
	: FlatulenceTorpedo(startX, startY, 8, world, 0) {}
void PlayerTorpedo::checkCollision()
{
	Actor* hit = getWorld()->getOneCollidingAlien(this);
	if (hit != nullptr)
	{
		static_cast<Alien*>(hit)->collideWithProjectile(this);
		setDead();
	}
}
AlienTorpedo::AlienTorpedo(double startX, double startY, StudentWorld * world)
	: FlatulenceTorpedo(startX, startY, -8, world, 180) {}

void AlienTorpedo::checkCollision()
{
	NachenBlaster* hit = getWorld()->getCollidingPlayer(this);
	if (hit != nullptr)
	{
		hit->sufferDamage(getDamage(), HIT_BY_PROJECTILE);
		setDead();
	}
}



/*
*
*	Goodie
*
*/
Goodie::Goodie(int imageID, double startX, double startY, StudentWorld * world) : Actor(imageID, startX, startY, world, 0, 0.5, 1) { }
void Goodie::doSomething()
{
	if (!isAlive())
		return;
	checkCollision();
	moveTo(getX() - 0.75, getY() - 0.75);
	checkCollision();
}
void Goodie::checkCollision()
{
	NachenBlaster* hit = getWorld()->getCollidingPlayer(this);
	if (hit != nullptr)
	{
		getWorld()->increaseScore(100);
		setDead();
		getWorld()->playSound(SOUND_GOODIE);

		grantReward(hit);
		return;
	}
}
		// Extra Life //
ExtraLifeGoodie::ExtraLifeGoodie(double startX, double startY, StudentWorld * world)
	: Goodie(IID_LIFE_GOODIE, startX, startY, world) {}
void ExtraLifeGoodie::grantReward(NachenBlaster* p)
{
	getWorld()->incLives();
}

		// Repair //
RepairGoodie::RepairGoodie(double startX, double startY, StudentWorld * world)
	: Goodie(IID_REPAIR_GOODIE, startX, startY, world) {}
void RepairGoodie::grantReward(NachenBlaster* p)
{
	p->increaseHP(10);
}

		// Torpedo //
TorpedoGoodie::TorpedoGoodie(double startX, double startY, StudentWorld * world)
	: Goodie(IID_TORPEDO, startX, startY, world) {}
void TorpedoGoodie::grantReward(NachenBlaster* p)
{
	p->increaseTorpedo(5);
}




/*
*
*	Ship
*
*/
Ship::Ship(int imageID, double startX, double startY, double hp, StudentWorld * world, int startDirection, double size, int depth)
	: Actor(imageID, startX, startY, world, startDirection, size, depth), m_hitPoints(hp) { }
double Ship::getHP() const
{
	return m_hitPoints;
}
void Ship::increaseHP(double amt)
{
	m_hitPoints += amt;
}
void Ship::sufferDamage(double damage, int cause)
{
	m_hitPoints -= damage;
	if (m_hitPoints <= 0)
	{
		setDead();
	}
	playHitSound(cause);
}

		// Nachenblaster (player) //
NachenBlaster::NachenBlaster(StudentWorld * world)
	: Ship(IID_NACHENBLASTER, 0, 128, 50, world, 0, 1.0, 0), m_cabbageP(30), m_torpedoes(0) {}
void NachenBlaster::doSomething()
{
	if (!isAlive())
		return;
	
	int input;
	if (getWorld()->getKey(input))
	{
		// Space: Cabbage if 5 cabbage points, cabbage 12px right of blaster w/ 0 degrees, play sound SOUND_PLAYER_SHOOT --> give to StudentWorld obj
		// Tab: Flatulence torpedo, 12px to the right, play SOUND_TORPEDO
		if (input == KEY_PRESS_SPACE && m_cabbageP > 5)
		{
			m_cabbageP -= 5;
			getWorld()->createActor(new Cabbage(getX() + 12, getY(), getWorld()));
			getWorld()->playSound(SOUND_PLAYER_SHOOT);
		}
		else if (input == KEY_PRESS_TAB)
		{
			if (m_torpedoes > 0)
			{
				m_torpedoes--;
				getWorld()->createActor(new PlayerTorpedo(getX() + 12, getY(), getWorld()));
				getWorld()->playSound(SOUND_TORPEDO);
			}
		}
		else if (input == KEY_PRESS_LEFT && getX() > 5)
			moveTo(getX() - 6, getY());
		else if (input == KEY_PRESS_RIGHT && getX() <= (VIEW_WIDTH-6))
			moveTo(getX() + 6, getY());
		else if (input == KEY_PRESS_UP && getY() <= (VIEW_HEIGHT-6))
			moveTo(getX(), getY() + 6);
		else if (input == KEY_PRESS_DOWN && getY() > 5)
			moveTo(getX(), getY() - 6);
	}
	
	if (m_cabbageP < 30) m_cabbageP++;
}
void NachenBlaster::increaseHP(double amt)
{
	if (getHP() + amt <= 50)
		Ship::increaseHP(amt);
	else
		Ship::increaseHP(50 - getHP());
}
void NachenBlaster::playHitSound(int cause) const
{
	if (cause == HIT_BY_PROJECTILE)
	{
		getWorld()->playSound(SOUND_BLAST);
	}
}
void NachenBlaster::increaseTorpedo(int amt)
{
	m_torpedoes += amt;
}
int NachenBlaster::getHPPercent() const
{
	return (getHP() * 100 / 50);
}
int NachenBlaster::getCabbagePercent() const
{
	return (m_cabbageP * 100) / 30;
}
int NachenBlaster::getTorpedoes() const
{
	return m_torpedoes;
}



/*
*
*	Ship.Alien
*
*/
Alien::Alien(int imageID, double startX, double startY, double hp, double damage, double deltaX, double deltaY, double speed, unsigned int value, StudentWorld * world)
	: Ship(imageID, startX, startY, hp, world, 0, 1.5, 1), m_flightLen(0), m_speed(speed), m_deltaX(deltaX),
	m_deltaY(deltaY), m_damage(damage), m_value(value) { }
bool Alien::isAlien()
{
	return true;
}

void Alien::doSomething()
{
	if (!isAlive())
		return;
	if (getX() < 0)
	{
		setDead();
		return;
	}
	checkCollision();
	// check flight plan
	checkFlightPlan();

	// AI shoot
	if (getWorld()->playerInLineOfFire(this))
	{
		if(attackAI()) return;
	}
	move();
	checkCollision();
}
void Alien::checkCollision()
{
	NachenBlaster* crash = getWorld()->getCollidingPlayer(this);
	if (crash != nullptr)
	{
		crash->sufferDamage(m_damage, HIT_BY_SHIP);	// will play death sound
		die();
	}

}
void Alien::collideWithProjectile(Projectile * p)
{
	sufferDamage(p->getDamage(), HIT_BY_PROJECTILE);
	if (!isAlive()) die();	// if damage killed the alien, process it
}
void Alien::die()
{
	setDead();	// can't hurt to beat a dead alien
	getWorld()->increaseScore(m_value);
	getWorld()->killAlien();
	getWorld()->createActor(new Explosion(getX(), getY(), getWorld()));
	possiblyDropGoodie();
}
void Alien::checkFlightPlan()
{
	if (m_flightLen <= 0 || getY() >= VIEW_HEIGHT - 1 || getY() <= 0)
	{
		if (getY() >= VIEW_HEIGHT - 1)
		{
			setDeltaY(-1);
		}
		else if (getY() < 0)
		{
			setDeltaY(1);
		}
		else if (m_flightLen <= 0)
		{
			int randDir = randInt(1, 3);
			if (randDir == 1)
				setDeltaY(0);
			else if (randDir == 2)
				setDeltaY(1);
			else if (randDir == 3)
				setDeltaY(-1);
		}
		m_flightLen = randInt(1, 32);
	}
}
void Alien::move()
{
	GraphObject::moveTo(getX() + m_speed * m_deltaX, getY() + m_speed * m_deltaY);
	m_flightLen--;
}
void Alien::setFlightLength(int len)
{
	m_flightLen = len;
}
void Alien::setDeltaY(double dy)
{
	m_deltaY = dy;
}
void Alien::setSpeed(double speed)
{
	m_speed = speed;
}
void Alien::playHitSound(int cause) const
{
	if (cause == HIT_BY_PROJECTILE && isAlive())
	{
		getWorld()->playSound(SOUND_BLAST);
	}
	else {
		getWorld()->playSound(SOUND_DEATH);
	}
}
		// Smallgon //
Smallgon::Smallgon(double startX, double startY, StudentWorld * world)
	: Alien(IID_SMALLGON, startX, startY, (5 * (1 + (world->getLevel() - 1) * 0.1)), 5, -1, 1, 2.0, 250, world) {}
// *************************************** deltaX and Y arguments?
bool Smallgon::attackAI()
{
	if (randInt(1, (20 / getWorld()->getLevel()) + 5) == 1)
	{
		getWorld()->createActor(new Turnip(getX() - 14, getY(), getWorld()));
		getWorld()->playSound(SOUND_ALIEN_SHOOT);
		return true;
	}
	return false;
}
void Smallgon::possiblyDropGoodie()
{
	// drops nothing
}

		// Smoregon //
Smoregon::Smoregon(double startX, double startY, StudentWorld * world)
	: Alien(IID_SMOREGON, startX, startY, (5 * (1 + (world->getLevel() - 1) * 0.1)), 5, -1, 1, 2.0, 250, world) {}
bool Smoregon::attackAI()
{
	if (randInt(1, (20 / getWorld()->getLevel()) + 5) == 1)
	{
		getWorld()->createActor(new Turnip(getX() - 14, getY(), getWorld()));
		getWorld()->playSound(SOUND_ALIEN_SHOOT);
		return true;
	}
	if (randInt(1, (20 / getWorld()->getLevel()) + 5) == 1)
	{
		//change flight plan
		setFlightLength(VIEW_WIDTH);
		setDeltaY(0);
		setSpeed(5.0);
	}
	return false;
}
void Smoregon::possiblyDropGoodie()
{
	if (randInt(1, 3) == 1)
	{
		if (randInt(1, 2) == 1)
			getWorld()->createActor(new RepairGoodie(getX(), getY(), getWorld()));
		else
			getWorld()->createActor(new TorpedoGoodie(getX(), getY(), getWorld()));
	}
}
		// Snagglegon //
Snagglegon::Snagglegon(double startX, double startY, StudentWorld * world)
	: Alien(IID_SNAGGLEGON, startX, startY, (10*(1+(world->getLevel()-1)*0.1)), 15, -1, 1, 1.75, 1000, world) {}
bool Snagglegon::attackAI()
{
	if (randInt(1, (15 / getWorld()->getLevel()) + 10) == 1)
	{
		getWorld()->createActor(new AlienTorpedo(getX() - 14, getY(), getWorld()));
		getWorld()->playSound(SOUND_TORPEDO);
		return true;
	}
	return false;
}
void Snagglegon::possiblyDropGoodie()
{
	if (randInt(1, 6) == 1)
	{
		getWorld()->createActor(new ExtraLifeGoodie(getX(), getY(), getWorld()));
	}
}

void Snagglegon::checkFlightPlan()
{
	if (getY() >= VIEW_HEIGHT - 1 || getY() < 0)
	{
		if (getY() >= VIEW_HEIGHT - 1)
		{
			setDeltaY(-1);
		}
		else if (getY() < 0)
		{
			setDeltaY(1);
		}
	}
}