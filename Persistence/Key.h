#pragma once
#include <KEY.h>
#include <string>
#include "ALG.h"


using namespace std;

/**
 * \brief Key proxy used by persistence layer. Data is loaded only if neededs
 */
class Key
{
public:
	string filePath;
	KEY type;
	ALG algorithm;

	string data = "";

	Key(string filePath, KEY type, ALG algorithm);
};

