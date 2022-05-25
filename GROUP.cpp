# include <random>
# include <ctime>
# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>

# include <gmp.h>
# include <assert.h>

# include "GROUP.hpp"

gmp_randstate_t genG;

uint64_t masks[64] = 
{
    0xffffffffffffffffU, 0x7fffffffffffffffU, 0x3fffffffffffffffU, 0x1fffffffffffffffU,
    0x0fffffffffffffffU, 0x07ffffffffffffffU, 0x03ffffffffffffffU, 0x01ffffffffffffffU,
    0x00ffffffffffffffU, 0x007fffffffffffffU, 0x003fffffffffffffU, 0x001fffffffffffffU,
    0x000fffffffffffffU, 0x0007ffffffffffffU, 0x0003ffffffffffffU, 0x0001ffffffffffffU,
    0x0000ffffffffffffU, 0x00007fffffffffffU, 0x00003fffffffffffU, 0x00001fffffffffffU,
    0x00000fffffffffffU, 0x000007ffffffffffU, 0x000003ffffffffffU, 0x000001ffffffffffU,
    0x000000ffffffffffU, 0x0000007fffffffffU, 0x0000003fffffffffU, 0x0000001fffffffffU,
    0x0000000fffffffffU, 0x00000007ffffffffU, 0x00000003ffffffffU, 0x00000001ffffffffU,
    0x00000000ffffffffU, 0x000000007fffffffU, 0x000000003fffffffU, 0x000000001fffffffU,
    0x000000000fffffffU, 0x0000000007ffffffU, 0x0000000003ffffffU, 0x0000000001ffffffU,
    0x0000000000ffffffU, 0x00000000007fffffU, 0x00000000003fffffU, 0x00000000001fffffU,
    0x00000000000fffffU, 0x000000000007ffffU, 0x000000000003ffffU, 0x000000000001ffffU,
    0x000000000000ffffU, 0x0000000000007fffU, 0x0000000000003fffU, 0x0000000000001fffU,
    0x0000000000000fffU, 0x00000000000007ffU, 0x00000000000003ffU, 0x00000000000001ffU,
    0x00000000000000ffU, 0x000000000000007fU, 0x000000000000003fU, 0x000000000000001fU,
    0x000000000000000fU, 0x0000000000000007U, 0x0000000000000003U, 0x0000000000000001U
};

// key <- key1 || key2 (left block incomplete)
void concatenate(const mpz_t key_1, const int n1, const mpz_t key_2, const int n2, mpz_t key)
{
    mpz_t offset;
    mpz_init(offset);
    mpz_ui_pow_ui(offset, 2, n2);
    mpz_mul(key, key_1, offset);
    mpz_add(key, key, key_2);
    mpz_clear(offset);
};

void convert(const mpz_t seed, mpz_t g_seed, const int g_bits)
{
    if (g_bits < 128)
    {
        mpz_t power2;
        mpz_init(power2);
        mpz_ui_pow_ui(power2, 2, g_bits);
        mpz_mod(g_seed, seed, power2);
        mpz_clear(power2);
    }
    else
    // it is rarely the case, we will make sure that the output size is equal or lower than lambda
    {
        mpz_t g_seed0, g_seed1;

        int n0 = g_bits / 2 ;
        int n1 = n0;
        if (n0 % 2 == 1) { n1 += 1; }
        int p0 = n0 / 32;
        int q0 = n0 % 32;
        int p1 = n1 / 32;
        int q1 = n1 % 32;
        
        gmp_randseed(genG, seed);
        unsigned long buffer = 0;

        mpz_init(g_seed0);
        buffer = gmp_urandomb_ui(genG,32);
        mpz_set_ui(g_seed0, buffer);
        for (int i = 0; i < p0; i++)
        {
            mpz_mul_ui(g_seed0, g_seed0, 4294967296);
            buffer = gmp_urandomb_ui(genG,32);
            mpz_add_ui(g_seed0, g_seed0, buffer);
        }
        if (q0 != 0)
        {
            uint64_t mask = masks[64 - q0] * 2;
            mpz_mul_ui(g_seed0, g_seed0, mask);
            buffer = gmp_urandomb_ui(genG,32-q0);
            mpz_add_ui(g_seed0, g_seed0, buffer);
        }

        mpz_init(g_seed1);
        buffer = gmp_urandomb_ui(genG,32);
        mpz_set_ui(g_seed1, buffer);
        for (int i = 0; i < (n1 / 32); i++)
        {
            mpz_mul_ui(g_seed1, g_seed1, 4294967296);
            buffer = gmp_urandomb_ui(genG,32);
            mpz_add_ui(g_seed1, g_seed1, buffer);
        }
        if (q1 != 0)
        {
            uint64_t mask = masks[64 - q1] * 2;
            mpz_mul_ui(g_seed1, g_seed1, mask);
            buffer = gmp_urandomb_ui(genG,32-q1);
            mpz_add_ui(g_seed1, g_seed1, buffer);
        }

        concatenate(g_seed0, n0, g_seed1, n1, g_seed);
        
        mpz_clear(g_seed0); mpz_clear(g_seed1); 
    }
}

void get_final_CW(mpz_t beta, const int beta_bits, const mpz_t s0, const mpz_t s1, const int t)
{
    mpz_t g_s0, g_s1;
    mpz_init(g_s0);
    convert(s0, g_s0, beta_bits);
    mpz_init(g_s1);
    convert(s1, g_s1, beta_bits);

    mpz_t power2;
    mpz_init(power2);
    mpz_ui_pow_ui(power2, 2, beta_bits);

    mpz_sub(beta, beta, g_s0);
    mpz_mod(beta, beta, power2);
    mpz_add(beta, beta, g_s1);
    mpz_mod(beta, beta, power2);
    if (t==1) 
    { 
        mpz_sub(beta, power2, beta);
    }

    mpz_clear(g_s0); mpz_clear(g_s1); mpz_clear(power2);
}