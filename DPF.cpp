# include <random>
# include <ctime>
# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <string>
# include <fstream>
# include <vector>

# include <gmp.h>
# include <gmpxx.h>
# include <assert.h>

# include "AES.hpp"
# include "DPF.hpp"

# define LAMBDA (127)
# define KEYBITS (128)

/*
gmp_randstate_t gen;

void key_gen(mpz_class seed_127)
{
    unsigned long buffer = 0;
    buffer = gmp_urandomb_ui(gen,32);
    seed_127 = buffer;
    buffer = gmp_urandomb_ui(gen,32);
    seed_127 = (seed_127 << 32) + buffer;
    buffer = gmp_urandomb_ui(gen,32);
    seed_127 = (seed_127 << 32) + buffer;
    buffer = gmp_urandomb_ui(gen,31);
    seed_127 = (seed_127 << 31) + buffer;
};
*/

mpz_class doubleKey(const mpz_class key_127, mpz_class init_vect)
{
    // Variables
    uint32_t rk[44];
    mpz_class key_128, cipher, key_256;
    mpz_class power2_128(1); power2_128 = power2_128 << 128;

    // Operations
    key_128 = key_127 << 1; // s||0
    int nrounds = expandEncryptKey(rk, key_128, 128); // Round Keys  
    cipher = encryptAES(rk, nrounds, init_vect); // 1er AES
    key_256 = cipher;
    init_vect = (init_vect + 1) % power2_128; // IV incrementation
    cipher = encryptAES(rk, nrounds, init_vect); // 2e AES
    key_256 = (key_256 << 128) + cipher;

    return key_256;
};

mpz_class convert(const mpz_class seed, const int g_bits, mpz_class init_vect)
{
    mpz_class g_seed;
    mpz_class power2(1);  power2 = power2 << g_bits;
    int p = g_bits / 128; int q = g_bits % 128;
    if (p == 0) { g_seed = seed; }
    else 
    {
        // Variables
        uint32_t rk[44];
        mpz_class key_128, cipher;
        mpz_class power2_128(1); power2_128 = power2_128 << 128;

        // Operations
        key_128 = seed << 1; // s||0
        int nrounds = expandEncryptKey(rk, key_128, 128); 
        if (q > 0) { p += 1;}
        cipher = encryptAES(rk, nrounds, init_vect);
        g_seed = cipher;
        p -= 1;
        while(p > 0)
        {
            init_vect = (init_vect + 1) % power2_128;
            cipher = encryptAES(rk, nrounds, init_vect);
            g_seed = (g_seed << 128) + cipher;
            p -= 1;
        }
    }
    g_seed = g_seed % power2;
    //std::cout << seed << " mod " << power2 << " = " << g_seed << std::endl;
    return g_seed;
};

void get_tLR(mpz_class sLR, int* tLR)
{
    tLR[0] = mpz_tstbit(sLR.get_mpz_t(), 128U); // tL
    tLR[1] = mpz_tstbit(sLR.get_mpz_t(), 0U); // tR
};

void Gen(const mpz_class seed, const mpz_class alpha, const int alpha_bits, mpz_class beta, const int beta_bits, std::vector<mpz_class>& key_0, std::vector<mpz_class>& key_1)
{
    // Seeding mersene random generator
    gmp_randclass gen(gmp_randinit_mt);
    gen.seed(seed);

    // Generating four random seeds
    mpz_class init_vect0, init_vect1, s_0, s_1;
    init_vect0 = gen.get_z_bits(127);
    init_vect1 = init_vect0;
    s_0 = gen.get_z_bits(127);
    s_1 = gen.get_z_bits(127);

    // Initialisation
    int t[2] = {0, 1};
    key_0.push_back(init_vect0);
    key_1.push_back(init_vect1);
    key_0.push_back(s_0);
    key_1.push_back(s_1);

    // Declaration of variables : L == 0 , R == 1
    int threshold = alpha_bits - mpz_sizeinbase(alpha.get_mpz_t(), 2);
    int alpha_i;
    mpz_class sLR_0, sLR_1, sCW, buffer;
    int tLR_0[2];
    int tLR_1[2];
    int tCW[2];

    mpz_class power2_128(1); power2_128 = power2_128 << 128; // 2^128

    // Loop
    for (int i = 0; i < alpha_bits; i++)
    {
        // Seed expansion of s||0
        // std::cout << s_0 << std::endl;
        sLR_0 = doubleKey(s_0, init_vect0);
        sLR_1 = doubleKey(s_1, init_vect1);
        init_vect0 = (init_vect0 + 2) % power2_128;
        init_vect1 = (init_vect1 + 2) % power2_128;
        get_tLR(sLR_0, tLR_0);
        get_tLR(sLR_1, tLR_1);
        std::cout << (sLR_0 >> 128) << ' ' << tLR_0[0] << ' ' << (sLR_0 % power2_128) << ' ' << tLR_0[1] << std::endl;
        std::cout << (sLR_1 >> 128) << ' ' << tLR_1[0] << ' ' << (sLR_1 % power2_128) << ' ' << tLR_1[1] << std::endl;

        // Variables
        if (i < threshold) { alpha_i = 0; } 
        else { alpha_i = mpz_tstbit(alpha.get_mpz_t(), alpha_bits - 1 - i); }
        std::cout << alpha_i << std::endl << std::endl;
        
        tCW[0] = tLR_0[0] ^ tLR_1[0] ^ alpha_i ^ 1;
        tCW[1] = tLR_0[1] ^ tLR_1[1] ^ alpha_i;
        //std::cout << i << " : " << t[0] << std::endl << i << " : " << t[1] << std::endl;
        //std::cout << i << " : " << tCW[0] << ' ' << tCW[1] << std::endl;

        sCW = sLR_0 ^ sLR_1;
        if (alpha_i == 0) { sCW = sCW % power2_128; } // RIGHT
        else { sCW = sCW >> 128; } // LEFT
        sCW = sCW >> 1;

        // Store Correcting  Words
        buffer = (sCW << 2) + (2*tCW[0] + tCW[1]);
        key_0.push_back(buffer);
        key_1.push_back(buffer);

        // Next nodes
        if (alpha_i == 0) 
        { 
            s_0 = sLR_0 >> 128;
            s_1 = sLR_1 >> 128;
        } 
        else 
        {  
            s_0 = sLR_0 % power2_128;
            s_1 = sLR_1 % power2_128;
        }
        s_0 = s_0 >> 1;
        s_1 = s_1 >> 1;
        if (t[0] == 1)
        {
            s_0 = s_0 ^ sCW;
            t[0] = tCW[alpha_i];
        }
        t[0] ^= tLR_0[alpha_i];
        if (t[1] == 1)
        {
            s_1 = s_1 ^ sCW;
            t[1] = tCW[alpha_i];
        }
        t[1] ^= tLR_1[alpha_i];
    }
    /*
    std::cout << std::endl;
    std::cout << "FINAL SEEDS" << std::endl;
    mpz_out_str(stdout,2,s_0);
    std::cout << std::endl;
    mpz_out_str(stdout,2,s_1);
    std::cout << std::endl;
    
    mpz_class power2;
    mpz_init(power2);
    mpz_ui_pow_ui(power2, 2, beta_bits);
    */

    //std::cout << s_0 << ' ' << s_1 << std::endl;
    //std::cout << t[0] << " ^ " << t[1] << " = " << (t[0]^t[1]) << std::endl;
    s_0 = convert(s_0, beta_bits, init_vect0);
    s_1 = convert(s_1, beta_bits, init_vect1); 
    // std::cout << "7 : " << t[0] << std::endl << "7 : " << s_0 << std::endl << "7 : " << t[1] << std::endl << "7 : " << s_1 << std::endl;
    //std::cout << s_0 << ' ' << s_1 << std::endl;

    beta = beta ^ s_0;
    beta = beta ^ s_1;
    //if (t[1]==1) { mpz_sub(beta, power2, beta); mpz_mod(beta, beta, power2);}

    key_0.push_back(beta);
    key_1.push_back(beta);
};

mpz_class Eval(const mpz_class input, const int input_bits, const int output_bits, const std::vector<mpz_class>& key, const int b)
{
    // Initialisation
    int t = b;
    mpz_class init_vect, s, output;
    init_vect = key[0];
    s = key[1];

    // Declaration of variables
    int threshold = input_bits - mpz_sizeinbase(input.get_mpz_t(), 2);
    int input_i;
    mpz_class CW, CW_LR, sLR;
    int tCW_L, tCW_R;

    mpz_class power2_128(1); power2_128 = power2_128 << 128; // 2^128

    //if (input == 64) { std::cout << input << " : " << t  << ' ' << s << std::endl; }

    // Loop
    for (int i = 0; i < input_bits; i++)
    {
        // Correcting Words
        CW = key[i+2];
        tCW_L = mpz_tstbit(CW.get_mpz_t(),128);
        tCW_R = mpz_tstbit(CW.get_mpz_t(),0);

        CW = (CW >> 2) << 1;
        CW_LR = ((CW + tCW_L) << 128) + (CW + tCW_R);

        // Seed expansion of s||0
        // if (input == 11) {std::cout << s << std::endl;}
        sLR = doubleKey(s, init_vect);
        if (input == 64) 
        { 
            //std::cout << i << " : " << t << std::endl;
            //std::cout << i << " : " << tCW_L << ' ' << tCW_R << std::endl;
            std::cout << (sLR >> 128) << ' ' << (sLR % power2_128) << std::endl; 
        }
        init_vect = (init_vect + 2) % power2_128; // IV incrementation
        
        // Tau
        if (t==1) { sLR = sLR ^ CW_LR; }

        // Next node
        if (i < threshold) { input_i = 0; } 
        else { input_i = mpz_tstbit(input.get_mpz_t(), input_bits - 1 - i); }

        if (input_i == 0) { s = sLR >> 128; } // LEFT
        else { s = sLR % power2_128; } // RIGHT
        t = mpz_tstbit(s.get_mpz_t(), 0);  
        s = s >> 1;
        if (input == 64) { std::cout << input_i << std::endl << std::endl; }
    }

    // std::cout << input << " : " << s << std::endl;
    

    s = convert(s, output_bits, init_vect) ;
    //if (input == 64) { std::cout << "7 : " << t << std::endl << "7 : " << s << std::endl ; }

    output = s;
    if (t == 1) 
    { 
        output = output ^ key[input_bits+2]; 
        //if (input == 64) { std::cout << input << " : " << output << ' ' << s << std::endl; }
    }
    // if (b == 1) { mpz_sub(output, power2, output); }

    return output;
}

/*
int main(int argc, char **argv)
{
    std::cout << std::endl;
    mpz_class seed(123);
    std::cout << "key 127 " << seed << std::endl << mpz_sizeinbase(seed.get_mpz_t(), 2) << " bits" << std::endl << std::endl;

    mpz_class vect(11);
    std::cout << "vect " << vect << std::endl  << mpz_sizeinbase(vect.get_mpz_t(), 2) << " bits" << std::endl << std::endl;

    mpz_class seed2; seed2 = doubleKey(seed, vect);
    std::cout << "key 256 " << seed2 << std::endl  << mpz_sizeinbase(seed2.get_mpz_t(), 2) << " bits" << std::endl << std::endl;

    return 0;    
}
*/
