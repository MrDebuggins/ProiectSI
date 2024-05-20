#include "DB.h"
#include "Controller/Controller.h"
#include "Crypto/OpenSSLFacade.h"
#include "Crypto/OpenGG.h"
//#include <openssl/applink.c>


using namespace std;

int main()
{
    char f[] = "file.txt";

    const char* dir = "SI.db";
    sqlite3* DB;

    cout << OpenGG::asymmetricEncrDecr("rsa4096.txt", "lol.txt", true) << endl;
    cout << OpenGG::asymmetricEncrDecr("rsa4096.txt", "lol.txt", false);

    return 0;
}