#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

// IDs for the game objects

const int IID_PLAYER = 0;
const int IID_GANGSTER = 1;
const int IID_BULLY = 2;
const int IID_HOSTAGE = 3;
const int IID_BULLY_NEST = 4;
const int IID_BULLET = 5;
const int IID_WALL = 6;
const int IID_FAKE_WALL = 7;
const int IID_EXIT = 8;
const int IID_GATE = 9;
const int IID_GOLD = 10;
const int IID_RESTORE_HEALTH = 11;
const int IID_EXTRA_LIFE = 12;
const int IID_AMMO = 13;
const int IID_STAR = 14;
const int IID_WATERPOOL = 15;
const int IID_FARPLANE_GUN = 16;
const int IID_ROBOT_BOSS = 17;

// sounds

const int SOUND_THEME = 0;
const int SOUND_ENEMY_DIE = 1;
const int SOUND_PLAYER_DIE = 2;
const int SOUND_ENEMY_FIRE = 3;
const int SOUND_PLAYER_FIRE = 4;
const int SOUND_GOT_GOODIE = 5;
const int SOUND_REVEAL_EXIT = 6;
const int SOUND_FINISHED_LEVEL = 7;
const int SOUND_BULLY_BORN = 8;
const int SOUND_ENEMY_IMPACT = 9;
const int SOUND_PLAYER_IMPACT = 10;
const int SOUND_BULLY_MUNCH = 11;
const int SOUND_BACKGROUND = 12;
const int SOUND_GOT_GOLD = 13;
const int SOUND_GOT_FARPLANE_GUN = 14;

const int SOUND_NONE = -1;

// keys the user can hit

const int KEY_PRESS_LEFT = 1000;
const int KEY_PRESS_RIGHT = 1001;
const int KEY_PRESS_UP = 1002;
const int KEY_PRESS_DOWN = 1003;
const int KEY_PRESS_SPACE = ' ';
const int KEY_PRESS_ESCAPE = '\x1b';
const int KEY_PRESS_TAB = '\t';

// board dimensions 

const int MAX_SUB_LEVEL = 6;
const int VIEW_WIDTH = 15;
const int VIEW_HEIGHT = 15;

// status of each tick (did the player die?)

const int GWSTATUS_PLAYER_DIED = 0;
const int GWSTATUS_CONTINUE_GAME = 1;
const int GWSTATUS_PLAYER_WON = 2;
const int GWSTATUS_FINISHED_LEVEL = 3;
const int GWSTATUS_LEVEL_ERROR = 4;

const double SPRITE_WIDTH_GL = .5; // note - this is tied implicitly to SPRITE_WIDTH due to carey's sloppy openGL programming
const double SPRITE_HEIGHT_GL = .5; // note - this is tied implicitly to SPRITE_HEIGHT due to carey's sloppy openGL programming


#endif // GAMECONSTANTS_H_
