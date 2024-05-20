#pragma once
#include <string>


namespace OpenSSLFacade
{
	void genSymmetricKey(void* cipher, std::string filePath);

	void readSymmetricKey(std::string filePath, char* key, char* iv);

	float symmetricEncrypt(std::string keyFilePath, std::string dataFilePath, void* cipher);

	float symmetricDecrypt(std::string keyFilePath, std::string dataFilePath, void* cipher);

	void genRSAKey(int size, std::string filePath);

	void saveSeal(std::string filePath, unsigned char* key, int keyLen, unsigned char* iv);

	void readSeal(std::string filePath, unsigned char* key, int* keyLen, unsigned char* iv);

	EVP_PKEY* readRSAPublicKey(std::string filePath);

	EVP_PKEY* readRSAPrivateKey(std::string filePath);

	float asymmetricEncrypt(std::string keyFilePath, std::string dataFilePath);

	float asymmetricDecrypt(std::string keyFilePath, std::string dataFilePath);

	std::string hash(std::string filePath);
};
