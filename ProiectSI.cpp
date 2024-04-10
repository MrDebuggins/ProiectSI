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

    system("Pause");
    return 0;
}