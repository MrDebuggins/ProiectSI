#pragma once
#include <queue>
#include <string>
#include <unordered_map>
#include <openssl/evp.h>
#include <openssl/types.h>

#include "LIB.h"
#include "ALG.h"
#include "Message.h"


class Controller
{
	bool running = true;

	std::vector<ALG> symmetrycAlgs = { ALG_AES_256_CBC, ALG_AES_128_CBC };
	std::vector<ALG> asymmetricAlgs = { ALG_A_RSA };

	std::vector<ALG> algsOpenSSL = { ALG_AES_256_CBC, ALG_AES_128_CBC, ALG_A_RSA };
	std::vector<ALG> algsOpenGG = { ALG_AES_256_CBC, ALG_A_RSA };

	std::unordered_map<ALG, const EVP_CIPHER* (*)()> cipherOpenSSL;

	std::queue<Message>* in;
	std::queue<Message>* out;

	void processMessage(Message msg);

	bool isSymmetric(ALG alg);

public:
	Controller(std::queue<Message>* in, std::queue<Message>* out);

	void start();
};
