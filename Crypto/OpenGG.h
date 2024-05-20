#pragma once
#include "mini-gmp.h"


namespace OpenGG
{
	void test();

	void readRSAKey(std::string keyFilePath, mpz_t* exp, mpz_t* mod, bool pub);

	float asymmetricEncrDecr(std::string keyFilePath, std::string dataFilePath, bool encrypt);
}
