#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <openssl/applink.c>

#include "OpenGG.h"

#include <chrono>

#include "OpenSSLFacade.h"

using namespace OpenGG;


void OpenGG::test()
{
	mpz_t a, exp1, exp2, mod;
	mpz_init(a);
	mpz_init(exp1);
	mpz_init(exp2);
	mpz_init(mod);
	OpenGG::readRSAKey("rsa4096.txt", &exp1, &mod, true);
	OpenGG::readRSAKey("rsa4096.txt", &exp2, &mod, false);
	char msg[] = "lmfao";
	char encr[512];
	char decr[32];
	size_t len, len2;
	mpz_import(a, 5, 1, 1, 1, 0, msg);
	mpz_powm(a, a, exp1, mod);
	mpz_export(encr, &len, 1, 1, 1, 0, a);
	std::cout << len << ":" << encr << std::endl;
	mpz_powm(a, a, exp2, mod);
	mpz_export(decr, &len2, 1, 1, 1, 0, a);
	std::cout << len2 << ":" << decr << std::endl;
}

void OpenGG::readRSAKey(std::string keyFilePath, mpz_t* exp, mpz_t* mod, bool pub)
{
	std::ifstream keyFile(keyFilePath + "priv", std::ios_base::binary);
	std::string primesFilePath = keyFilePath + "primes";
	char buffer[4096];
	std::string modulus, exponent;
	int read;

	if (!keyFile.good())
	{
		keyFile.close();
		throw std::exception("Invalid file path!");
	}
	keyFile.close();

	EVP_PKEY* key = OpenSSLFacade::readRSAPrivateKey(keyFilePath);
	EVP_PKEY_print_private_fp(fopen(primesFilePath.c_str(), "w"), key, 0, nullptr);

	keyFile.open(primesFilePath, std::ios_base::binary);
	keyFile.read(buffer, 4096);
	read = keyFile.gcount();

	// parse modulus
	bool started = false;
	int i = 0;
	while (true)
	{
		if (started)
		{
			if (buffer[i] == 'p')
				break;

			if (buffer[i] != ' ' && buffer[i] != ':' && buffer[i] != 10)
				modulus += buffer[i];
		}
		else
		{
			if (buffer[i] == 's' && buffer[i + 1] == ':')
				started = true;
		}
		++i;
	}

	// parse exponent
	i += 24;
	if (pub)
	{
		exponent = "010001";
	}
	else
	{
		started = false;
		while (true)
		{
			if (started)
			{
				if (buffer[i] == 'p')
					break;

				if (buffer[i] != ' ' && buffer[i] != ':' && buffer[i] != 10)
					exponent += buffer[i];
			}
			{
				if (buffer[i] == 't' && buffer[i + 1] == ':')
					started = true;
			}
			++i;
		}
	}

	mpz_set_str(*mod, modulus.c_str(), 16);
	mpz_set_str(*exp, exponent.c_str(), 16);

	std::remove(primesFilePath.c_str());
}

float OpenGG::asymmetricEncrDecr(std::string keyFilePath, std::string dataFilePath, bool encrypt)
{
	mpz_t mod, exp, data;
	std::string tempOutPath = dataFilePath + "tmp";
	std::ifstream input(dataFilePath, std::ios_base::binary);
	std::ofstream output(tempOutPath, std::ios_base::binary);
	char blockIn[1024], blockOut[1024];

	mpz_init(mod);
	mpz_init(exp);
	mpz_init(data);
	readRSAKey(keyFilePath, &exp, &mod, encrypt);

	int read;
	size_t outLen;
	auto start = std::chrono::high_resolution_clock::now();
	while (true)
	{
		input.read(blockIn, 512);
		read = input.gcount();

		if (read == 0)
			break;

		mpz_import(data, read, 1, 1, 1, 0, blockIn);
		mpz_powm(data, data, exp, mod);
		mpz_export(blockOut, &outLen, 1, 1, 1, 0, data);

		output.write(blockOut, outLen);
	}
	float duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000000.0;

	input.close();
	output.close();
	std::remove(dataFilePath.c_str());
	std::rename(tempOutPath.c_str(), dataFilePath.c_str());

	mpz_clear(exp);
	mpz_clear(mod);
	mpz_clear(data);

	return duration;
}