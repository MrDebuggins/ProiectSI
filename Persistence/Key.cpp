#include "Key.h"


Key::Key(string filePath, KEY type, ALG algorithm)
{
	this->filePath = filePath;
	this->type = type;
	this->algorithm = algorithm;
}