#pragma once
#include "Key.h"
#include "File.h"


class Repository
{
public:
	virtual void saveKey(Key key) = 0;

	virtual Key getKey() = 0;

	virtual void saveFile(File file) = 0;

	virtual File getFile() = 0;
};

