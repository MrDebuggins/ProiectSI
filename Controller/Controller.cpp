#include "Controller.h"
#include "OpenGG.h"
#include "OpenSSLFacade.h"


Controller::Controller(std::queue<Message>* in, std::queue<Message>* out)
{
	this->in = in;
	this->out = out;
}

void Controller::processMessage(Message msg)
{
	
}

void Controller::initLibs()
{
	libraries["SSL"] = OpenSSLFacade::symmetricEncrypt;
	libraries["GG"] = OpenGG::symmetricEncrypt;

	// init OpenSSL ciphers
	ciphers["SSL_AES_256_CBC"] = (void*)EVP_aes_256_cbc();
}


void Controller::start()
{
	while(true)
	{
		Message msg = in->front();
		in->pop();
		processMessage(msg);
	}
}