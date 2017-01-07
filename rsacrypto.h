#ifndef __RSA_CRYTO_WCH_H
#define __RSA_CRYTO_WCH_H
#include <cmath>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "largeinteger.h"
using namespace std;

#define MAX_BLOCK_SIZE(x) (x/4)
#define HEX_BLOCK_SIZE(x) (x/2)

struct PublicKey{
    LargeInteger e;
    LargeInteger n;
};

struct PrivateKey{
    LargeInteger d;
};


class CryptoRSA{
    private:
        PublicKey public_key;
        PrivateKey private_key;
        size_t bits;
    public:
        CryptoRSA();
        PublicKey GetPublicKey() { return public_key; }
        PrivateKey GetPrivateKey() { return private_key; }
        int GetMaxBlockSize() { return MAX_BLOCK_SIZE(bits); }

        void GenerateKeyPair(int bits){
            LargeInteger p, q;
            p.ToPrimeRandom(bits);
            do{
                q.ToPrimeRandom(bits);
            }while(mpz_cmp(p.num, q.num) == 0);

            LargeInteger totient_n;
            LargeInteger::Multiply(public_key.n, p, q);
            LargeInteger::Totient(totient_n, p, q);

            public_key.e.ToCoprimeRandom(totient_n, bits);
        
            LargeInteger::ModularMultiplicativeInverse(public_key.e, totient_n, private_key.d);

            CryptoRSA::bits = bits;

            p.Delete();
            q.Delete();            
            totient_n.Delete();
        }

        void Finish(){
            public_key.e.Delete();
            public_key.n.Delete();
            private_key.d.Delete();
        }
        
        void Encrypt_LI(LargeInteger msg, LargeInteger &out_cipher, bool reverseUseOfKeys = false){
            mpz_powm(out_cipher.num, msg.num, reverseUseOfKeys ? private_key.d.num : public_key.e.num, public_key.n.num);
        }
    
        void Decrypt_LI(LargeInteger cipher, LargeInteger &out_msg, bool reverseUseOfKeys = false){
            mpz_powm(out_msg.num, cipher.num, reverseUseOfKeys ? public_key.e.num : private_key.d.num, public_key.n.num);
        }


        //Encryption/Decryption
        std::string Encrypt(std::string msg, bool reverseUseOfKeys = false){
            std::string cipher = "";
            size_t byte_encrypted = 0;
            size_t msglen = msg.length();

            std::stringstream cipher_ss;
            LargeInteger tmp;
            while(byte_encrypted < msglen){
                for(int j = 7; j >= 0; j--)
                    cipher_ss << ((msg[byte_encrypted] >> j) & 1);
                byte_encrypted++;
                
                // if reached max block size or done
                if((byte_encrypted % MAX_BLOCK_SIZE(bits) == 0 && byte_encrypted != 0) || byte_encrypted == msglen){
                    tmp.SetValue(cipher_ss.str(), USE_BIN);
                    mpz_powm(tmp.num, tmp.num, reverseUseOfKeys ? private_key.d.num : public_key.e.num, public_key.n.num);
                        
                    size_t size = mpz_sizeinbase(tmp.num, 16) + 2;
                    char buf[size];
                    memset(buf,0,size);
                    mpz_get_str(buf, 16, tmp.num);
                    if(size - 2 < HEX_BLOCK_SIZE(bits)){
                        for(int i = size - 2; i < HEX_BLOCK_SIZE(bits); i++){
                            cipher += '0';
                        }
                    }

                    cipher += buf;
                    cipher_ss.str("");
                }
            }

            tmp.Delete();
            
            return cipher;
        }

        std::string Decrypt(std::string cipher, bool reverseUseOfKeys = false){
            size_t byte_decrypted = 0;
            size_t msglen = cipher.length();

            std::stringstream tmp_ss, msg_ss;
            
            LargeInteger tmp;
            
            int block_size = HEX_BLOCK_SIZE(bits);
            int block_num = (cipher.length() + 1) / block_size ;

            for(int count = 0; count < block_num ; count++){
                tmp.SetValue(cipher.substr(block_size * count, block_size),USE_HEX);
                mpz_powm(tmp.num, tmp.num, reverseUseOfKeys ? public_key.e.num : private_key.d.num, public_key.n.num);

                size_t size = mpz_sizeinbase(tmp.num, 2) + 2;
                char buf[size];
                memset(buf,0,size);
                mpz_get_str(buf, 2, tmp.num);

                int remainder = (size - 2) % 8;
                remainder = remainder == 0 ? 0 : 8 - remainder;
                for(int k = 0; k < 8; k++){
                    if(k < remainder){
                        tmp_ss << '0';
                    }else{
                        tmp_ss << buf[k - remainder];
                    }
                }
                msg_ss << (char) strtol(tmp_ss.str().c_str(),NULL,2);
                tmp_ss.str("");
                int i = 1;
                while(i*8 < (size - 2)){
                    for(int j = 0; j < 8; j++){
                        tmp_ss << buf[i*8 + j - remainder];
                    }   

                    msg_ss << ((char) strtol(tmp_ss.str().c_str(),NULL,2));
                    i++;         
                    tmp_ss.str("");
                }
            }

            tmp.Delete();

            return msg_ss.str();
        }

        //Base64
        static inline bool isBase64(unsigned char c);
        static std::string Base64Encode(unsigned char const* bytes_to_encode, unsigned int in_len);
        static std::string Base64Decode(std::string const& encoded_string);
};

#endif
