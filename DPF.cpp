# include <random>
# include <ctime>
# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <string>
# include <fstream>

# include <gmp.h>
# include <assert.h>

# include "AES.hpp"
# include "DPF.hpp"

# define LAMBDA (127)
# define KEYBITS (128)

gmp_randstate_t gen;

void key_gen(mpz_t seed_127)
{
    unsigned long buffer = 0;
    buffer = gmp_urandomb_ui(gen,32);
    mpz_set_ui(seed_127, buffer);
    mpz_mul_ui(seed_127, seed_127, 4294967296);
    buffer = gmp_urandomb_ui(gen,32);
    mpz_add_ui(seed_127, seed_127, buffer);
    mpz_mul_ui(seed_127, seed_127, 4294967296);
    buffer = gmp_urandomb_ui(gen,32);
    mpz_add_ui(seed_127, seed_127, buffer);
    mpz_mul_ui(seed_127, seed_127, 2147483648);
    buffer = gmp_urandomb_ui(gen,31);
    mpz_add_ui(seed_127, seed_127, buffer);
};

void doubleKey(const mpz_t key_127, mpz_t key_256, mpz_t init_vect)
{
    // Variables
    uint32_t rk[44];
    mpz_t key_128, cipher, power2_128;
    mpz_init(key_128); mpz_init(cipher); mpz_init(power2_128);
    mpz_ui_pow_ui(power2_128, 2, 128); // 2^128
    int check_bits;

    // Operations
    mpz_mul_ui(key_128, key_127, 2); // s||0
    int nrounds = expandEncryptKey(rk, key_128, 128); // Round Keys  
    encryptAES(rk, nrounds, init_vect, cipher); // 1er AES
    mpz_mul(key_256, cipher, power2_128);
    mpz_add_ui(init_vect, init_vect, 1); // IV incrementation
    check_bits = mpz_sizeinbase(init_vect, 2);
    if (check_bits == 129) { mpz_set_ui(init_vect,0); }
    encryptAES(rk, nrounds, init_vect, cipher); // 2e AES
    mpz_add(key_256, key_256, cipher);
    mpz_add_ui(init_vect, init_vect, 1); // IV incrementation
    check_bits = mpz_sizeinbase(init_vect, 2);
    if (check_bits == 129) { mpz_set_ui(init_vect,0); }

    // Memory space
    mpz_clear(key_128); mpz_clear(cipher); mpz_clear(power2_128);
};

void get_tLR(mpz_t sLR, int* tLR)
{
    tLR[0] = mpz_tstbit(sLR, 128U); // tL
    tLR[1] = mpz_tstbit(sLR, 0U); // tR
};

void Gen(const mpz_t seed, const mpz_t alpha, const int alpha_bits, mpz_t beta, const int beta_bits, mpz_t* key_0, mpz_t* key_1)
{
    // Seeding mersene random generator
    gmp_randinit_mt(gen);   
    gmp_randseed(gen, seed);

    // Generating four random seeds
    mpz_t init_vect0, init_vect1, s_0, s_1;
    mpz_init(init_vect0);
    key_gen(init_vect0);
    mpz_init(init_vect1);
    key_gen(init_vect1);
    mpz_init(s_0);
    key_gen(s_0);
    mpz_init(s_1);
    key_gen(s_1);

    // Initialisation
    int t[2] = {0, 1};
    mpz_init(key_0[0]);
    mpz_set(key_0[0], init_vect0);
    mpz_init(key_1[0]);
    mpz_set(key_1[0], init_vect1);
    mpz_init(key_0[1]);
    mpz_set(key_0[1], s_0);
    mpz_init(key_1[1]);
    mpz_set(key_1[1], s_1);

    // Declaration of variables : L == 0 , R == 1
    int alpha_i;
    mpz_t sLR_0, sLR_1, sCW;
    mpz_init(sLR_0);
    mpz_init(sLR_1);
    mpz_init(sCW);
    int tLR_0[2];
    int tLR_1[2];
    int tCW[2];

    mpz_t power2_128;
    mpz_init(power2_128);
    mpz_ui_pow_ui(power2_128, 2, 128); // 2^128

    // Loop
    for (int i = 0; i < alpha_bits; i++)
    {
        // Seed expansion of s||0
        doubleKey(s_0, sLR_0, init_vect0);
        doubleKey(s_1, sLR_1, init_vect1);
        get_tLR(sLR_0, tLR_0);
        get_tLR(sLR_1, tLR_1);

        // Variables
        alpha_i = mpz_tstbit(alpha, alpha_bits - 1 - i);
        tCW[0] = tLR_0[0] ^ tLR_1[0] ^ alpha_i ^ 1;
        tCW[1] = tLR_0[1] ^ tLR_1[1] ^ alpha_i;
        mpz_xor(sCW, sLR_0, sLR_1);
        if (alpha_i == 0) { mpz_fdiv_r(sCW, sCW, power2_128); } 
        else { mpz_fdiv_q(sCW, sCW, power2_128); }
        mpz_fdiv_q_ui(sCW, sCW, 2);

        // Store Correcting  Words
        mpz_init(key_0[i+2]);
        mpz_mul_ui(key_0[i+2], sCW, 4);
        mpz_add_ui(key_0[i+2], key_0[i+2], 2*tCW[0] + tCW[1]);
        mpz_init(key_1[i+2]);
        mpz_mul_ui(key_1[i+2], sCW, 4);
        mpz_add_ui(key_1[i+2], key_1[i+2], 2*tCW[0] + tCW[1]);

        // Next nodes
        if (alpha_i == 0) 
        { 
            mpz_fdiv_q(s_0, sLR_0, power2_128);
            mpz_fdiv_q(s_1, sLR_1, power2_128);
        } 
        else 
        {  
            mpz_fdiv_r(s_0, sLR_0, power2_128);
            mpz_fdiv_r(s_1, sLR_1, power2_128);
        }
        mpz_fdiv_q_ui(s_0, s_0, 2);
        mpz_fdiv_q_ui(s_1, s_1, 2);
        if (t[0] == 1)
        {
            mpz_xor(s_0, s_0, sCW);
            t[0] = tCW[alpha_i];
        }
        t[0] ^= tLR_0[alpha_i];
        if (t[1] == 1)
        {
            mpz_xor(s_1, s_1, sCW);
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
    */

    mpz_t power2;
    mpz_init(power2);
    mpz_ui_pow_ui(power2, 2, beta_bits);

    mpz_mod(s_0, s_0, power2);
    mpz_mod(s_1, s_1, power2);

    mpz_sub(beta, beta, s_0);
    mpz_mod(beta, beta, power2);
    mpz_add(beta, beta, s_1);
    mpz_mod(beta, beta, power2);
    if (t[1]==1) { mpz_sub(beta, power2, beta); }
    
    mpz_init(key_0[alpha_bits + 2]);
    mpz_set(key_0[alpha_bits + 2], beta);
    mpz_set(key_1[alpha_bits + 2], beta);

    mpz_clear(init_vect0); mpz_clear(init_vect1);
    mpz_clear(power2_128); mpz_clear(power2);
    mpz_clear(s_0); mpz_clear(s_1);
    mpz_clear(sLR_0); mpz_clear(sLR_1); mpz_clear(sCW);
};

void Eval(const mpz_t input, const int input_bits, mpz_t output, const int output_bits, const mpz_t* key, const int b)
{
    // Initialisation
    int t = b;
    mpz_t init_vect, s;
    mpz_init(init_vect);
    mpz_set(init_vect, key[0]);
    mpz_init(s);
    mpz_set(s, key[1]);

    // Declaration of variables
    int input_i;
    mpz_t CW, CW_LR, sLR;
    mpz_init(CW);
    mpz_init(CW_LR);
    mpz_init(sLR);
    int tCW_R;

    mpz_t power2_128;
    mpz_init(power2_128);
    mpz_ui_pow_ui(power2_128, 2, 128); // 2^128

    // Loop
    for (int i = 0; i < input_bits; i++)
    {
        // Correcting Words
        mpz_set(CW, key[i+2]);
        tCW_R = mpz_tstbit(CW,0);
        mpz_fdiv_q_ui(CW, CW, 2);
        mpz_mul(CW_LR, CW, power2_128);
        mpz_add(CW_LR, CW_LR, CW);
        mpz_clrbit(CW_LR, 0);
        mpz_add_ui(CW_LR, CW_LR, tCW_R);

        // Seed expansion of s||0
        doubleKey(s, sLR, init_vect);
        
        // Tau
        if (t==1) { mpz_xor(sLR, sLR, CW_LR); }

        // Next node
        input_i = mpz_tstbit(input, input_bits - 1 - i);
        if (input_i == 0) { mpz_fdiv_q(s, sLR, power2_128); mpz_fdiv_r(s, s, power2_128); } else { mpz_fdiv_r(s, sLR, power2_128); }
        t = mpz_tstbit(s, 0);   
        mpz_fdiv_q_ui(s, s, 2);
    }
    
    /*
    std::cout << std::endl;
    std::cout << "FINAL SEED " << b << std::endl;
    mpz_out_str(stdout,2,s);
    std::cout << std::endl;
    */

    mpz_clear(init_vect); mpz_clear(sLR); mpz_clear(CW); mpz_clear(CW_LR);

    mpz_t power2;
    mpz_init(power2);
    mpz_ui_pow_ui(power2, 2, output_bits);

    mpz_mod(s, s, power2);

    mpz_set(output,s);
    if (t == 1) { mpz_add(output, output, key[input_bits+2]); }
    if (b == 1) { mpz_sub(output, power2, output); mpz_mod(output, output, power2); }

    mpz_clear(s); mpz_clear(power2); mpz_clear(power2_128);
}

/*
int main(int argc, char **argv)
{
    std::cout << std::endl;
    int flag;
    char* seed_char = argv[1];
    mpz_t seed;
    mpz_init(seed);
    mpz_set_ui(seed,0);
    flag = mpz_set_str(seed, seed_char, 2);
    assert (flag == 0);
    std::cout << "key 127 " ;
    mpz_out_str(stdout, 10, seed);
    std::cout << std::endl << mpz_sizeinbase(seed, 2) << " bits" << std::endl << std::endl;

    char* vect_char = argv[2];
    mpz_t vect;
    mpz_init(vect);
    mpz_set_ui(vect,0);
    flag = mpz_set_str(vect, vect_char, 2);
    assert (flag == 0);
    std::cout << "vect " ;
    mpz_out_str(stdout, 10, vect);
    std::cout << std::endl  << mpz_sizeinbase(vect, 2) << " bits" << std::endl << std::endl;

    mpz_t seed2;
    mpz_init(seed2);
    doubleKey(seed, seed2, vect);
    std::cout << "key 256 " ;
    mpz_out_str(stdout, 10, seed2);
    std::cout << std::endl  << mpz_sizeinbase(seed2, 2) << " bits" << std::endl << std::endl;

    inv_doubleKey(seed, seed2, vect);

    mpz_clear(seed); mpz_clear(vect); mpz_clear(seed2);
    return 0;    
}
*/