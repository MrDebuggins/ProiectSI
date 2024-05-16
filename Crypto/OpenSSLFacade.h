#pragma once
#include <chrono>
#include <time.h>
#include <fstream>
#include <string>
#include <openssl/evp.h>
#include <string>
#include <vector>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/applink.c>


namespace OpenSSLFacade
{
	inline void genSymmetricKey(void* cipher, std::string filePath = nullptr)
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

	inline void readSymmetricKey(std::string filePath, char* key, char* iv)
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

	inline float symmetricEncrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		std::string tempOutPath = dataFilePath + "tmp";							// temporary output file path
		std::ifstream keyFile(keyFilePath);										// stream to check if key file exists
		std::ifstream input(dataFilePath, std::ios_base::binary);			// input data file stream
		std::ofstream output(tempOutPath, std::ios_base::binary);			// temp output file stream
		EVP_CIPHER_CTX* ctx;													// OpenSSL context
		int keyLen = EVP_CIPHER_key_length((EVP_CIPHER*)cipher);				// key length
		int ivLen = EVP_CIPHER_get_iv_length((EVP_CIPHER*)cipher);				// IV length
		unsigned char* key = new unsigned char[keyLen];							// key data
		unsigned char* iv = new unsigned char[ivLen];							// IV data
		unsigned char blockIn[1024], blockOut[1024 + EVP_MAX_BLOCK_LENGTH];		// in buffer - original data, out buffer - encrypted data
		auto freeAll = [&]()												// lambda function to free allocated memory in case of an exception
			{
				// close file streams
				input.close();
				output.close();

				// free everything
				EVP_CIPHER_CTX_free(ctx);
				delete[]key;
				delete[]iv;
			};

		// check key file
		if (!keyFile.good())
		{
			keyFile.close();
			freeAll();
			throw std::exception((std::string("File not found: ") + keyFilePath).c_str());
		}
		keyFile.close();

		// read key and iv
		readSymmetricKey(keyFilePath, (char*)key, (char*)iv);

		// create context and init encryption
		ctx = EVP_CIPHER_CTX_new();
		if(EVP_EncryptInit(ctx, (EVP_CIPHER*)cipher, key, iv) != 1)
		{
			freeAll();
			throw std::exception("Encryption init failed!");
		}

		// read - encrypt - write blocks of 1024 bytes until end of file
		auto start = chrono::high_resolution_clock::now();
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
			{
				freeAll();
				throw std::exception("Encryption failed!");
			}

			// write to temporary file
			output.write((char*)blockOut, outLen);
		}

		// finish encryption
		int offset = outLen;
		if (1 != EVP_EncryptFinal(ctx, blockOut + outLen, &outLen))
		{
			freeAll();
			throw std::exception("Encryption finish failed");
		}

		// get duration
		float duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() / 1000000.0;

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

		return duration;
	}

	inline float symmetricDecrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		std::string tempOutPath = dataFilePath + "tmp";							// temporary output file path
		std::ifstream keyFile(keyFilePath);										// stream to check if key file exists
		std::ifstream input(dataFilePath, std::ios_base::binary);			// input data file stream
		std::ofstream output(tempOutPath, std::ios_base::binary);			// temp output file stream
		EVP_CIPHER_CTX* ctx;													// OpenSSL context
		int keyLen = EVP_CIPHER_key_length((EVP_CIPHER*)cipher);				// key length
		int ivLen = EVP_CIPHER_get_iv_length((EVP_CIPHER*)cipher);				// IV length
		unsigned char* key = new unsigned char[keyLen];							// key data
		unsigned char* iv = new unsigned char[ivLen];							// IV data
		unsigned char blockIn[1024], blockOut[1024 + EVP_MAX_BLOCK_LENGTH];		// in buffer - original data, out buffer - encrypted data
		auto freeAll = [&]()												// lambda function to free allocated memory in case of an exception
			{
				// close file streams
				input.close();
				output.close();

				// free everything
				EVP_CIPHER_CTX_free(ctx);
				delete[]key;
				delete[]iv;
			};

		// check key file
		if (!keyFile.good())
		{
			keyFile.close();
			freeAll();
			throw std::exception((std::string("File not found: ") + keyFilePath).c_str());
		}

		// read key and iv
		readSymmetricKey(keyFilePath, (char*)key, (char*)iv);

		// create context and init encryption
		ctx = EVP_CIPHER_CTX_new();
		if (EVP_DecryptInit(ctx, (EVP_CIPHER*)cipher, key, iv) != 1)
		{
			freeAll();
			throw std::exception("Decryption init failed!");
		}

		// read - decrypt - write blocks of 1024 bytes until end of file
		auto start = chrono::high_resolution_clock::now();
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
			{
				freeAll();
				throw std::exception("Decryption failed!");
			}

			// write to temporary file
			output.write((char*)blockOut, outLen);
		}

		// finish encryption
		int offset = outLen;
		if (1 != EVP_DecryptFinal(ctx, blockOut + outLen, &outLen))
		{
			freeAll();
			throw std::exception("Decryption finish failed");
		}

		//get duration
		float duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() / 1000000.0;

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

		return duration;
	}

	inline void genRSAKey(int size, std::string filePath)
	{
		EVP_PKEY* key = nullptr;
		EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
		BIO* out = BIO_new_file((filePath + "pub").c_str(), "w");
		auto freeAll = [&]()
			{
				EVP_PKEY_CTX_free(ctx);
				BIO_free_all(out);
			};

		if (!ctx)
		{
			freeAll();
			throw std::exception("Key initialization failed!");
		}

		if(EVP_PKEY_keygen_init(ctx) <= 0)
		{
			freeAll();
			throw std::exception("Key initialization failed!");
		}

		if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, size) <= 0)
		{
			freeAll();
			throw std::exception("Key initialization failed!");
		}

		if (EVP_PKEY_keygen(ctx, &key) <= 0)
		{
			freeAll();
			throw std::exception("Key generation failed!");
		}

		// write public key
		if (!PEM_write_bio_PUBKEY(out, key))
		{
			freeAll();
			throw std::exception("Key initialization failed!");
		}
		BIO_flush(out);
		BIO_free_all(out);

		// write private key
		out = BIO_new_file((filePath + "priv").c_str(), "w");
		if(!PEM_write_bio_PrivateKey(out, key, nullptr, nullptr, 0, 0, nullptr))
		{
			freeAll();
			throw std::exception("Key initialization failed!");
		}
		BIO_flush(out);

		freeAll();
	}

	inline void saveSeal(std::string filePath, unsigned char* key, int keyLen, unsigned char* iv)
	{
		BIO* b64 = BIO_new(BIO_f_base64());
		BIO* out = BIO_new_file((filePath + "seal").c_str(), "w");

		BIO_push(b64, out);

		BIO_write(b64, (char*)key, keyLen);
		BIO_write(b64, (char*)iv, 16);

		BIO_flush(b64);
		BIO_free_all(b64);
	}

	inline void readSeal(std::string filePath, unsigned char* key, int* keyLen, unsigned char* iv)
	{
		char line[1024];

		BIO* b64 = BIO_new(BIO_f_base64());
		BIO* in = BIO_new_file((filePath + "seal").c_str(), "r");
		BIO_push(b64, in);

		unsigned long long read;
		BIO_read_ex(b64, line, 1024, &read);
		BIO_free_all(b64);

		std::string keyStr(line, read);
		*keyLen = read - 16;

		// write key
		memcpy_s(key, *keyLen, line, *keyLen);
		memcpy_s(iv, 16, line + *keyLen, 16);
	}

	inline EVP_PKEY* readRSAPublicKey(std::string filePath)
	{
		EVP_PKEY* key = nullptr;

		BIO *in = BIO_new_file((filePath + "pub").c_str(), "r");
		key = PEM_read_bio_PUBKEY(in, nullptr, 0, nullptr);
		BIO_free_all(in);

		return key;
	}

	inline EVP_PKEY* readRSAPrivateKey(std::string filePath)
	{
		EVP_PKEY* key = nullptr;

		BIO *in = BIO_new_file((filePath + "priv").c_str(), "r");
		key = PEM_read_bio_PrivateKey(in, nullptr, 0, nullptr);
		BIO_free_all(in);

		return key;
	}

	inline float asymmetricEncrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		std::string tempOutPath = dataFilePath + "tmp";												// output stream file path
		std::ifstream input(dataFilePath, std::ios_base::binary);								// input stream
		std::ofstream output(tempOutPath, std::ios_base::binary);								// output stream
		EVP_CIPHER_CTX* ctx;																		// OpenSSL context
		EVP_PKEY* key = readRSAPublicKey(keyFilePath);											// OpenSSL rsa key
		int keyLen = EVP_PKEY_size(key);															// rsa key length
		unsigned char *blockIn = new unsigned char[keyLen], *blockOut = new unsigned char[keyLen];	// original data buffer, encrypted data buffer
		unsigned char** encryptedKey = new unsigned char* [1];										// array of aes private keys
		encryptedKey[0] = new unsigned char[keyLen];												// we use only one symmetric key
		unsigned char* iv = new unsigned char[32];													// IV for the key above
		auto freeAll = [&]()																	// lambda to free memory on exceptions
			{
				// close file streams
				input.close();
				output.close();

				// free everything else
				EVP_CIPHER_CTX_free(ctx);
				EVP_PKEY_free(key);
				delete[]blockIn;
				delete[]blockOut;
				delete[]encryptedKey[0];
			};

		// init
		if (!(ctx = EVP_CIPHER_CTX_new()))
		{
			freeAll();
			throw std::exception("Context init failed!");
		}

		// init encryption
		int ekLen;
		if (1 != EVP_SealInit(ctx, EVP_aes_256_cbc(), encryptedKey, &ekLen, iv, &key, 1))
		{
			freeAll();
			throw std::exception("Encryption init failed!");
		}

		// save seal
		saveSeal(keyFilePath, encryptedKey[0], ekLen, iv);

		// read - encrypt - write blocks of 16 bytes until end of file
		auto start = chrono::high_resolution_clock::now();
		int inLen = 0, outLen = 0;
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
			{
				freeAll();
				throw std::exception("Encryption failed!");
			}

			// write to temporary file
			output.write((char*)blockOut, outLen);
		}

		// finish encryption
		int offset = outLen;
		if (1 != EVP_SealFinal(ctx, blockOut + outLen, &outLen))
		{
			freeAll();
			throw new std::exception("Encryption finish failed");
		}

		// get duration
		float duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() / 1000000.0;

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
		delete[]blockIn;
		delete[]blockOut;
		delete[]encryptedKey[0];

		return duration;
	}

	inline float asymmetricDecrypt(std::string keyFilePath, std::string dataFilePath, void* cipher)
	{
		std::string tempOutPath = dataFilePath + "tmp";												// output stream file path
		std::ifstream input(dataFilePath, std::ios_base::binary);								// input stream
		std::ofstream output(tempOutPath, std::ios_base::binary);								// output stream
		EVP_CIPHER_CTX* ctx;																		// OpenSSL context
		EVP_PKEY* key = readRSAPrivateKey(keyFilePath);											// rsa key
		int keyLen = EVP_PKEY_bits(key) / 8;														// rsa key length
		unsigned char* blockIn = new unsigned char[keyLen], * blockOut = new unsigned char[keyLen];	// encrypted data buffer, decrypted data buffer
		unsigned char* encryptedKey = new unsigned char[keyLen];									// aes private key
		unsigned char* iv = new unsigned char[16];													// aes IV
		auto freeAll = [&]()																	// lambda to free memory on exceptions
			{
				// close file streams
				input.close();
				output.close();

				// free everything else
				EVP_CIPHER_CTX_free(ctx);
				EVP_PKEY_free(key);
				delete[]blockIn;
				delete[]blockOut;
				delete[]encryptedKey;
			};

		// init
		if (!(ctx = EVP_CIPHER_CTX_new()))
		{
			freeAll();
			throw std::exception("Context init failed!");
		}

		// read envelope seal (aes 256 cbc key)
		int ekLen;
		readSeal(keyFilePath, encryptedKey, &ekLen, iv);

		// init decryption
		if (0 == EVP_OpenInit(ctx, EVP_aes_256_cbc(), encryptedKey, ekLen, iv, key))
		{
			freeAll();
			throw std::exception("Decryption init failed!");
		}

		// read - decrypt - write blocks of 16 bytes until end of file
		auto start = chrono::high_resolution_clock::now();
		int inLen = 0, outLen = 0;
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
			{
				freeAll();
				throw std::exception("Decryption failed!");
			}

			// write to temporary file
			output.write((char*)blockOut, outLen);
		}

		// finish decryption
		int offset = outLen;
		if (1 != EVP_OpenFinal(ctx, blockOut + outLen, &outLen))
		{
			freeAll();
			throw std::exception("Decryption finish failed!");
		}

		// get duration
		float duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count() / 1000000.0;

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
		delete[]blockIn;
		delete[]blockOut;
		delete[]encryptedKey;

		return duration;
	}
};
