#include <iostream>
#include "rsacrypto.h"
#include <gmpxx.h>

using namespace std;


int main(){
        CryptoRSA c;
        c.GenerateKeyPair(1024);

        std::string msg("Any String can be encrypt/decrypted");

        cout << "================Message================" << endl;
        cout << msg << endl;
        std::string cipher = c.Encrypt(msg);
        cout << "================Cipher================" << endl;
        cout << cipher << endl;
        std::string output = c.Decrypt(cipher);
        cout << "================Output================" << endl;
        cout << output << endl;
        cout << "=====================================" << endl;

}