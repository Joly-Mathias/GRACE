# include <random>
# include <ctime>
# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>

# include "GROUP.hpp"

std::mt19937_64 genG;

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
void concatenate(const uint64_t* key_1, const int n1, const uint64_t* key_2, const int n2, uint64_t* key)
{
    int p1 = n1 / 64;
    int q1 = n1 % 64;
    int p2 = n2 / 64;
    int q2 = n2 % 64;
    uint64_t mask1 = masks[64-q1];
    int init2 = (q2 != 0);
    int init1 = (q1 != 0);
    int init = (q1 + q2 > 64) ? 2 : 1 - (1 - init1)*(1 - init2);

    for (int i = 0; i < p2; i++)
    {
        key[init + p1 + i] = key_2[init2 + i];
    }
    if (q2 == 0 )
    {
        for (int j = 0; j < p1; j++)
        {
            key[init + j] = key_1[init1 + j];
        }
        if (q1 > 0)
        {
            key[0] = key_1[0] & mask1;
        }
    }
    else
    {
        uint64_t mask2 = masks[64 - q2];
        key[init + p1 - 1] = (key_1[init1 + p1 - 1] << q2) ^ (key_2[0] & mask2);
        for (int j = 0; j < p1 - 1; j++)
        {
            key[init + j] = ((key_1[init1 + j] << q2) ^ (key_1[init1 + j + 1] >> (64 - q2)));
        }
        key[init - 1] ^= (key_1[init1] >> (64 - q2));
        if (p1 > 0 && init1 == 1)
        {
            key[init - 1] ^= key_1[0] << q2;
        }
        if (init == 2)
        {
            key[0] = (key_1[0] & mask1) >> (64 - (q1 + q2 % 64));
        }
    }
};

void convert(const uint64_t* seed, uint64_t* g_seed, const int g_bits)
{
    if (g_bits <= 128)
    {
        g_seed[0] = seed[0];
        if (g_bits > 64)
        {
            g_seed[1] = (seed[1] & 0xfffffffffffffffeU);
        }
        else
        {
            g_seed[0] &= 0xfffffffffffffffeU;
        }
    }
    else
    // it is rarely the case, we will make sure that the output size is equal or lower than lambda
    {
        int n = g_bits / 64 ;
        uint64_t* g_seed0 = (uint64_t*) calloc(n , 64);
        if (g_seed0 == NULL) { exit(1); }
        uint64_t* g_seed1 = (uint64_t*) calloc(n , 64);
        if (g_seed1 == NULL) { exit(1); }

        int n0 = g_bits / 2 ;
        int n1 = n0;
        if (n0 % 2 == 1) { n1 += 1; }
        genG.seed(seed[0]);
        for (int i = 0; i < n; i++)
        {
            g_seed0[i] = genG();
        }
        genG.seed(seed[1]);
        for (int i = 0; i < n; i++)
        {
            g_seed1[i] = genG();
        }
        concatenate(g_seed0, n0, g_seed1, n1, g_seed);
        
        free(g_seed0); free(g_seed1); 
    }
}

void add(uint64_t* g1, const uint64_t* g2, const int p, const int q)
{
    uint64_t retenue = 0;
    int fin = p;
    int deb = 0;
    if (q != 0)
    {
        fin ++;
        deb ++;
    }
    for (int i = fin; i > deb; i--)
    {
        uint64_t temp = g1[i-1];
        g1[i-1] = temp + retenue + g2[i-1];
        if (g1[i-1] < temp)
        { retenue = 1; }
        else
        { retenue = 0; }   
    }
    if ( deb == 1)
    {
        g1[0] = (g1[0] + retenue + g2[0]) & masks[64 - q];
    }
}

void subtract(uint64_t* g1, const uint64_t* g2, const int p, const int q)
{
    uint64_t retenue = 0;
    int fin = p;
    int deb = 0;
    if (q != 0)
    {
        fin ++;
        deb ++;
    }
    for (int i = fin; i > deb; i--)
    {
        uint64_t temp = g1[i-1];
        g1[i-1] = temp - retenue - g2[i-1];
        if (g1[i-1] > temp)
        { retenue = 1; }
        else
        { retenue = 0; }    
    }
    if ( deb == 1)
    {
        g1[0] = (g1[0] - retenue - g2[0]) & masks[64 - q];
    }
    
}

void negative(uint64_t* g, const int p, const int q)
{
    uint64_t retenue = 1;
    int fin = p;
    int deb = 0;
    if (q != 0)
    {
        fin ++;
        deb ++;
    }
    if (g[fin-1] == 0)
    {
        retenue = 0;
    }
    g[fin-1] = - g[fin-1];
    for (int i = fin-1; i > deb; i--)
    {
        uint64_t temp = g[i-1];
        g[i-1] = - temp - retenue;
        if (retenue == 0 && (temp != 0))
        {
            retenue = 1;
        }  
    }
    if ( deb == 1)
    {
        g[0] = (- g[0] - retenue) & masks[64 - q];
    }
}

void get_final_CW(uint64_t* beta, const int beta_bits, const uint64_t* s0, const uint64_t* s1, const int t)
{
    int p = beta_bits / 64;
    int q = beta_bits % 64;
    int n = p ;
    if (q != 0) { n++; }

    uint64_t* g_s0 = (uint64_t*) calloc(n, 64);
    if (g_s0==NULL) { exit(1); }
    convert(s0, g_s0, beta_bits);

    uint64_t* g_s1 = (uint64_t*) calloc(n, 64);
    if (g_s1==NULL) { exit(1); }
    convert(s1, g_s1, beta_bits);

    subtract(beta, g_s0, p, q);
    add(beta, g_s1, p, q);
    if (t==1) 
    { 
        negative(beta, p, q);
    }

    free(g_s0); free(g_s1);
}


