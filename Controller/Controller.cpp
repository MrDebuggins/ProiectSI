#include "Controller.h"
#include "OpenSSLFacade.h"
#include "OpenGG.h"


Controller::Controller(std::queue<Message>* in, std::queue<Message>* out)
{
	this->in = in;
	this->out = out;

	cipherOpenSSL = 
	{
		{ALG_AES_256_CBC, EVP_aes_256_cbc},
		{ALG_AES_128_CBC, EVP_aes_128_cbc}
	};
}

bool Controller::isSymmetric(ALG alg)
{
	for(int i = 0; i < symmetrycAlgs.size(); ++i)
	{
		if (alg == symmetrycAlgs[i])
			return true;
	}

	return false;
}

int Controller::rsaSize(ALG alg)
{
	switch (alg)
	{
	case ALG_RSA_4096:
		return 4096;
	case ALG_RSA_2048:
		return 2048;
	case ALG_RSA_1024:
		return 1024;
	}
}

std::vector<ALG> Controller::getOpenSSLAlgs()
{
	return algsOpenSSL;
}

std::vector<ALG> Controller::getOpenggAlgs()
{
	return algsOpenGG;
}

void Controller::processMessage(Message msg)
{
	switch (msg.operation)
	{
	case GEN_KEY:
		try
		{
			if(isSymmetric(msg.algorithm))
				OpenSSLFacade::genSymmetricKey(cipherOpenSSL[msg.algorithm], msg.keyFilePath);
			else
				OpenSSLFacade::genRSAKey(rsaSize(msg.algorithm), msg.keyFilePath);
		}
		catch (std::exception ex)
		{
			msg.setResponse(ex.what());
		}

		out->push(msg);
		break;
	case ENCRYPT:
		try
		{
			float t = 0.0f;
			std::string hash = OpenSSLFacade::hash(msg.dataFilePath);
			if(msg.library == LIB::LIB_OpenSSL)
			{
				if(isSymmetric(msg.algorithm))
				{
					t = OpenSSLFacade::symmetricEncrypt(msg.keyFilePath, msg.dataFilePath, cipherOpenSSL[msg.algorithm]);
				}
				else
				{
					t = OpenSSLFacade::asymmetricEncrypt(msg.keyFilePath, msg.dataFilePath);
				}
			}
			else
			{
				if(msg.algorithm == ALG_AES_256_CBC)
				{
					//TODO aes implementation
				}
				else
				{
					t = OpenGG::asymmetricEncrDecr(msg.keyFilePath, msg.dataFilePath, true);
				}
			}
			msg.setResponse("", t, hash);
		}
		catch (std::exception ex)
		{
			msg.setResponse(ex.what());
		}

		out->push(msg);
		break;
	case DECRYPT:
		try
		{
			float t = 0.0f;
			std::string hash;
			if (msg.library == LIB::LIB_OpenSSL)
			{
				if (isSymmetric(msg.algorithm))
				{
					t = OpenSSLFacade::symmetricDecrypt(msg.keyFilePath, msg.dataFilePath, cipherOpenSSL[msg.algorithm]);
				}
				else
				{
					t = OpenSSLFacade::asymmetricDecrypt(msg.keyFilePath, msg.dataFilePath);
				}
			}
			else
			{
				if (msg.algorithm == ALG_AES_256_CBC)
				{
					//TODO aes implementation
				}
				else
				{
					t = OpenGG::asymmetricEncrDecr(msg.keyFilePath, msg.dataFilePath, false);
				}
			}
			hash = OpenSSLFacade::hash(msg.dataFilePath);
			msg.setResponse("", t, hash);
		}
		catch (std::exception ex)
		{
			msg.setResponse(ex.what());
		}

		out->push(msg);
		break;
	}
}

void Controller::start()
{
	while(running)
	{
		Message msg = in->front();
		in->pop();

		if (msg.message.compare("stop"))
			break;

		processMessage(msg);
	}
}
