#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <list>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir)
{
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}
bool StudentWorld::collides(const Actor* a, const Actor* b) const
{
	return sqrt(pow(a->getX() - b->getX(), 2) + pow(a->getY() - b->getY(), 2)) < (0.75 * (a->getRadius() + b->getRadius()));
}
Actor * StudentWorld::getOneCollidingAlien(const Actor * a) const
{
	list<Actor*>::const_iterator it = m_actors.cbegin();
	while (it != m_actors.end())
	{
		if ((*it)->isAlien())
		{
			int x = (*it)->getX();
			int y = (*it)->getY();
			int r = (*it)->getRadius();
			if (collides(*it, a))
				return (*it);
		}
		it++;
	}
	return nullptr;
}

NachenBlaster * StudentWorld::getCollidingPlayer(const Actor * a) const
{
	if (collides(m_nachenBlaster, a))
		return m_nachenBlaster;
	return nullptr;
}

bool StudentWorld::playerInLineOfFire(const Actor* a) const
{
	return abs((a->getY() - m_nachenBlaster->getY())) <= 4 && a->getX() > m_nachenBlaster->getX();
}

void StudentWorld::killAlien()
{
	m_killReq--;
}

void StudentWorld::createActor(Actor* act)
{
	m_actors.push_back(act);
}

/*
MUST keep track of all of the actors (e.g., aliens like Snagglegons, stars, explosions, projectiles like cabbages, goodies, etc.)
in a single STL collection such as a list or vector. (To do so, we recommend using a container of pointers to the actors)
m_nachenBlaster is ONLY actor allowed to not be stored in this
*/
int StudentWorld::init()
{
	// Initialize data structures to keep track of world
	for (int i = 0; i < 30; i++)
	{
		createActor(new Star(randInt(0, VIEW_WIDTH - 1), randInt(0, VIEW_HEIGHT - 1), this));
	}
	m_nachenBlaster = new NachenBlaster(this);

	m_level = getLevel();
	m_maxAliens = 4 + (0.5 * getLevel());
	m_currAliens = 0;
	m_killReq = 6 + (4 * m_level);

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	/*int input;
	if (getKey(input) && input == KEY_PRESS_ESCAPE)
	{
		//exit game
		;
	}*/
	list<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		(*it)->doSomething();
		it++;
	}
	it = m_actors.begin();
	while (it != m_actors.end())
	{
		if (!(*it)->isAlive())
		{
			if ((*it)->isAlien())
			{
				m_currAliens--;
			}
			delete *it;
			it = m_actors.erase(it);
			continue;
		}
		it++;
	}

	if (!m_nachenBlaster->isAlive())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	else {
		m_nachenBlaster->doSomething();
	}

	if (m_killReq == 0)
	{

		return GWSTATUS_FINISHED_LEVEL;
	}

	if (randInt(1, 15) == 1)
	{
		createActor(new Star(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), this));
	}
	if (m_currAliens < m_maxAliens && m_currAliens < m_killReq)	// if there is still room on screen & we haven't capped yet
	{
		int S1 = 60, S2 = 20 + getLevel() * 5, S3 = 5 + getLevel() * 10;
		int S = S1 + S2 + S3;
		int rng = randInt(1, S);
		
		if (rng <= S1)
		{
			createActor(new Smallgon(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), this));
		}
		else if (rng <= S1 + S2)
		{
			createActor(new Smoregon(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), this));
		}
		else {
			createActor(new Snagglegon(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), this));
		}
		m_currAliens++;
	}
	

	ostringstream oss;
	oss.setf(ios::fixed);
	oss.precision(2);
	oss << "Lives: " << getLives() << "  Health: " << (m_nachenBlaster->getHPPercent()) << "%  Score: " << getScore() << "  Level: " << getLevel() 
		<< "  Cabbages: " << m_nachenBlaster->getCabbagePercent() << "%  Torpedoes: " << m_nachenBlaster->getTorpedoes();
	setGameStatText(oss.str());
	//decLives();
	return GWSTATUS_CONTINUE_GAME;

	
	
	// Update status on top w/ latest info
			// setGameStatText() Lives: 3		Health:	100%		Score:	24530		Level:	3		Cabbages:	80%		Torpedoes:	4
	// Else return GWSTATUS_CONTINUE_GAME

	// press f to pause game, press r to resume, or any other key to call move() once
    
}

void StudentWorld::cleanUp()
{
	list<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		delete *it;
		it = m_actors.erase(it);
	}
	delete m_nachenBlaster;
	m_nachenBlaster = nullptr;
}
