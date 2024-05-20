#pragma once
#include <queue>
#include <unordered_map>
#include <openssl/evp.h>

#include "Message.h"


class Controller
{
	bool running = true;

	std::vector<ALG> symmetrycAlgs = { ALG_AES_256_CBC, ALG_AES_128_CBC };
	std::vector<ALG> asymmetricAlgs = { ALG_RSA_4096, ALG_RSA_2048, ALG_RSA_2048 };

	std::vector<ALG> algsOpenSSL = { ALG_AES_256_CBC, ALG_AES_128_CBC, ALG_RSA_4096, ALG_RSA_2048, ALG_RSA_1024 };
	std::vector<ALG> algsOpenGG = { ALG_AES_256_CBC, ALG_RSA_4096 };

	std::unordered_map<ALG, const EVP_CIPHER* (*)()> cipherOpenSSL;

	std::queue<Message>* in;
	std::queue<Message>* out;

	void processMessage(Message msg);

	bool isSymmetric(ALG alg);

	int rsaSize(ALG alg);

public:
	Controller(std::queue<Message>* in, std::queue<Message>* out);

	void start();

	std::vector<ALG> getOpenSSLAlgs();

	std::vector<ALG> getOpenggAlgs();
};
