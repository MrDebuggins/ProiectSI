#include "Controller.h"
#include "OpenGG.h"
#include "OpenSSLFacade.h"


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
				OpenSSLFacade::genRSAKey(msg.time, msg.keyFilePath);
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
					//TODO rsa implementation
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
					//TODO rsa implementation
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
