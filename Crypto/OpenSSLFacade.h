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
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/applink.c>


namespace OpenSSLFacade
{
	void genSymmetricKey(void* cipher, std::string filePath = nullptr)
	{
		int keyLen = EVP_CIPHER_key_length((EVP_CIPHER*)cipher);
		int ivLen = EVP_CIPHER_get_iv_length((EVP_CIPHER*)cipher);

		unsigned char *key = new unsigned char[keyLen];
		unsigned char *iv = new unsigned char[ivLen];

		RAND_bytes(key, keyLen);
		RAND_bytes(iv, ivLen);

		BIO* b64 = BIO_new(BIO_f_base64());
		BIO* out = BIO_new_file(filePath.c_str(), "w");

		BIO_push(b64, out);

		BIO_write(b64, key, keyLen);
		BIO_write(b64, "\n", 1);
		BIO_write(b64, iv, ivLen);
		BIO_flush(b64);

		BIO_free_all(b64);
		delete[]key;
		delete[]iv;
	}

	void saveSeal(std::string filePath, unsigned char* key, int keyLen, unsigned char *iv, int ivLen)
	{
		BIO* b64 = BIO_new(BIO_f_base64());
		BIO* out = BIO_new_file((filePath + "seal").c_str(), "w");

		BIO_push(b64, out);

		BIO_write(b64, (char*)key, keyLen);
		BIO_write(b64, "\n", 1);
		BIO_write(b64, (char*)iv, ivLen);

		BIO_flush(b64);
		BIO_free_all(b64);
	}

	void readSeal(std::string filePath, unsigned char* key, int* keyLen, unsigned char* iv, int* ivLen)
	{
		char line[1024];

		BIO* b64 = BIO_new(BIO_f_base64());
		BIO* in = BIO_new_file((filePath + "seal").c_str(), "r");
		BIO_push(b64, in);

		unsigned long long read;
		BIO_read_ex(b64, line, 1024, &read);
		BIO_free_all(b64);

		std::string keyStr(line, read);
		*keyLen = (read - 1) / 3 * 2;
		*ivLen = *keyLen / 2;

		// write key
		memcpy_s(key, *keyLen, line, *keyLen);
		memcpy_s(iv, *ivLen, line + *keyLen + 1, *ivLen);
	}

	void readSymmetricKey(std::string filePath, char* key, char* iv)
	{
		char line[128];

		BIO* b64 = BIO_new(BIO_f_base64());
		BIO* in = BIO_new_file(filePath.c_str(), "r");
		BIO_push(b64, in);

		unsigned long long read;
		BIO_read_ex(b64, line, 128, &read);
		BIO_free_all(b64);

		int keySize = (read - 1) / 3 * 2;
		memcpy_s(key, keySize, line, keySize);
		memcpy_s(iv, keySize / 2, line + keySize + 1, keySize / 2);
	}

	float symmetricEncrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		std::string tempOutPath = dataFilePath + "tmp";
		std::ifstream keyFile(keyFilePath);
		std::ifstream input(dataFilePath, std::ios_base::binary);
		std::ofstream output(tempOutPath, std::ios_base::binary);
		EVP_CIPHER_CTX* ctx;
		int keyLen = EVP_CIPHER_key_length((EVP_CIPHER*)cipher);
		int ivLen = EVP_CIPHER_get_iv_length((EVP_CIPHER*)cipher);
		unsigned char* key = new unsigned char[keyLen];
		unsigned char* iv = new unsigned char[ivLen];
		unsigned char blockIn[1024], blockOut[1024 + EVP_MAX_BLOCK_LENGTH];

		// check key file
		if (!keyFile.good())
			throw new std::exception((std::string("File not found: ") + keyFilePath).c_str());
		keyFile.close();

		// read key and iv
		readSymmetricKey(keyFilePath, (char*)key, (char*)iv);

		// create context and init encryption
		ctx = EVP_CIPHER_CTX_new();
		if(EVP_EncryptInit(ctx, (EVP_CIPHER*)cipher, key, iv) != 1)
			throw new std::exception("Encryption init failed!");

		// read - encrypt - write blocks of 1024 bytes until end of file
		int inLen = 0, outLen;
		while(true)
		{
			// read max 1024 bytes from file
			input.read((char*)blockIn, 1024);
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

		// finish encryption
		int offset = outLen;
		if (1 != EVP_EncryptFinal(ctx, blockOut + outLen, &outLen))
			throw new std::exception("Encryption finish failed");

		// write remaining data
		output.write((char*)blockOut + offset, outLen);

		// close file streams
		input.close();
		output.close();

		// remove original file and rename temporary to original
		int a = std::remove(dataFilePath.c_str());
		int b = std::rename(tempOutPath.c_str(), dataFilePath.c_str());

		// free everything
		EVP_CIPHER_CTX_free(ctx);
		delete[]key;
		delete[]iv;

		return 0;
	}

	float symmetricDecrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		std::ifstream keyFile(keyFilePath);
		std::string tempOutPath = dataFilePath + "tmp";
		EVP_CIPHER_CTX* ctx;
		int keyLen = EVP_CIPHER_key_length((EVP_CIPHER*)cipher);
		int ivLen = EVP_CIPHER_get_iv_length((EVP_CIPHER*)cipher);

		// check key file
		if (!keyFile.good())
			throw new std::exception((std::string("File not found: ") + keyFilePath).c_str());

		// read key and iv
		unsigned char* key = new unsigned char[keyLen];
		unsigned char* iv = new unsigned char[ivLen];
		readSymmetricKey(keyFilePath, (char*)key, (char*)iv);

		// create context and init encryption
		ctx = EVP_CIPHER_CTX_new();
		if (EVP_DecryptInit(ctx, (EVP_CIPHER*)cipher, key, iv) != 1)
			throw new std::exception("Decryption init failed!");

		// read - decrypt - write blocks of 1024 bytes until end of file
		std::ifstream input(dataFilePath, std::ios_base::binary);
		std::ofstream output(tempOutPath, std::ios_base::binary);
		unsigned char blockIn[1024], blockOut[1024 + EVP_MAX_BLOCK_LENGTH];
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
		if (1 != EVP_DecryptFinal(ctx, blockOut + outLen, &outLen))
			throw new std::exception("Encryption finish failed");

		// write remaining data
		output.write((char*)blockOut + offset, outLen);

		// close file streams
		input.close();
		output.close();

		// remove original file and rename temporary to original
		int a = std::remove(dataFilePath.c_str());
		int b = std::rename(tempOutPath.c_str(), dataFilePath.c_str());

		// free everything
		EVP_CIPHER_CTX_free(ctx);
		delete[]key;
		delete[]iv;

		return 0;
	}

	void genRSAKey(int size, std::string filePath)
	{
		EVP_PKEY* key = nullptr;
		EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);

		if (!ctx)
			throw std::exception("Key initialization failed!");

		if(EVP_PKEY_keygen_init(ctx) <= 0)
			throw std::exception("Key initialization failed!");

		if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, size) <= 0)
			throw std::exception("Key initialization failed!");

		if (EVP_PKEY_keygen(ctx, &key) <= 0)
			throw std::exception("Key generation failed!");

		EVP_PKEY_CTX_free(ctx);

		// write public key
		BIO* out = BIO_new_file((filePath + "pub").c_str(), "w");
		int err = PEM_write_bio_PUBKEY(out, key);
		BIO_flush(out);
		BIO_free_all(out);

		// write private key
		out = BIO_new_file((filePath + "priv").c_str(), "w");
		err &= PEM_write_bio_PrivateKey(out, key, nullptr, nullptr, 0, 0, nullptr);
		BIO_flush(out);
		BIO_free_all(out);

		if (err == 0)
			throw std::exception("Invalid file");
	}

	EVP_PKEY* readRSAPublicKey(std::string filePath)
	{
		EVP_PKEY* key = nullptr;

		BIO *in = BIO_new_file((filePath + "pub").c_str(), "r");
		key = PEM_read_bio_PUBKEY(in, nullptr, 0, nullptr);
		BIO_free_all(in);

		return key;
	}

	EVP_PKEY* readRSAPrivateKey(std::string filePath)
	{
		EVP_PKEY* key = nullptr;

		BIO *in = BIO_new_file((filePath + "priv").c_str(), "r");
		key = PEM_read_bio_PrivateKey(in, nullptr, 0, nullptr);
		BIO_free_all(in);

		return key;
	}

	float asymmetricEncrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		EVP_CIPHER_CTX* ctx;
		EVP_PKEY* key = readRSAPublicKey(keyFilePath);
		int keyLen = EVP_PKEY_size(key);

		// init
		if (!(ctx = EVP_CIPHER_CTX_new()))
			throw std::exception("Encryption failed!");

		unsigned char **encryptedKey = new unsigned char*[1];
		encryptedKey[0] = new unsigned char[keyLen];
		unsigned char *iv = new unsigned char[16];
		int ekLen;

		// init encryption
		if (1 != EVP_SealInit(ctx, EVP_aes_256_cbc(), encryptedKey, &ekLen, iv, &key, 1))
			throw std::exception("Encryption failed!");

		// save seal
		saveSeal(keyFilePath, encryptedKey[0], ekLen, iv, 16);

		// open files in binary mode
		std::string tempOutPath = dataFilePath + "tmp";
		std::ifstream input(dataFilePath, std::ios_base::binary);
		std::ofstream output(tempOutPath, std::ios_base::binary);
		unsigned char *blockIn = new unsigned char[keyLen], *blockOut = new unsigned char[keyLen];
		int inLen = 0, outLen;

		// read - encrypt - write blocks of 16 bytes until end of file
		while(true)
		{
			// read max 16 bytes from file
			input.read((char*)blockIn, 16);
			inLen = input.gcount();

			// end of file
			if (inLen == 0)
				break;

			// encrypt
			if (1 != EVP_SealUpdate(ctx, blockOut, &outLen, blockIn, inLen))
				throw std::exception("Encryption failed!");

			// write to temporary file
			output.write((char*)blockOut, outLen);
		}

		// finish encryption
		int offset = outLen;
		if (1 != EVP_SealFinal(ctx, blockOut + outLen, &outLen))
			throw new std::exception("Encryption failed");

		// write remaining data
		output.write((char*)blockOut + offset, outLen);

		// close file streams
		input.close();
		output.close();

		// remove original file and rename temporary to original
		int a = std::remove(dataFilePath.c_str());
		int b = std::rename(tempOutPath.c_str(), dataFilePath.c_str());

		// free everything
		EVP_CIPHER_CTX_free(ctx);
		EVP_PKEY_free(key);
		delete[]encryptedKey[0];
		delete[]encryptedKey;
		delete[]iv;
		delete[]blockIn;
		delete[]blockOut;

		return 0;
	}

	float asymmetricDecrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		EVP_CIPHER_CTX* ctx;
		EVP_PKEY* key = readRSAPrivateKey(keyFilePath);
		int keyLen = EVP_PKEY_bits(key) / 8;

		// init
		if (!(ctx = EVP_CIPHER_CTX_new()))
			throw std::exception("Decryption failed!");

		// read envelope seal (aes 256 cbc key)
		unsigned char *encryptedKey = new unsigned char[1024];
		unsigned char *iv = new unsigned char[16];
		int ekLen, ivLen;
		readSeal(keyFilePath, encryptedKey, &ekLen, iv, &ivLen);

		// init decryption
		if (0 == EVP_OpenInit(ctx, EVP_aes_256_cbc(), encryptedKey, ekLen, iv, key))
			throw std::exception("Decryption failed!");

		// open file streams in binary mode
		std::string tempOutPath = dataFilePath + "tmp";
		std::ifstream input(dataFilePath, std::ios_base::binary);
		std::ofstream output(tempOutPath, std::ios_base::binary);
		unsigned char *blockIn = new unsigned char[keyLen], *blockOut = new unsigned char[keyLen];
		int inLen = 0, outLen;

		// read - decrypt - write blocks of 16 bytes until end of file
		while(true)
		{
			// read max 16 bytes from file
			input.read((char*)blockIn, 16);
			inLen = input.gcount();

			// end of file
			if (inLen == 0)
				break;

			// encrypt
			if (1 != EVP_OpenUpdate(ctx, blockOut, &outLen, blockIn, inLen))
				throw std::exception("Decryption failed!");

			// write to temporary file
			output.write((char*)blockOut, outLen);
		}

		// finish decryption
		int offset = outLen;
		if (1 != EVP_OpenFinal(ctx, blockOut + outLen, &outLen))
			throw exception("Decryption failed!");

		// write remaining data
		output.write((char*)blockOut + offset, outLen);

		// close file streams
		input.close();
		output.close();

		// remove original file and rename temporary to original
		int a = std::remove(dataFilePath.c_str());
		int b = std::rename(tempOutPath.c_str(), dataFilePath.c_str());

		// free everything
		EVP_CIPHER_CTX_free(ctx);
		EVP_PKEY_free(key);
		delete[]encryptedKey;
		delete[]iv;
		delete[]blockIn;
		delete[]blockOut;

		return 0;
	}
};
