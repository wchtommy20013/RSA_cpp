#ifndef __RSA_LARGE_INTEGER_WCH_H
#define __RSA_LARGE_INTEGER_WCH_H
#include <cmath>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gmpxx.h>
#include "largeinteger.h"
using namespace std;

#define USE_HEX 16
#define USE_BIN 2
#define USE_DEC 10

// LargeInteger make use of gmp library
class LargeInteger{
    private:
        static bool rand_state_inited;
        static gmp_randstate_t rand_state;
    public:
        LargeInteger(){ mpz_init(num); }
        LargeInteger(std::string s, int m = USE_DEC){ 
            mpz_init(num); 
            mpz_init_set_str(num, s.c_str(), m);
        }
        LargeInteger(int i){ 
            mpz_init(num); 
            mpz_init_set_ui(num, i);
        }
        LargeInteger(mpz_t n){
            mpz_init(num);
            mpz_set(num, n);
        }
        LargeInteger(const LargeInteger& n){
            mpz_init(num);
            mpz_set(num, n.num);
        }

        mpz_t num;

        void Show(){ gmp_printf("%Zd\n",num); }
        void SetValue(std::string s, int m = USE_DEC){ mpz_init_set_str(num, s.c_str(), m); }
        void SetValue(mpz_t n){ mpz_set(num,n); }
        void SetValue(int i){ mpz_set_ui(num,i); }


        void Delete() { mpz_clear(num); }

        
        //Randomize
        bool isPrime(){ return mpz_probab_prime_p(num, mpz_sizeinbase(num, 2) / 4); }
        bool isCoprimeWith(LargeInteger LI){
            LargeInteger tmp;
            GCD(tmp, *this, LI);
            bool result = mpz_cmp_ui(tmp.num, 1) == 0;
            tmp.Delete();
            return result;
        }

        void PrepareRandom(){ 
            gmp_randinit_default(rand_state);
            gmp_randseed_ui(rand_state, time(NULL));
            rand_state_inited = true;
        }

        void ToPrimeRandom(int bits){ 
            if(!rand_state_inited) PrepareRandom();
            mpz_t temp;
            mpz_init(temp);
            mpz_ui_pow_ui(temp,2, bits - 1);
            do{
                mpz_urandomb(num, rand_state, bits - 1);
                mpz_add(num, num, temp);
                mpz_sub_ui(num, num, 1);
            }while(!isPrime() || mpz_cmp(num, temp) <= 0);
            
            mpz_clear(temp);
        }

        void ToCoprimeRandom(LargeInteger ref_LI, int bits){
            if(!rand_state_inited) PrepareRandom();
            mpz_t temp;
            mpz_init(temp);
            mpz_ui_pow_ui(temp,2, bits - 1);
            do{
                mpz_urandomb(num, rand_state, bits - 1);
                mpz_add(num, num, temp);
                mpz_sub_ui(num, num, 1);
            }while(!isCoprimeWith(ref_LI) || mpz_cmp(num, temp) <= 0);
            
            mpz_clear(temp);
        }

        //Operations
        int Compare(int i){
            return mpz_cmp_ui(num, i);
        }

        static void Totient(LargeInteger& out_LI, LargeInteger p, LargeInteger q){
            mpz_t temp;
            mpz_init(temp);
            mpz_sub_ui(out_LI.num, q.num, 1);
            mpz_sub_ui(temp, p.num, 1);
            mpz_mul(out_LI.num, out_LI.num, temp);
            mpz_clear(temp);
        }
        static void Add(LargeInteger& out_LI, LargeInteger p, LargeInteger q){
            mpz_add(out_LI.num, p.num, q.num);
        }
        static void Subtract(LargeInteger& out_LI, LargeInteger p, LargeInteger q){
            mpz_sub(out_LI.num, p.num, q.num);
        }

        static void Multiply(LargeInteger& out_LI, LargeInteger p, LargeInteger q){
            mpz_mul(out_LI.num, p.num, q.num);
        }

        static void Divide(LargeInteger& out_LI, LargeInteger p, LargeInteger q){
            mpz_div(out_LI.num, p.num, q.num);
        }

        static void Modulus(LargeInteger& out_LI, LargeInteger m, LargeInteger n){
            mpz_mod(out_LI.num, m.num, n.num);
        }

        static void ModuloMod(LargeInteger& out_LI, LargeInteger m, LargeInteger n, LargeInteger o ){
            mpz_powm(out_LI.num, m.num, n.num, o.num);
        }

        static void GCD(LargeInteger& out_LI ,LargeInteger m, LargeInteger n){
            if( n.Compare(0) == 0 )  {
                out_LI.SetValue(m.num);
            }else {            
                Modulus(m,m,n);
                GCD(out_LI, n, m);
            }
        }

        static void ExtendedGCD(LargeInteger a, LargeInteger b, LargeInteger& x, LargeInteger& y, LargeInteger& res){
            // Base Case
            if (a.Compare(0) == 0){
                x.SetValue(0);
                y.SetValue(1);
                res.SetValue(b.num);
                return;
            }
        
            LargeInteger x1, y1; // To store results of recursive call
            LargeInteger tmp;
            LargeInteger::Modulus(tmp,b,a);
            ExtendedGCD(tmp, a, x1, y1, res);
        
            // Update x and y using results of recursive
            // call
            LargeInteger::Divide(x,b,a);
            LargeInteger::Multiply(x,x,x1);
            LargeInteger::Subtract(x, y1, x);
            y.SetValue(x1.num);

            x1.Delete();
            y1.Delete();
            tmp.Delete();
        }

        static bool ModularMultiplicativeInverse(LargeInteger a, LargeInteger m, LargeInteger &out_LI){
            LargeInteger x, y;
            LargeInteger g;
            ExtendedGCD(a, m, x, y, g);
            if (g.Compare(1) != 0){
                cout << "Inverse doesn't exist";
                
                g.Delete();
                x.Delete();
                y.Delete();
                
                return false;
            }else{
                LargeInteger::Modulus(out_LI, x ,m);
                LargeInteger::Add(out_LI, out_LI, m);
                LargeInteger::Modulus(out_LI, out_LI, m);
                
                g.Delete();
                x.Delete();
                y.Delete();

                return true;
            }        
        }
};


#endif