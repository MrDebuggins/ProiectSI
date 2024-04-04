#include "File.h"


File::File(string path, Key *key, STATUS status, float encrTyme, float decrTime)
{
	this->path = path;
	this->key = key;
	this->status = status;
	minEncryptTime = encrTyme;
	minDecryptTime = decrTime;
}
