#include "Controller/Controller.h"
#include "Crypto/OpenSSLFacade.h"
#include "Crypto/OpenGG.h"
#include "Interface.h"


using namespace std;

int main()
{
    sqlite3* DB;

    while (true)
    {
        int ok = Interface::optionChoice();
        if (ok == 0)
            break;
    }

    //cout << OpenGG::asymmetricEncrDecr("rsa4096.txt", "lol.txt", true) << endl;
    //cout << OpenGG::asymmetricEncrDecr("rsa4096.txt", "lol.txt", false);

    return 0;
}