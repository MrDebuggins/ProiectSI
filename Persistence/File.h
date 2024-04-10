#pragma once
#include "Key.h"


enum STATUS
{
	ENCRYPTED,
	DECRYPTED,
	ENCRYPTING,
	DECRYPTING
};

class File
{
public:
	string path;
	Key* key;

	STATUS status;
	float minEncryptTime;
	float minDecryptTime;

	File(string path, Key *key = nullptr, STATUS status = DECRYPTED, float encrTyme = 0.0f, float decrTime = 0.0f);
};