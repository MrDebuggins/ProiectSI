#include "Controller/Controller.h"
#include "Crypto/OpenSSLFacade.h"
#include "Crypto/OpenGG.h"
#include "Interface.h"


using namespace std;

int main()
{
    sqlite3* DB;
    Interface interface;

    int ok = interface.optionChoice();
    while (true)
    {
        if (ok == 0)
            break;
        ok = interface.optionChoice();
        
    }

    //cout << OpenGG::asymmetricEncrDecr("rsa4096.txt", "lol.txt", true) << endl;
    //cout << OpenGG::asymmetricEncrDecr("rsa4096.txt", "lol.txt", false);

    return 0;
}