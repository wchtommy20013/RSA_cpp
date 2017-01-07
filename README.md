# RSA_cpp
This is a slower version of RSA developed by <b>Tommy Wong</b>

## To include or compile the project
This project required GMP https://gmplib.org/
~~~
# g++ -o program *.cpp -lgmp -lgmpxx
~~~


## How to use

###1. Initiate an RSA object
~~~
CryptoRSA rsa;
~~~

###2. Generate a key with a specified length
~~~
rsa.GenerateKeyPair(1024);
~~~
###3. Encryption/Decryption
~~~
std::string msg = "Some string to be encrypted." 

std::string cipher = rsa.Encrypt(msg);      // cipher = (The encrypted hex-string)
std::string output = rsa.Decrypt(cipher);   // output = msg
~~~
