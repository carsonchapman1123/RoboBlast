#include "GameController.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

// If your program is having trouble finding the Assets directory,
// replace the string literal with a full path name to the directory,
// e.g., "Z:/CS32/RoboBlast/Assets" or "/Users/fred/cs32/RoboBlast/Assets"

const string assetDirectory = "/Users/macuser/Desktop/RoboBlast/DerivedData/RoboBlast/Build/Products/Debug/Assets";

class GameWorld;

GameWorld* createStudentWorld(string assetDir = "");

int main(int argc, char* argv[])
{
	{
		string path = assetDirectory;
		if (!path.empty())
			path += '/';
		ifstream ifs(path + "level00.dat");
		if (!ifs)
		{
			cout << "Cannot find level00.dat in ";
			cout << (assetDirectory.empty() ? "current directory"
				: assetDirectory) << endl;
			return 1;
		}
	}
    
    
	srand(static_cast<unsigned int>(time(nullptr)));
    
	GameWorld* gw = createStudentWorld(assetDirectory);
	Game().run(argc, argv, gw, "RoboBlast");
}
