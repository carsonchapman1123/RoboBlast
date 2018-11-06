#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int image_id, int x, int y, Direction dir, int subLevel, StudentWorld* world = nullptr);
    
    virtual void doSomething() {}
    
    StudentWorld* getWorld();

    bool isAlive();
    
    virtual void kill();
    
    virtual void damage() {}

private:
    bool m_alive;
    StudentWorld* m_world;
};

#endif // ACTOR_H_
