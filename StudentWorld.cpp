#include "StudentWorld.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Initializes the StudentWorld for the current level
int StudentWorld::init()
{
    int currentLevel = getLevel();
    for (int subLevel = 0; subLevel <= MAX_SUB_LEVEL; subLevel++){
        ostringstream oss;
        oss << "level" << setfill('0') << setw(2) << currentLevel;
        if (subLevel > 0){
            oss << "_" << subLevel;
        }
        oss << ".dat";
        string fileName = oss.str();
        Level lev(assetDirectory());
        Level::LoadResult result = lev.loadLevel(fileName, subLevel);
        if ((subLevel == 0 && result == Level::load_fail_file_not_found) || currentLevel == 100){
            return GWSTATUS_PLAYER_WON;
        }
        if (result == Level::load_sublevel_fail_file_not_found){
            break;
        }
        if ((subLevel == 0 && result == Level::load_fail_bad_format) || (subLevel > 0 && result == Level::load_sublevel_fail_bad_format)){
            return GWSTATUS_LEVEL_ERROR;
        }
        
        vector< Actor* > newSubLevel;
        vec.push_back(newSubLevel);
        
        for (int i = 0; i < VIEW_WIDTH; i++){
            for (int j = 0; j < VIEW_HEIGHT; j++){
                Level::MazeEntry item = lev.getContentsOf(i, j, subLevel);
                if (item == Level::player){
                    Player *p = new Player(IID_PLAYER, i, j, subLevel, this);
                    players.push_back(p);
                }
                else if (item == Level::gold){
                    Gold *g = new Gold(IID_GOLD, i, j, subLevel, this);
                    vec[subLevel].push_back(g);
                }
                else if (item == Level::fake_wall){
                    FakeWall *f = new FakeWall(i, j, subLevel);
                    vec[subLevel].push_back(f);
                }
                else if (item == Level::wall){
                    Wall *w = new Wall(IID_WALL, i, j, subLevel);
                    vec[subLevel].push_back(w);
                }
                else if (item == Level::hostage){
                    Hostage *h = new Hostage(i, j, subLevel, this);
                    vec[subLevel].push_back(h);
                }
                else if (item == Level::extra_life){
                    cout << "Extra Life" << endl;
                    ExtraLife *e = new ExtraLife(i, j, subLevel, this);
                    vec[subLevel].push_back(e);
                }
                else if (item == Level::restore_health){
                    RestoreHealth *r = new RestoreHealth(IID_RESTORE_HEALTH, i, j, subLevel, this);
                    vec[subLevel].push_back(r);
                }
                else if (item == Level::ammo){
                    Ammo *a = new Ammo(IID_AMMO, i, j, subLevel, this);
                    vec[subLevel].push_back(a);
                }
                else if (item == Level::farplane_gun){
                    FarplaneGun *f = new FarplaneGun(i, j, subLevel, this);
                    vec[subLevel].push_back(f);
                }
                else if (item == Level::vert_gangster){
                    Gangster *v = new Gangster(IID_GANGSTER, i, j, subLevel, this, Actor::Direction::down);
                    vec[subLevel].push_back(v);
                }
                else if (item == Level::horiz_gangster){
                    Gangster *h = new Gangster(IID_GANGSTER, i, j, subLevel, this, Actor::Direction::right);
                    vec[subLevel].push_back(h);
                }
                else if (item == Level::robot_boss){
                    RobotBoss *r = new RobotBoss(i, j, subLevel, this);
                    vec[subLevel].push_back(r);
                }
                else if (item == Level::bully_nest){
                    BullyNest *b = new BullyNest(i, j, subLevel, this);
                    vec[subLevel].push_back(b);
                }
                else if (item == Level::gate0){
                    Gate *g = new Gate(i, j, subLevel, this, 0);
                    vec[subLevel].push_back(g);
                }
                else if (item == Level::gate1){
                    Gate *g = new Gate(i, j, subLevel, this, 1);
                    vec[subLevel].push_back(g);
                }
                else if (item == Level::gate2){
                    Gate *g = new Gate(i, j, subLevel, this, 2);
                    vec[subLevel].push_back(g);
                }
                else if (item == Level::gate3){
                    Gate *g = new Gate(i, j, subLevel, this, 3);
                    vec[subLevel].push_back(g);
                }
                else if (item == Level::gate4){
                    Gate *g = new Gate(i, j, subLevel, this, 4);
                    vec[subLevel].push_back(g);
                }
                else if (item == Level::gate5){
                    Gate *g = new Gate(i, j, subLevel, this, 5);
                    vec[subLevel].push_back(g);
                }
                else if (item == Level::exit){
                    Exit *e = new Exit(i, j, this);
                    vec[subLevel].push_back(e);
                }
            }
        }
    }
    
    setCurrentSubLevel(0);
    
    m_time_limit = 3000;
    m_hitpoints = 20;
    m_ammo = 0;
    return GWSTATUS_CONTINUE_GAME;
}

// Moves all players in the game
int StudentWorld::move()
{

    updateDisplayText();
    
    int currentSubLevel = getCurrentSubLevel();
    for (int i = (int) vec[currentSubLevel].size() - 1; i >= 0; i--){
        vec[currentSubLevel][i]->doSomething();
    }
    players[m_sublevel]->doSomething();
    
    // Delete dead objects
    for (int i = 0; i < vec[currentSubLevel].size(); i++){
        if (!vec[currentSubLevel][i]->isAlive()){
            Actor *dead = vec[currentSubLevel][i];
            for (int j = i; j < vec[currentSubLevel].size() - 1; j++){
                vec[currentSubLevel][j] = vec[currentSubLevel][j + 1];
            }
            vec[currentSubLevel].pop_back();
            delete dead;
        }
    }
    
    if (thePlayerHasFulfilledTheRequirements()){
        exposeTheExitInTheMaze();
    }
    
    if (!players[m_sublevel]->isAlive() || m_time_limit == 0){
        return GWSTATUS_PLAYER_DIED;
    }
    
    if (m_completed_level){
        m_completed_level = false;
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    m_time_limit--;
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (int i = (int) players.size() - 1; i >= 0; i--){
        Player *killMe = players[i];
        players.pop_back();
        delete killMe;
    }
    for (int i = 0; i < vec.size(); i++){
        for (int j = (int) vec[i].size() - 1; j >= 0; j--){
            Actor *killMe = vec[i][j];
            vec[i].pop_back();
            delete killMe;
        }
    }
}

// returns the vector of sublevels
std::vector< std::vector< Actor* > > StudentWorld::getVec()
{
    return vec;
}

// returns the x coordinate of the player in the current sublevel
int StudentWorld::getPlayerX()
{
    return players[m_sublevel]->getX();
}

// returns the y coordinate of the player in the current sublevel
int StudentWorld::getPlayerY()
{
    return players[m_sublevel]->getY();
}

// Damages the player in the current sub level
void StudentWorld::damagePlayer(int howMuch){
    players[m_sublevel]->damage(howMuch);
}

// Helper function to format the output of the display text
string formatDisplayText(int score, int level, int subLevel, int lives, int health, int ammo, int timeLimit)
{
    ostringstream oss;
    oss << "Score: " << setfill('0') << setw(7) << score << "  Level: " << setw(2) << level << "-" << setw(2) << subLevel
        << "  Lives: " << setfill(' ') << setw(2) << lives << "  Health: " << setw(3) << health << "%  Ammo: " << setw(3) << ammo
        << "  TimeLimit: " << setw(4) << timeLimit;
    string formattedText = oss.str();
    return formattedText;
}

// Updates the display text based on the current game variables
void StudentWorld::updateDisplayText()
{
    int score = getScore();
    int level = getLevel();
    int subLevel = getCurrentSubLevel();
    int lives = getLives();
    int percentHealth = getPlayerHitpoints() / 20.0 * 100;
    int ammo = getAmmo();
    int timeLimit = getTimeLimit();
    
    string formattedText = formatDisplayText(score, level, subLevel, lives, percentHealth, ammo, timeLimit);
    setGameStatText(formattedText);
}

// Returns true if the player has killed all robot bosses, saved all hostages, collected all gold,
// and the exit is not already visible.
bool StudentWorld::thePlayerHasFulfilledTheRequirements()
{
    for (int i = 0; i < vec.size(); i++){
        for (int j = 0; j < vec[i].size(); j++){
            int thisID = vec[i][j]->getID();
            if (thisID == IID_GOLD || thisID == IID_ROBOT_BOSS || thisID == IID_HOSTAGE){
                return false;
            }
            if (thisID == IID_EXIT && vec[i][j]->isVisible()){
                return false;
            }
        }
    }
    return true;
}

// Reveals the exit to the player to advance to the next level
void StudentWorld::exposeTheExitInTheMaze()
{
    for (int i = 0; i < vec[0].size(); i++){
        if (vec[0][i]->getID() == IID_EXIT){
            playSound(SOUND_REVEAL_EXIT);
            vec[0][i]->setVisible(true);
        }
    }
}
