#pragma once
#include <string>


enum CRYPTO_OP
{
	GEN_KEY,
	ENCRYPT,
	DECRYPT
};

class Message
{
public:
	CRYPTO_OP operation;
	LIB library;
	ALG algorithm;
	std::string keyFilePath, dataFilePath, message, hash;
	float time = 0.0f;

	/**
	 * \brief Constructor for request message
	 * \param op operation requested
	 * \param lib library used
	 * \param alg algorithm used
	 * \param keyPath path to key file
	 * \param dataPath path to data file (for encryption/decryption, this can be omitted if the operation is GEN_KEY)
	 * \param size size of the key in case of RSA
	 */
	Message(CRYPTO_OP op, LIB lib, ALG alg, std::string keyPath, std::string dataPath = "", float size = 0.0f)
	{
		operation = op;
		library = lib;
		algorithm = alg;
		keyFilePath = keyPath;
		dataFilePath = dataPath;
		time = size;
	}

	/**
	 * \brief Add info to message to pass it as a response 
	 * \param msg response message ("" - success, either - error)
	 * \param t duration of encryption/decryption in seconds
	 * \param h hash value of the first block in file
	 */
	void setResponse(std::string msg, float t = 0.0f, std::string h = "")
	{
		message = msg;
		time = t;
		hash = h;
	}
};
