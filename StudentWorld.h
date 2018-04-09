#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"

#include <list>
#include <string>

class Actor;
class NachenBlaster;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
	virtual ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

	Actor* getOneCollidingAlien(const Actor* a) const;
	NachenBlaster* getCollidingPlayer(const Actor* a) const;
	bool playerInLineOfFire(const Actor* a) const;

	void killAlien();
	void createActor(Actor* act);
	

	/*
	Inherited
	unsigned int getLevel()	const;
	unsigned int getLives()	const;
	void	decLives();
	void	incLives();
	unsigned int getScore()	const;
	void increaseScore(unsigned int howMuch);
	void setGameStatText(string	text);
	bool getKey(int&	value);
	void playSound(int soundID);
	*/

private:
	bool collides(const Actor* a, const Actor* b) const;
	std::list<Actor*> m_actors;
	NachenBlaster* m_nachenBlaster;
	int m_level;
	int m_maxAliens, m_currAliens;
	int m_killReq;
};

#endif // STUDENTWORLD_H_
