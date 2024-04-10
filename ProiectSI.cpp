#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include "DB.h"
#include "Crypto/OpenSSLFacade.h"


using namespace std;

int main(int argc, char* argv[])
{
    //OpenSSLFacade::symmetricEncrypt("bubn.txt", "lol.txt", (void*)EVP_aes_256_cbc());
    //OpenSSLFacade::symmetricDecrypt("bubn.txt", "lol.txt", (void*)EVP_aes_256_cbc());
    //OpenSSLFacade::genRSAKey(4096, "rsa.txt");
    OpenSSLFacade::asymmetricEncrypt("rsa.txt", "lol.txt", nullptr);
    //OpenSSLFacade::asymmetricDecrypt("rsa.txt", "lol.txt", nullptr);

    return 0;
}