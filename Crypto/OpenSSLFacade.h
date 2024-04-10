#pragma once
#include <fstream>
#include <string>
#include <openssl/evp.h>
#include <iostream>
#include <string>
#include <memory>
#include <limits>
#include <vector>
#include <openssl/rand.h>


namespace OpenSSLFacade
{
	void genSymmetricKey(int size, std::string filePath = nullptr)
	{
		unsigned char *key = new unsigned char[size];
		unsigned char *iv = new unsigned char[size/2];

		RAND_bytes(key, size);
		RAND_bytes(iv, size / 2);

		BIO* b64 = BIO_new(BIO_f_base64());
		BIO* out = BIO_new_file(filePath.c_str(), "w");

		BIO_push(b64, out);

		BIO_write(b64, (char*)key, size);
		BIO_write(b64, "\n", 1);
		BIO_write(b64, (char*)iv, size/2);
		BIO_flush(b64);

		BIO_free_all(b64);
	}

	std::vector<std::string> readSymmetricKey(std::string filePath)
	{
		char line[128];

		BIO* b64 = BIO_new(BIO_f_base64());
		BIO* in = BIO_new_file(filePath.c_str(), "r");
		BIO_push(b64, in);

		unsigned long long read;
		BIO_read_ex(b64, line, 128, &read);
		BIO_free_all(b64);

		std::string keyStr(line, read);
		int delimiter = keyStr.find("\n");

		std::vector<std::string> keyComponents;
		keyComponents.push_back(keyStr.substr(0, delimiter));
		keyComponents.push_back(keyStr.substr(delimiter + 1));

		return keyComponents;
	}

	float symmetricEncrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		std::ifstream keyFile(keyFilePath);
		std::string tempOutPath = dataFilePath + "tmp";
		std::ifstream input(dataFilePath);
		std::ofstream output(tempOutPath);
		EVP_CIPHER_CTX* ctx;

		// check key file
		if (!keyFile.good())
			throw new std::exception((std::string("File not found: ") + keyFilePath).c_str());

		// read key and iv
		std::vector<std::string> keyComponents = readSymmetricKey(keyFilePath);
		unsigned char* key = new unsigned char[keyComponents[0].size()];
		unsigned char* iv = new unsigned char[keyComponents[1].size()];

		// cast to unsigned char
		key = (unsigned char*)keyComponents[0].c_str();
		iv = (unsigned char*)keyComponents[1].c_str();

		// create context and init encryption
		ctx = EVP_CIPHER_CTX_new();
		if(EVP_EncryptInit_ex(ctx, (EVP_CIPHER*)cipher, NULL, key, iv) != 1)
			throw new std::exception("Encryption init failed!");

		
		int inLen = 0, outLen;
		while(true)
		{
			unsigned char blockIn[1024], blockOut[4096];

			// read max 1024 bytes from file
			input.read((char*)blockIn, 8);
			inLen = input.gcount();

			// end of file
			if (inLen == 0)
				break;

			// encrypt
			if (1 != EVP_EncryptUpdate(ctx, blockOut, &outLen, blockIn, inLen))
				throw new std::exception("Encryption failed!");

			// write to temporary file
			output.write((char*)blockOut, outLen);
		}

		unsigned char blockIn[1024], blockOut[4096];

		// finish encryption
		if (1 != EVP_EncryptFinal_ex(ctx, blockOut + outLen, &outLen))
			throw new std::exception("Encryption finish failed");

		// write remaining data if there is any
		if(outLen != 0)
			output.write((char*)blockOut, outLen);

		// free context
		EVP_CIPHER_CTX_free(ctx);

		// close file streams
		input.close();
		output.close();

		// remove original file and rename temporary to original
		int a = std::remove(dataFilePath.c_str());
		int b = std::rename(tempOutPath.c_str(), dataFilePath.c_str());

		return 0;
	}

	float symmetricDecrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		std::ifstream keyFile(keyFilePath);
		std::string tempOutPath = dataFilePath + "tmp";
		std::ifstream input(dataFilePath);
		std::ofstream output(tempOutPath);
		EVP_CIPHER_CTX* ctx;

		// check key file
		if (!keyFile.good())
			throw new std::exception((std::string("File not found: ") + keyFilePath).c_str());

		// read key and iv
		std::vector<std::string> keyComponents = readSymmetricKey(keyFilePath);
		unsigned char* key = new unsigned char[keyComponents[0].size()];
		unsigned char* iv = new unsigned char[keyComponents[1].size()];

		// cast to unsigned char
		key = (unsigned char*)keyComponents[0].c_str();
		iv = (unsigned char*)keyComponents[1].c_str();

		// create context and init encryption
		ctx = EVP_CIPHER_CTX_new();
		if (EVP_DecryptInit_ex(ctx, (EVP_CIPHER*)cipher, NULL, key, iv) != 1)
			throw new std::exception("Decryption init failed!");

		unsigned char blockIn[1024], blockOut[4096];
		int inLen = 0, outLen;
		while (true)
		{
			// read max 1024 bytes from file
			input.read((char*)blockIn, 1024);
			inLen = input.gcount();

			// end of file
			if (inLen == 0)
				break;

			// encrypt
			if (1 != EVP_DecryptUpdate(ctx, blockOut, &outLen, blockIn, inLen))
				throw new std::exception("Encryption failed!");

			// write to temporary file
			output.write((char*)blockOut, outLen);
		}

		// finish encryption
		int offset = outLen;
		if (1 != EVP_DecryptFinal_ex(ctx, blockOut + outLen, &outLen))
			throw new std::exception("Encryption finish failed");

		// write remaining data if any
		if(outLen != 0)
			output.write((char*)blockOut + offset, outLen);

		// free context
		EVP_CIPHER_CTX_free(ctx);

		// close file streams
		input.close();
		output.close();

		// remove original file and rename temporary to original
		int a = std::remove(dataFilePath.c_str());
		int b = std::rename(tempOutPath.c_str(), dataFilePath.c_str());

		return 0;
	}
};
