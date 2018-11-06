#include "freeglut.h"
#include "GameController.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "SoundFX.h"
#include "SpriteManager.h"
#include <string>
#include <map>
#include <utility>
#include <cstdlib>
#include <algorithm>
using namespace std;

/*
spriteWidth = .67
spritesPerRow = 16

RowWidth = spriteWidth*spritesPerRow = 10.72
PixelWidth = RowWidth/256 = .041875
newSpriteWidth = PixelWidth * NumPixels

spriteHeight = .54
spritesPerRow = 16

RowHeight = spriteHeight*spritesPerCol = 8.64

PixelHeight = RowHeight/256 = .03375

newSpriteHeight = PixelHeight * NumPixels
*/

static const int WINDOW_WIDTH = 768; //1024;
static const int WINDOW_HEIGHT = 768;

static const int PERSPECTIVE_NEAR_PLANE = 4;
static const int PERSPECTIVE_FAR_PLANE	= 22;

static const double VISIBLE_MIN_X = -2.15; // -2.04375; //	-2.4375; //-3.25;
static const double VISIBLE_MAX_X = 1.85; //  2.04375;	  //1.65;// 2.4375; //3.25;
static const double VISIBLE_MIN_Y = -2.1;
static const double VISIBLE_MAX_Y = 1.9;
static const double VISIBLE_MIN_Z = -20;
//static const double VISIBLE_MAX_Z = -6;

static const double FONT_SCALEDOWN = 760.0;

static const double SCORE_Y = 3.8;
static const double SCORE_Z = -10;

static const int MS_PER_FRAME = 10;

static const double PI = 4 * atan(1.0);

struct SpriteInfo
{
	unsigned int imageID;
	unsigned int frameNum;
	std::string	 tgaFileName;
};

static void convertToGlutCoords(double x, double y, double& gx, double& gy, double& gz);
static void drawPrompt(string mainMessage, string secondMessage);
static void drawScoreAndLives(string);

void GameController::initDrawersAndSounds()
{
	SpriteInfo drawers[] = {
		{ IID_PLAYER			, 0, "dude_1.tga" },
		{ IID_PLAYER			, 1, "dude_2.tga" },
		{ IID_PLAYER			, 2, "dude_3.tga" },
		{ IID_BULLY 			, 0, "bully1.tga" },
		{ IID_BULLY 			, 1, "bully2.tga" },
		{ IID_BULLY 			, 2, "bully3.tga" },
		{ IID_BULLY 			, 3, "bully4.tga" },
		{ IID_GANGSTER			, 0, "gangster1.tga" },
		{ IID_GANGSTER			, 1, "gangster2.tga" },
		{ IID_GANGSTER			, 2, "gangster3.tga" },
		{ IID_BULLET			, 0, "bullet.tga" },
		{ IID_BULLY_NEST		, 0, "factory.tga" },
		{ IID_GOLD				, 0, "gold.tga" },
		{ IID_RESTORE_HEALTH	, 0, "medkit.tga" },
		{ IID_EXTRA_LIFE		, 0, "extralife.tga" },
		{ IID_AMMO				, 0, "ammo.tga" },
		{ IID_EXIT				, 0, "exit.tga" },
		{ IID_WALL				, 0, "wall.tga" },
		{ IID_FAKE_WALL         , 0, "wall.tga" },
		{ IID_GATE				, 0, "gate.tga" },
		{ IID_HOSTAGE			, 0, "hostage2.tga" },
		{ IID_WATERPOOL			, 0, "water1.tga"},
		{ IID_WATERPOOL			, 1, "water2.tga" },
		{ IID_WATERPOOL			, 2, "water3.tga" },
		{ IID_FARPLANE_GUN		, 0, "farplanegun.tga"},
		{ IID_ROBOT_BOSS 		, 0, "robotboss1.tga" },
		{ IID_ROBOT_BOSS 		, 1, "robotboss2.tga" },
		{ IID_ROBOT_BOSS 		, 2, "robotboss3.tga" },
		{ IID_ROBOT_BOSS 		, 3, "robotboss4.tga" },
	};

	SoundMapType::value_type sounds[] = {
		make_pair(SOUND_THEME			, "theme.wav"),
	    //make_pair(SOUND_BACKGROUND		, "cave1.mp3"),
		make_pair(SOUND_PLAYER_FIRE		, "torpedo.wav"),
		make_pair(SOUND_ENEMY_FIRE		, "pop.wav"),
		make_pair(SOUND_ENEMY_DIE		, "explode.wav"),
		make_pair(SOUND_PLAYER_DIE		, "die.wav"),
		make_pair(SOUND_GOT_GOODIE		, "goodie.wav"),
		make_pair(SOUND_REVEAL_EXIT		, "revealexit.wav"),
		make_pair(SOUND_FINISHED_LEVEL	, "finished.wav"),
		make_pair(SOUND_ENEMY_IMPACT	, "clank.wav"),
		make_pair(SOUND_PLAYER_IMPACT	, "ouch.wav"),
		make_pair(SOUND_BULLY_MUNCH		, "munch.wav"),
		make_pair(SOUND_BULLY_BORN		, "materialize.wav"),
		make_pair(SOUND_GOT_GOLD        , "woohoo.wav"),
		make_pair(SOUND_GOT_FARPLANE_GUN, "farplanegun.wav"),
	};

	for (int k = 0; k < sizeof(drawers)/sizeof(drawers[0]); k++)
	{
		string path = m_gw->assetDirectory();
		if (!path.empty())
			path += '/';
		const SpriteInfo& d = drawers[k];
		if (!m_spriteManager.loadSprite(path + d.tgaFileName, d.imageID, d.frameNum))
			exit(0);
	}
	for (int k = 0; k < sizeof(sounds)/sizeof(sounds[0]); k++)
		m_soundMap[sounds[k].first] = sounds[k].second;
}

static void doSomethingCallback()
{
	Game().doSomething();
}

static void reshapeCallback(int w, int h)
{
	Game().reshape(w, h);
}

static void keyboardEventCallback(unsigned char key, int x, int y)
{
	Game().keyboardEvent(key, x, y);
}

static void specialKeyboardEventCallback(int key, int x, int y)
{
	Game().specialKeyboardEvent(key, x, y);
}

static void timerFuncCallback(int val)
{
	Game().doSomething();
	glutTimerFunc(MS_PER_FRAME, timerFuncCallback, 0);
}

void GameController::run(int argc, char* argv[], GameWorld* gw, string windowTitle)
{
	gw->setController(this);
	m_gw = gw;
	setGameState(welcome);
	m_lastKeyHit = INVALID_KEY;
	m_singleStep = false;
	m_curIntraFrameTick = 0;
	m_playerWon = false;

    glutInit(&argc, argv);
    
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(windowTitle.c_str());

	initDrawersAndSounds();

	glutKeyboardFunc(keyboardEventCallback);
	glutSpecialFunc(specialKeyboardEventCallback);
	glutReshapeFunc(reshapeCallback);
	glutDisplayFunc(doSomethingCallback);
	glutTimerFunc(MS_PER_FRAME, timerFuncCallback, 0);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
	delete m_gw;
}

void GameController::keyboardEvent(unsigned char key, int /* x */, int /* y */)
{
	switch (key)
	{
	case 'a': case '4': m_lastKeyHit = KEY_PRESS_LEFT;  break;
	case 'd': case '6': m_lastKeyHit = KEY_PRESS_RIGHT; break;
	case 'w': case '8': m_lastKeyHit = KEY_PRESS_UP;	break;
	case 's': case '2': m_lastKeyHit = KEY_PRESS_DOWN;  break;
	case 'f':           m_singleStep = true;			break;
	case 'r':           m_singleStep = false;			break;
	case 'q': case 'Q': setGameState(quit);				break;
	default:            m_lastKeyHit = key;				break;
	}
}

void GameController::specialKeyboardEvent(int key, int /* x */, int /* y */)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:  m_lastKeyHit = KEY_PRESS_LEFT;		break;
	case GLUT_KEY_RIGHT: m_lastKeyHit = KEY_PRESS_RIGHT;	break;
	case GLUT_KEY_UP:    m_lastKeyHit = KEY_PRESS_UP;		break;
	case GLUT_KEY_DOWN:  m_lastKeyHit = KEY_PRESS_DOWN;		break;
	default:             m_lastKeyHit = INVALID_KEY;		break;
	}
}

void GameController::playSound(int soundID)
{
	if (soundID == SOUND_NONE)
		return;

	SoundMapType::const_iterator p = m_soundMap.find(soundID);
	if (p != m_soundMap.end())
	{
		string path = m_gw->assetDirectory();
		if (!path.empty())
			path += '/';
		SoundFX().playClip(path + p->second);
	}
}

void GameController::doSomething()
{
	switch (m_gameState)
	{
	case not_applicable:
		break;
	case welcome:
		playSound(SOUND_THEME);
		m_mainMessage = "Welcome to RoboBlast!";
		m_secondMessage = "Press Enter to begin play...";
		setGameState(prompt);
		m_nextStateAfterPrompt = init;
		break;
	case contgame:
		m_mainMessage = "You lost a life!";
		m_secondMessage = "Press Enter to continue playing...";
		setGameState(prompt);
		m_nextStateAfterPrompt = cleanup;
		break;
	case finishedlevel:
		m_mainMessage = "Woot! You finished the level!";
		m_secondMessage = "Press Enter to continue playing...";
		setGameState(prompt);
		m_nextStateAfterPrompt = cleanup;
		break;
	case makemove:
		m_curIntraFrameTick = ANIMATION_POSITIONS_PER_TICK;
		m_nextStateAfterAnimate = not_applicable;
		{
			int status = m_gw->move();
			if (status == GWSTATUS_PLAYER_DIED)
			{
				// animate one last frame so the player can see what happened
				m_nextStateAfterAnimate = (m_gw->isGameOver() ? gameover : contgame);
			}
			else if (status == GWSTATUS_FINISHED_LEVEL)
			{
				m_gw->advanceToNextLevel();
				// animate one last frame so the player can see what happened
				m_nextStateAfterAnimate = finishedlevel;
			}
		}
		setGameState(animate);
		break;
	case animate:
		displayGamePlay(m_gw->getCurrentSubLevel());
		if (m_curIntraFrameTick-- <= 0)
		{
			if (m_nextStateAfterAnimate != not_applicable)
                setGameState(m_nextStateAfterAnimate);
			else
			{
				int key;
				if (!m_singleStep || getLastKey(key))
					setGameState(makemove);
			}
		}
		break;
	case cleanup:
		m_gw->cleanUp();
		setGameState(init);
		break;
	case gameover:
	{
		ostringstream oss;
		oss << (m_playerWon ? "You won the game!" : "Game Over!")
			<< " Final score: " << m_gw->getScore() << '!';
		m_mainMessage = oss.str();
	}
	m_secondMessage = "Press Enter to quit...";
	setGameState(prompt);
	m_nextStateAfterPrompt = quit;
	break;
	case prompt:
		drawPrompt(m_mainMessage, m_secondMessage);
		{
			int key;
			if (getLastKey(key) && key == '\r')
				setGameState(m_nextStateAfterPrompt);
		}
		break;
	case init:
	{
		int status = m_gw->init();
		//playSound(SOUND_BACKGROUND);
		SoundFX().abortClip();
		if (status == GWSTATUS_PLAYER_WON)
		{
			m_playerWon = true;
			setGameState(gameover);
		}
		else if (status == GWSTATUS_LEVEL_ERROR)
		{
			m_mainMessage = "Error in level data file encoding!";
			m_secondMessage = "Press Enter to quit...";
			setGameState(prompt);
			m_nextStateAfterPrompt = quit;
		}
		else {
			setGameState(makemove);
		}
	}
	break;
	case quit:
		glutLeaveMainLoop();
        break;
	}
}

void GameController::displayGamePlay(int sub_level)
{
	glEnable(GL_DEPTH_TEST); // must be done each time before displaying graphics or gets disabled for some reason
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);

	std::set<GraphObject*>& graphObjects = GraphObject::getGraphObjects(sub_level);
	for (auto it = graphObjects.begin(); it != graphObjects.end(); it++)
	{
		GraphObject* cur = *it;
		if (cur->isVisible())
		{
			cur->animate();

			double x, y, gx, gy, gz;
			cur->getAnimationLocation(x, y);
			convertToGlutCoords(x, y, gx, gy, gz);

			SpriteManager::Angle angle;
			switch (cur->getDirection())
			{
			case GraphObject::up:
				angle = SpriteManager::face_up;
				break;
			case GraphObject::down:
				angle = SpriteManager::face_down;
				break;
			case GraphObject::left:
				angle = SpriteManager::face_left;
				break;
			default:
			case GraphObject::right:
			case GraphObject::none:
				angle = SpriteManager::face_right;
				break;
			}

			int imageID = cur->getID();
			int frame = cur->getAnimationNumber() % m_spriteManager.getNumFrames(imageID);
			m_spriteManager.plotSprite(imageID, frame, gx, gy, gz, angle, 1.1);
		}
	}

	drawScoreAndLives(m_gameStatText);

	glutSwapBuffers();
}

void GameController::reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(45.0, double(WINDOW_WIDTH) / WINDOW_HEIGHT, PERSPECTIVE_NEAR_PLANE, PERSPECTIVE_FAR_PLANE);
	glMatrixMode (GL_MODELVIEW);
}

static void convertToGlutCoords(double x, double y, double& gx, double& gy, double& gz)
{
	x /= VIEW_WIDTH;
	y /= VIEW_HEIGHT;
	gx = 2 * VISIBLE_MIN_X + .3 + x * 2 * (VISIBLE_MAX_X - VISIBLE_MIN_X);
	gy = 2 * VISIBLE_MIN_Y +	  y * 2 * (VISIBLE_MAX_Y - VISIBLE_MIN_Y);
	gz = .6 * VISIBLE_MIN_Z;
}

static void doOutputStroke(double x, double y, double z, double size, const char* str, bool centered)
{
	if (centered)
	{
		double len = glutStrokeLength(GLUT_STROKE_ROMAN, reinterpret_cast<const unsigned char*>(str)) / FONT_SCALEDOWN;
		x = -len / 2;
		size = 1;
	}
	GLfloat scaledSize = static_cast<GLfloat>(size / FONT_SCALEDOWN);
	glPushMatrix();
	glLineWidth(1);
	glLoadIdentity();
	glTranslatef(static_cast<GLfloat>(x), static_cast<GLfloat>(y), static_cast<GLfloat>(z));
	glScalef(scaledSize, scaledSize, scaledSize);
	for ( ; *str != '\0'; str++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *str);
	glPopMatrix();
}

//static void outputStroke(double x, double y, double z, double size, const char* str)
//{
//	doOutputStroke(x, y, z, size, str, false);
//}

static void outputStrokeCentered(double y, double z, const char* str)
{
	doOutputStroke(0, y, z, 1, str, true);
}

static void drawPrompt(string mainMessage, string secondMessage)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
	glLoadIdentity ();
	outputStrokeCentered(1, -5, mainMessage.c_str());
	outputStrokeCentered(-1, -5, secondMessage.c_str());
	glutSwapBuffers();
}

static void drawScoreAndLives(string gameStatText)
{
	static int RATE = 1;
	static GLfloat rgb[3] =
		{ static_cast<GLfloat>(.6), static_cast<GLfloat>(.6), static_cast<GLfloat>(.6) };
	for (int k = 0; k < 3; k++)
	{
		double strength = rgb[k] + (-RATE + rand() % (2*RATE+1)) / 100.0;
		if (strength < .6)
			strength = .6;
		else if (strength > 1.0)
			strength = 1.0;
		rgb[k] = static_cast<GLfloat>(strength);
	}
	glColor3f(rgb[0], rgb[1], rgb[2]);
	outputStrokeCentered(SCORE_Y, SCORE_Z, gameStatText.c_str());
}
