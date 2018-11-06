#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <iostream>
#include <vector>
#include <list>

class Player;

class StudentWorld : public GameWorld

{
public:
	StudentWorld(std::string assetDir) : GameWorld(assetDir)
	{
        m_sublevel = 0;
        m_completed_level = false;
	}
    
    ~StudentWorld()
    {
        cleanUp();
    }

	virtual int init();
    
	virtual int move();

	virtual void cleanUp();
    
    virtual int getCurrentSubLevel() { return m_sublevel; }
    
    std::vector< std::vector< Actor* > > getVec();
    
    void push_back_actor(Actor* a, int sublevel)
    {
        vec[sublevel].push_back(a);
    }
    
    int getPlayerX();
    
    int getPlayerY();

    void increasePlayerAmmo(int howMuch) { m_ammo += howMuch; };
    
    int getAmmo() const { return m_ammo; }
    
    void decAmmo(){ m_ammo--; }
    
    void damagePlayer(int howMuch);
    
    int getPlayerHitpoints() const { return m_hitpoints; }
    
    void setPlayerHitpoints(int newHitpoints) { m_hitpoints = newHitpoints; }
    
    int getTimeLimit() const { return m_time_limit; };

    void setCurrentSubLevel(int sublevel)
    {
        m_sublevel = sublevel;
    }
    
    void setLevelComplete() { m_completed_level = true; }
    
private:
    void updateDisplayText();
    
    bool thePlayerHasFulfilledTheRequirements();
    
    void exposeTheExitInTheMaze();
    
    bool thePlayerCompletedTheCurrentLevel() { return m_completed_level;}
    
    std::vector< Player* > players;
    std::vector< std::vector< Actor* > > vec;
    int m_time_limit;
    int m_ammo;
    int m_hitpoints;
    int m_sublevel;
    bool m_completed_level;
};


class Wall : public Actor
{
public:
    Wall(int item_id, int x, int y, int subLevel) : Actor(item_id, x, y, none, subLevel) {}
};


class FakeWall : public Actor
{
public:
    FakeWall(int x, int y, int subLevel) : Actor(IID_FAKE_WALL, x, y, none, subLevel) {}
};


class Gold : public Actor
{
public:
    Gold(int item_id, int x, int y, int subLevel, StudentWorld* world) : Actor (item_id, x, y, none, subLevel, world) {}
    
    bool isPickedUp()
    {
        return (getWorld()->getPlayerX() == getX() && getWorld()->getPlayerY() == getY());
    }
    
    virtual void doSomething()
    {
        if (isAlive()){
            if (isPickedUp()){
                StudentWorld *world = getWorld();
                world->increaseScore(100);
                kill();
                world->playSound(SOUND_GOT_GOLD);
            }
        }
    }
};


class Waterpool : public Actor
{
public:
    Waterpool(int x, int y, int subLevel) : Actor(IID_WATERPOOL, x, y, none, subLevel)
    {
        m_hitpoints = 30;
    }
    
    virtual void doSomething()
    {
        if (isAlive()){
            if (m_hitpoints == 0){
                kill();
                return;
            }
            m_hitpoints--;
        }
    }
private:
    int m_hitpoints;
};


class Bullet : public Actor
{
public:
    Bullet(int image_id, int x, int y, int subLevel, Direction dir, StudentWorld* world) : Actor(image_id , x, y, dir, subLevel, world) {}
    
    virtual bool isObstructed(int x, int y)
    {
        StudentWorld *world = getWorld();
        int currentSubLevel = world->getCurrentSubLevel();
        std::vector< std::vector< Actor* > > vec = world->getVec();
        for (int i = 0; i < vec[currentSubLevel].size(); i++){
            int itemID = vec[currentSubLevel][i]->getID();
            int itemX = vec[currentSubLevel][i]->getX();
            int itemY = vec[currentSubLevel][i]->getY();
            if (itemX == x && itemY == y && (itemID == IID_WALL || itemID == IID_BULLY_NEST || itemID == IID_WATERPOOL)){
                return true;
            }
        }
        return false;
    }
    
    virtual void moveLeft()
    {
        moveTo(getX() - 1, getY());
    }
    
    virtual void moveRight()
    {
        moveTo(getX() + 1, getY());
    }
    
    virtual void moveUp()
    {
        moveTo(getX(), getY() + 1);
    }
    
    virtual void moveDown()
    {
        moveTo(getX(), getY() - 1);
    }
    
    // Moves the bullet by one square in the given direction
    void moveInDirection(Direction dir){
        if (dir == left){
            moveLeft();
        }
        if (dir == right){
            moveRight();
        }
        if (dir == up){
            moveUp();
        }
        if (dir == down){
            moveDown();
        }
    }
    
    virtual void doSomething()
    {
        if (isAlive()){
            // Checks for impact
            StudentWorld *world = getWorld();
            int currentSubLevel = world->getCurrentSubLevel();
            std::vector< std::vector< Actor* > > vec = world->getVec();
            // Checks to see if a bullet hit the player and damages it if so
            int bulletX = getX();
            int bulletY = getY();
            if (bulletX == world->getPlayerX() && bulletY == world->getPlayerY()){
                world->playSound(SOUND_PLAYER_IMPACT);
                world->damagePlayer(2);
                kill();
                return;
            }
            // Checks to see if a bullet hit one of the enemies and damages it if so
            for (int i = 0; i < vec[currentSubLevel].size(); i++){
                Actor *thisActor = vec[currentSubLevel][i];
                int thisActorX = thisActor->getX();
                int thisActorY = thisActor->getY();
                int thisActorID = thisActor->getID();
                if (bulletX == thisActorX && bulletY == thisActorY && (thisActorID == IID_GANGSTER || thisActorID == IID_BULLY || thisActorID == IID_ROBOT_BOSS)){
                    thisActor->damage();
                    kill();
                    return;
                }
            }
            // If the bullet is obstructed, kills it
            if (isObstructed(bulletX, bulletY)){
                kill();
                return;
            }
            
            // If the bullet did not hit the player, an enemy, or an obstruction, move it one square in
            // its current direction
            Direction dir = getDirection();
            moveInDirection(dir);
            
            // After moving, it once again checks to see if it hit the player, an enemy, or an obstruction
            world = getWorld();
            vec = world->getVec();
            // Checks to see if a bullet hit the player and damages it if so
            bulletX = getX();
            bulletY = getY();
            if (bulletX == world->getPlayerX() && bulletY == world->getPlayerY()){
                world->playSound(SOUND_PLAYER_IMPACT);
                world->damagePlayer(2);
                kill();
                return;
            }
            // Checks to see if a bullet hit one of the enemies and damages it if so
            for (int i = 0; i < vec[currentSubLevel].size(); i++){
                Actor *thisActor = vec[currentSubLevel][i];
                int thisActorX = thisActor->getX();
                int thisActorY = thisActor->getY();
                int thisActorID = thisActor->getID();
                if (bulletX == thisActorX && bulletY == thisActorY && (thisActorID == IID_GANGSTER || thisActorID == IID_BULLY || thisActorID == IID_ROBOT_BOSS)){
                    thisActor->damage();
                    kill();
                    return;
                }
            }
            // If the bullet is obstructed, kills it
            if (isObstructed(bulletX, bulletY)){
                kill();
                return;
            }
        }
    }
};


class Player : public Bullet
{
public:
    Player(int image_id, int x, int y, int subLevel, StudentWorld* world) : Bullet(image_id, x, y, subLevel, right, world)
    {
    }
    
    virtual bool isObstructed(int x, int y)
    {
        StudentWorld *world = getWorld();
        int currentSubLevel = world->getCurrentSubLevel();
        std::vector< std::vector< Actor* > > vec = getWorld()->getVec();
        for (int i = 0; i < vec[currentSubLevel].size(); i++){
            int itemID = vec[currentSubLevel][i]->getID();
            int itemX = vec[currentSubLevel][i]->getX();
            int itemY = vec[currentSubLevel][i]->getY();
            if (itemX == x && itemY == y && (itemID == IID_WATERPOOL || itemID == IID_WALL || itemID == IID_GANGSTER || itemID == IID_BULLY || itemID == IID_ROBOT_BOSS || itemID == IID_BULLY_NEST)){
                return true;
            }
        }
        return false;
    }
    
    void shoot()
    {
        StudentWorld *world = getWorld();
        Direction dir = getDirection();
        int currentSubLevel = world->getCurrentSubLevel();
        if (dir == left){
            Bullet *b = new Bullet(IID_BULLET, getX() - 1, getY(), currentSubLevel, dir, world);
            world->push_back_actor(b, currentSubLevel);
        }
        else if (dir == right){
            Bullet *b = new Bullet(IID_BULLET, getX() + 1, getY(), currentSubLevel, dir, world);
            world->push_back_actor(b, currentSubLevel);
        }
        else if (dir == up){
            Bullet *b = new Bullet(IID_BULLET, getX(), getY() + 1, currentSubLevel, dir, world);
            world->push_back_actor(b, currentSubLevel);
        }
        else if (dir == down){
            Bullet *b = new Bullet(IID_BULLET, getX(), getY() - 1, currentSubLevel, dir, world);
            world->push_back_actor(b, currentSubLevel);
        }
    }
    
    virtual void damage(int howMuch)
    {
        StudentWorld *world = getWorld();
        world->setPlayerHitpoints(world->getPlayerHitpoints() - howMuch);
        int newHitpoints = world->getPlayerHitpoints();
        if (newHitpoints <= 0){
            kill();
            world->decLives();
            world->playSound(SOUND_PLAYER_DIE);
        }
    }

    virtual void doSomething()
    {
        if (isAlive()){
            int keyInput;
            StudentWorld *world = getWorld();
            if (world->getKey(keyInput))
            {
                if (keyInput == KEY_PRESS_ESCAPE){
                    kill();
                    world->decLives();
                }
                if (keyInput == KEY_PRESS_LEFT){
                    setDirection(left);
                    if (!isObstructed(getX() - 1, getY())){
                        moveLeft();
                    }
                }
                else if (keyInput == KEY_PRESS_RIGHT){
                    setDirection(right);
                    if (!isObstructed(getX() + 1, getY())){
                        moveRight();
                    }
                }
                else if (keyInput == KEY_PRESS_UP){
                    setDirection(up);
                    if (!isObstructed(getX(), getY() + 1)){
                        moveUp();
                    }
                }
                else if (keyInput == KEY_PRESS_DOWN){
                    setDirection(down);
                    if (!isObstructed(getX(), getY() - 1)){
                        moveDown();
                    }
                }
                else if (keyInput == KEY_PRESS_SPACE && world->getAmmo() > 0){
                    shoot();
                    world->decAmmo();
                    world->playSound(SOUND_PLAYER_FIRE);
                }
            }
        }
    }
    
};


class Gangster : public Player
{
public:
    Gangster(int item_id, int x, int y, int subLevel, StudentWorld* world, Direction dir) : Player(item_id, x, y, subLevel, world)
    {
        setDirection(dir);
        setHitpoints(10);
        m_rest_duration = (28 - getWorld()->getLevel()) / 4;
        if (m_rest_duration < 3){
            m_rest_duration = 3;
        }
        m_ticks = 0;
    }
    
    ~Gangster()
    {
        getWorld()->playSound(SOUND_ENEMY_DIE);
    }
    
    void setHitpoints(int hitpoints)
    {
        m_hitpoints = hitpoints;
    }
    
    int getHitpoints() const
    {
        return m_hitpoints;
    }
    
    virtual void kill()
    {
        Actor::kill();
        StudentWorld *world = getWorld();
        int currentSubLevel = world->getCurrentSubLevel();
        Waterpool *w = new Waterpool(getX(), getY(), currentSubLevel);
        world->push_back_actor(w, currentSubLevel);
        world->increaseScore(100);
    }
    
    bool playerInView(){
        StudentWorld *world = getWorld();
        Direction bullyDir = getDirection();
        int bullyX = getX();
        int bullyY = getY();
        int playerX = world->getPlayerX();
        int playerY = world->getPlayerY();
        if (bullyX == playerX){
            if (bullyDir == up && playerY > bullyY){
                for (int y = bullyY + 1; y < playerY; y++){
                    if (isVisionObstructed(bullyX, y)){
                        return false;
                    }
                }
                return true;
            }
            else if (bullyDir == down && playerY < bullyY){
                for (int y = bullyY - 1; y > playerY; y--){
                    if (isVisionObstructed(bullyX, y)){
                        return false;
                    }
                }
                return true;
            }
        }
        else if (bullyY == playerY){
            if (bullyDir == right && playerX > bullyX){
                for (int x = bullyX + 1; x < playerX; x++){
                    if (isVisionObstructed(x, bullyY)){
                        return false;
                    }
                }
                return true;
            }
            else if (bullyDir == left && playerX < bullyX){
                for (int x = bullyX - 1; x > playerX; x--){
                    if (isVisionObstructed(x, bullyY)){
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }
    
    virtual void switchDirection()
    {
        Direction dir = getDirection();
        if (dir == right){
            setDirection(left);
        }
        else if (dir == left){
            setDirection(right);
        }
        else if (dir == up){
            setDirection(down);
        }
        else if (dir == down){
            setDirection(up);
        }
    }
    
    virtual void moveLeft()
    {
        setDirection(left);
        if (!isObstructed(getX() - 1, getY())){
            Bullet::moveLeft();
        }
        else {
            switchDirection();
        }
    }
    
    virtual void moveRight()
    {
        setDirection(right);
        if (!isObstructed(getX() + 1, getY())){
            Bullet::moveRight();
        }
        else {
            switchDirection();
        }
    }
    
    virtual void moveUp(){
        setDirection(up);
        if (!isObstructed(getX(), getY() + 1)){
            Bullet::moveUp();
        }
        else {
            switchDirection();
        }
    }
    
    virtual void moveDown(){
        setDirection(down);
        if (!isObstructed(getX(), getY() - 1)){
            Bullet::moveDown();
        }
        else {
            switchDirection();
        }
    }
    
    virtual void damage(){
        // Damages by 2 hitpoints as specified in the spec.
        setHitpoints(getHitpoints() - 2);
        if (getHitpoints() > 0){
            getWorld()->playSound(SOUND_ENEMY_IMPACT);
        }
        else {
            kill();
        }
    }
    
    virtual void doSomething()
    {
        if (isAlive()){
            if (m_ticks % m_rest_duration == 0){
                if (playerInView()){
                    shoot();
                    getWorld()->playSound(SOUND_ENEMY_FIRE);
                }
                else {
                    Direction dir = getDirection();
                    moveInDirection(dir);

                }
            }
            m_ticks++;
        }
    }
private:
    bool isVisionObstructed(int x, int y)
    {
        StudentWorld *world = getWorld();
        int currentSubLevel = world->getCurrentSubLevel();
        std::vector< std::vector< Actor* > > vec = world->getVec();
        for (int i = 0; i < vec[currentSubLevel].size(); i++){
            int itemID = vec[currentSubLevel][i]->getID();
            int itemX = vec[currentSubLevel][i]->getX();
            int itemY = vec[currentSubLevel][i]->getY();
            if (itemX == x && itemY == y && (itemID == IID_WATERPOOL || itemID == IID_WALL || itemID == IID_GANGSTER || itemID == IID_BULLY || itemID == IID_ROBOT_BOSS || itemID == IID_BULLY_NEST || itemID == IID_FAKE_WALL)){
                return true;
            }
        }
        return false;
    }
    
    int m_hitpoints;
    int m_rest_duration;
    int m_ticks;
};


class RestoreHealth : public Gold
{
public:
    RestoreHealth(int item_id, int x, int y, int subLevel, StudentWorld *world) : Gold(item_id, x, y, subLevel, world) {}
    
    virtual void doSomething()
    {
        if (isAlive()){
            if (isPickedUp()){
                StudentWorld *world = getWorld();
                world->increaseScore(250);
                kill();
                world->playSound(SOUND_GOT_GOODIE);
                world->setPlayerHitpoints(20);
            }
        }
    }
};


class Ammo : public Gold
{
public:
    Ammo(int item_id, int x, int y, int subLevel, StudentWorld* world) : Gold(item_id, x, y, subLevel, world) {}
    
    virtual void doSomething()
    {
        if (isAlive()){
            if (isPickedUp()){
                StudentWorld *world = getWorld();
                world->increaseScore(150);
                kill();
                world->playSound(SOUND_GOT_GOODIE);
                world->increasePlayerAmmo(25);
            }
        }
    }
};


class ExtraLife : public Gold
{
public:
    ExtraLife(int x, int y, int subLevel, StudentWorld *world) : Gold(IID_EXTRA_LIFE, x, y, subLevel, world)
    {
    }
    
    virtual void doSomething()
    {
        if (isAlive()){
            if (isPickedUp()){
                StudentWorld *world = getWorld();
                world->increaseScore(150);
                kill();
                world->playSound(SOUND_GOT_GOODIE);
                world->incLives();
            }
        }
    }
};


class Bully : public Gangster
{
public:
    Bully(int x, int y, int subLevel, StudentWorld *world) : Gangster(IID_BULLY, x, y, subLevel, world, right)
    {
        setHitpoints(5);
        m_distanceBeforeTurning = rand() % 6 + 1;
        m_ticks = 0;
        m_rest_duration = (28 - getWorld()->getLevel()) / 4;
        if (m_rest_duration < 3){
            m_rest_duration = 3;
        }
    }
    
    // Kills the bully,
    virtual void kill()
    {
        StudentWorld *world = getWorld();
        int currentSubLevel = world->getCurrentSubLevel();
        for (int i = 0; i < m_goodies.size(); i++){
            int thisID = m_goodies[i];
            if (thisID == IID_RESTORE_HEALTH){
                RestoreHealth *r = new RestoreHealth(IID_RESTORE_HEALTH, getX(), getY(), currentSubLevel, world);
                world->push_back_actor(r, currentSubLevel);
            }
            else if (thisID == IID_AMMO){
                Ammo *a = new Ammo(IID_AMMO, getX(), getY(), currentSubLevel, world);
                world->push_back_actor(a, currentSubLevel);
            }
            else if (thisID == IID_EXTRA_LIFE){
                ExtraLife *e = new ExtraLife(getX(), getY(), currentSubLevel, world);
                world->push_back_actor(e, currentSubLevel);
            }
        }
        Actor::kill();
        world->increaseScore(10);
    }
    
    virtual void doSomething()
    {
        if (isAlive()){
            if (m_ticks % m_rest_duration == 0){
                StudentWorld *world = getWorld();
                if (playerInView()){
                    world->playSound(SOUND_ENEMY_FIRE);
                    shoot();
                    m_ticks++;
                    return;
                }
                
                // Tries to pick up goodie if one is under the bully
                int currentSubLevel = world->getCurrentSubLevel();
                std::vector< std::vector< Actor* > > vec = world->getVec();
                bool ateGoodies = false;
                for (int i = 0; i < vec[currentSubLevel].size(); i++){
                    Actor *thisActor = vec[currentSubLevel][i];
                    int thisActorID = thisActor->getID();
                    int thisActorX = thisActor->getX();
                    int thisActorY = thisActor->getY();
                    if (thisActorX == getX() && thisActorY == getY() && (thisActorID == IID_AMMO || thisActorID == IID_RESTORE_HEALTH || thisActorID == IID_EXTRA_LIFE)){
                        ateGoodies = true;
                        m_goodies.push_back(thisActorID);
                        thisActor->kill();
                        world->playSound(SOUND_BULLY_MUNCH);
                        m_ticks++;
                    }
                }
                if (ateGoodies){
                    return;
                }
                
                // If there is no goodie, move
                if (m_distanceBeforeTurning != 0 && !isObstructedInDirection(getDirection())){
                    Direction dir = getDirection();
                    if (dir == left){
                        moveTo(getX() - 1, getY());
                        m_distanceBeforeTurning--;
                    }
                    else if (dir == right){
                        moveTo(getX() + 1, getY());
                        m_distanceBeforeTurning--;
                    }
                    else if (dir == up){
                        moveTo(getX(), getY() + 1);
                        m_distanceBeforeTurning--;
                    }
                    else if (dir == down){
                        moveTo(getX(), getY() - 1);
                        m_distanceBeforeTurning--;
                    }
                }
                else {
                    // if the bully is obstructed or has hit its distance before turning,
                    // change direction with a new random distance before turning
                    m_distanceBeforeTurning = rand() % 6 + 1;
                    std::list<Direction> dirList;
                    dirList.push_back(up);
                    dirList.push_back(down);
                    dirList.push_back(left);
                    dirList.push_back(right);
                    int randomIndex = rand() % dirList.size();
                    int count = 0;
                    std::list<Direction>::iterator it = dirList.begin();
                    while (count < randomIndex) {
                        it++;
                        count++;
                    }
                    Direction d = *it;
                    dirList.erase(it);
                    
                    while (isObstructedInDirection(d) && dirList.size() != 0){
                        int randomIndex = rand() % dirList.size();
                        int count = 0;
                        std::list<Direction>::iterator it = dirList.begin();
                        while (count < randomIndex) {
                            it++;
                            count++;
                        }
                        d = *it;
                        dirList.erase(it);
                    }
                    setDirection(d);
                    if (!isObstructedInDirection(d)){
                        if (d == left){
                            moveTo(getX() - 1, getY());
                            m_distanceBeforeTurning--;
                        }
                        else if (d == right){
                            moveTo(getX() + 1, getY());
                            m_distanceBeforeTurning--;
                        }
                        else if (d == up){
                            moveTo(getX(), getY() + 1);
                            m_distanceBeforeTurning--;
                        }
                        else if (d == down){
                            moveTo(getX(), getY() - 1);
                            m_distanceBeforeTurning--;
                        }
                    }
                }
            }
            m_ticks++;
        }
    }
private:
    bool isObstructedInDirection(Direction dir){
        StudentWorld *world = getWorld();
        if (dir == left){
            int checkX = getX() - 1;
            int checkY = getY();
            return (isObstructed(checkX, checkY) || (checkX == world->getPlayerX() && checkY == world->getPlayerY()));
        }
        else if (dir == right){
            int checkX = getX() + 1;
            int checkY = getY();
            return (isObstructed(checkX, checkY) || (checkX == world->getPlayerX() && checkY == world->getPlayerY()));
        }
        else if (dir == up){
            int checkX = getX();
            int checkY = getY() + 1;
            return (isObstructed(checkX, checkY) || (checkX == world->getPlayerX() && checkY == world->getPlayerY()));
        }
        else if (dir == down){
            int checkX = getX();
            int checkY = getY() - 1;
            return (isObstructed(checkX, checkY) || (checkX == world->getPlayerX() && checkY == world->getPlayerY()));
        }
        return true;
    }
    
    int m_distanceBeforeTurning;
    int m_ticks;
    int m_rest_duration;
    std::vector<int> m_goodies;
};


class BullyNest : public Actor
{
public:
    BullyNest(int x, int y, int subLevel, StudentWorld *world) : Actor(IID_BULLY_NEST, x, y, none, subLevel, world) {}
    
    bool bullyCanSpawn()
    {
        // needs to be no bully on nest and less than 3 bullies in range
        StudentWorld *world = getWorld();
        int currentSubLevel = world->getCurrentSubLevel();
        std::vector< std::vector< Actor* > > vec = world->getVec();
        int nestX = getX();
        int nestY = getY();
        int count = 0;
        
        // Checks all actors in the current sub level to see if they are bullies and either on the nest or within
        // range to block spawning if the count is high enough
        for (int i = 0; i < vec[currentSubLevel].size(); i++){
            Actor *thisActor = vec[currentSubLevel][i];
            if (thisActor->getID() == IID_BULLY){
                int thisActorX = thisActor->getX();
                int thisActorY = thisActor->getY();
                // If a bully is on the nest, a new bully can't spawn
                if (thisActorX == nestX && thisActorY == nestY){
                    return false;
                }
                // If a bully is not on the nest, but within range of plus or minus 3 in any direction, add it to the count
                // and return false if there are 3 or more bullies in range
                else if (thisActorX >= nestX - 3 && thisActorX <= nestX + 3 && thisActorY >= nestY - 3 && thisActorY <= nestY + 3){
                    count++;
                    if (count > 2){
                        return false;
                    }
                }
            }
        }
        return true;
    }
    
    // Spawns a new bully on the nest
    void spawnBully(){
        StudentWorld *world = getWorld();
        int currentSubLevel = world->getCurrentSubLevel();
        Bully *b = new Bully(getX(), getY(), currentSubLevel, world);
        world->playSound(SOUND_BULLY_BORN);
        world->push_back_actor(b, currentSubLevel);
    }
    
    virtual void doSomething()
    {
        if (bullyCanSpawn()){
            // Creates a random integer between 0 and 99
            int randomInt = rand() % 100;
            // If the random integer is 0 or 1, spawn a bully (1 in 50 chance)
            if (randomInt <= 1){
                spawnBully();
            }
        }
    }
};


class Exit : public Gold
{
public:
    Exit(int x, int y, StudentWorld *world) : Gold(IID_EXIT, x, y, 0, world)
    {
        setVisible(false);
    }
    
    virtual void doSomething()
    {
        if (isVisible() && isPickedUp()){
            kill();
            getWorld()->playSound(SOUND_FINISHED_LEVEL);
            // Increases the score by 1500 plus the bonus of the current remaining time limit
            getWorld()->increaseScore(1500 + getWorld()->getTimeLimit());
            getWorld()->setLevelComplete();
        }
    }
};

// Gate class
class Gate : public Gold
{
public:
    Gate(int x, int y, int subLevel, StudentWorld *world, int toSubLevel) : Gold(IID_GATE, x, y, subLevel, world)
    {
        m_toSubLevel = toSubLevel;
    }
    
    virtual void doSomething()
    {
        if (isAlive()){
            if (isPickedUp()){
                kill();
                getWorld()->setCurrentSubLevel(m_toSubLevel);
            }
        }
    }
private:
    int m_toSubLevel;
};


class Hostage : public Gold
{
public:
    Hostage(int x, int y, int subLevel, StudentWorld *world) : Gold(IID_HOSTAGE, x, y, subLevel, world)
    {
    }
    
    virtual void doSomething()
    {
        if (isPickedUp()){
            kill();
            getWorld()->playSound(SOUND_GOT_GOODIE);
        }
    }
};


class FarplaneGun : public Gold
{
public:
    FarplaneGun(int x, int y, int subLevel, StudentWorld* world) : Gold(IID_FARPLANE_GUN, x, y, subLevel, world) {}
    
    virtual void doSomething()
    {
        if (isAlive()){
            if (isPickedUp()){
                kill();
                StudentWorld *world = getWorld();
                int currentSubLevel = world->getCurrentSubLevel();
                world->playSound(SOUND_GOT_FARPLANE_GUN);
                std::vector< std::vector< Actor* > > vec = world->getVec();
                int farplaneX = getX();
                int farplaneY = getY();
                for (int i = 0; i < vec[currentSubLevel].size(); i++){
                    Actor *thisActor = vec[currentSubLevel][i];
                    int thisActorX = thisActor->getX();
                    int thisActorY = thisActor->getY();
                    int thisActorID = thisActor->getID();
                    if (farplaneX - 4 <= thisActorX && thisActorX <= farplaneX + 4
                        && farplaneY - 4 <= thisActorY && thisActorY <= farplaneY + 4){
                        if (thisActorID == IID_BULLY || thisActorID == IID_GANGSTER){
                            thisActor->kill();
                        }
                    }
                }
                getWorld()->damagePlayer(10);
            }
        }
    }
};


class RobotBoss : public Gangster
{
public:
    RobotBoss(int x, int y, int subLevel, StudentWorld *world) : Gangster(IID_ROBOT_BOSS, x, y, subLevel, world, right)
    {
        setHitpoints(50);
    }
    
    virtual void kill()
    {
        Actor::kill();
        StudentWorld *world = getWorld();
        int currentSubLevel = world->getCurrentSubLevel();
        Hostage *h = new Hostage(getX(), getY(), currentSubLevel, world);
        world->push_back_actor(h, currentSubLevel);
        world->increaseScore(3000);
    }
    
};

#endif // STUDENTWORLD_H_
