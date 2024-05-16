#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include "DB.h"
#include "Crypto/OpenSSLFacade.h"


using namespace std;

int main()
{
    char f[] = "file.txt";

    const char* dir = "SI.db";
    sqlite3* DB;

    //createDB(dir);
    //createTable(dir);
    //insertAlgorithm(dir, "bubn");
    //insertKey(dir, 1, "C:");
    //insertFile(dir, 1, "D:", 1, 2);
    

    //OpenSSLFacade::genSymmetricKey((void*)EVP_aes_256_cbc(), "aes_256_cbc.txt");
    //float a = OpenSSLFacade::symmetricEncrypt("aes_256_cbc.txt", "lol.txt", (void*)EVP_aes_256_cbc());
    //float b = OpenSSLFacade::symmetricDecrypt("aes_256_cbc.txt", "lol.txt", (void*)EVP_aes_256_cbc());
    //cout << "encr: " << a << "  decr: " << b << endl;
    //float a1 = OpenSSLFacade::asymmetricEncrypt("rsa.txt", "lol.txt", nullptr);
    //float b1 = OpenSSLFacade::asymmetricDecrypt("rsa.txt", "lol.txt", nullptr);
    //cout << "encr: " << a1 << "  decr: " << b1 << endl;

    return 0;
}