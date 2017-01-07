#include <iostream>
#include "rsacrypto.h"
#include <gmpxx.h>


bool CoprimeTest(){
    LargeInteger a(17);
    LargeInteger b(5);
    return a.isCoprimeWith(b);
}


bool ModularMultiplicativeInverseTest(){
    LargeInteger a(17);
    LargeInteger b(3120);
    LargeInteger out;
    LargeInteger::ModularMultiplicativeInverse(out, a, b);
    return (out.Compare(2756) != 0);
}

bool Base64Test(){
    const std::string s = "ADP GmbH\nAnalyse Design & Programmierung\nGesellschaft mit beschränkter Haftung" ;

    std::string encoded = CryptoRSA::Base64Encode(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
    std::string decoded = CryptoRSA::Base64Decode(encoded);

    return s.compare(decoded) == 0;
}

bool RSATest(){
    CryptoRSA c;
    c.GenerateKeyPair(1024);
    std::string test_str = "aA1@# Hello World!"
    std::string msg(test_str);
    std::string cipher = c.Encrypt(msg);
    std::string output = c.Decrypt(cipher);
    return output.compare(test_str) == 0;
}

int main(){
    cout << "Coprime Test......" << flush;
    cout << CoprimeTest() ? "Pass" : "Fail" << endl;

    cout << "Modular Multiplicative Inverse Test......" << flush;
    cout << ModularMultiplicativeInverseTest() ? "Pass" : "Fail" << endl;

    cout << "Base64 Test......" << flush;
    cout << Base64Test() ? "Pass" : "Fail" << endl;

    cout << "RSA Test......" << flush;
    cout << RSATest() ? "Pass" : "Fail" << endl;


    return 0;
}