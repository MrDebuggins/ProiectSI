#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include "DB.h"


using namespace std;

int main(int argc, char* argv[])
{
    unsigned char outbuf[1024];
    int outlen, tmplen;
    /*
     * Bogus key and IV: we'd normally set these from
     * another source.
     */
    unsigned char key[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
    unsigned char iv[] = { 1,2,3,4,5,6,7,8 };
    char intext[] = "Some Crypto Text";
    EVP_CIPHER_CTX* ctx;
    FILE* out;

    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_EncryptInit_ex2(ctx, EVP_idea_cbc(), key, iv, NULL)) {
        /* Error */
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    
    if (!EVP_EncryptUpdate(ctx, outbuf, &outlen, (unsigned char*)intext, strlen(intext))) {
        /* Error */
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    /*
     * Buffer passed to EVP_EncryptFinal() must be after data just
     * encrypted to avoid overwriting it.
     */
    if (!EVP_EncryptFinal_ex(ctx, outbuf + outlen, &tmplen)) {
        /* Error */
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    outlen += tmplen;
    EVP_CIPHER_CTX_free(ctx);
    /*
     * Need binary mode for fopen because encrypted data is
     * binary data. Also cannot use strlen() on it because
     * it won't be NUL terminated and may contain embedded
     * NULs.
     */

    // Am comentat ce e sub

    //out = fopen_s(out*, "wb", "r");
    //if (out == NULL) {
        /* Error */
    //   return 0;
    //}
    //fwrite(outbuf, 1, outlen, out);
    //fclose(out);
    return 1;
}


int main()
{
    char f[] = "file.txt";
    int a = do_crypt(f);
    // cout << a << endl;

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