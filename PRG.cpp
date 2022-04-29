# include <iostream>
# include <cstdlib>
# include <cstdio>

# include "AES.hpp"
# include "PRG.hpp"

#define KEYBITS 128

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

void zeros_gen(uint64_t* key)
{
    key[0] = 0x0000000000000000U;
    key[1] = 0x0000000000000000U;
}

void ones_gen(uint64_t* key)
{
    key[0] = 0xffffffffffffffffU;
    key[1] = 0xffffffffffffffffU;
}

void concatenateKey(const uint64_t* key_1, const int n1, const uint64_t* key_2, const int n2, uint64_t* key)
{
    int p1 = n1 / 64;
    int q1 = n1 % 64;
    int p2 = n2 / 64;
    int q2 = n2 % 64;

    for (int i = 0; i < p1; i++)
    {
        key[i] = key_1[i];
    }
    if (q1 == 0 )
    {
        for (int j = 0 ; j < p2; j++)
        {
            key[p1 + j] = key_2[j];
        }
        if (q2 > 0)
        {
            key[p1 + p2] = key_2[p2];
        }
    }
    else
    {
        uint64_t mask = masks[64 - q1];
        key[p1] = key_1[p1] + (key_2[0] >> q1);
        for (int j = 0 ; j < p2; j++)
        {
            key[p1 + j + 1] = ((key_2[j] & mask) << (64 - q1)) ^ (key_2[j+1] >> q1);
        }
        if (q2 + q1 > 64)
        {
            key[p1 + p2 + 1] = (key_2[p2] & mask) << (64 - q1);
        }
    }
}

void doubleKey(const uint64_t* key_128, uint64_t* key_256)
{
    // Round Keys
    uint32_t rk[44];
    int nrounds = expandEncryptKey(rk, key_128, 128);

    // Input generation
    uint64_t plain0[2];
    uint64_t plain1[2];
    zeros_gen(plain0);
    ones_gen(plain1);
    // print_uint8(plain0);
    // print_uint8(plain1);
  
    // AES encryption
    uint64_t cipher0[2];
    uint64_t cipher1[2];
    encryptAES(rk, nrounds, plain0, cipher0);
    encryptAES(rk, nrounds, plain1, cipher1);
  
    // Key expansion
    for (int i = 0; i < 16; i++)
    {
        key_256[i] = cipher0[i] ^ key_128[i];
        key_256[16 + i] = cipher1[i] ^ key_128[i];
    };
}