#include "Actor.h"
#include "StudentWorld.h"

// Constructor for an actor object
Actor::Actor(int image_id, int x, int y, Direction dir, int subLevel, StudentWorld* world) : GraphObject(image_id, x, y, dir, subLevel)
{
    setVisible(true);
    m_alive = true;
    m_world = world;
}

// Returns the actor's StudentWorld
StudentWorld* Actor::getWorld()
{
    return m_world;
}

// Returns whether or not the actor is alive
bool Actor::isAlive()
{
    return m_alive;
}

// Kills the actor to mark it for cleanup
void Actor::kill(){
    m_alive = false;
}
