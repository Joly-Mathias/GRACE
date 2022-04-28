# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <random>
# include <ctime>

# include "AES.hpp"

#define KEYBITS 128

void seed_gen(uint8_t* key)
{
    srand(time(NULL));
    for (int i = 0 ; i < 15 ; i ++)
    {
        key[i] = rand()%256;
    }
    key[15] = (rand()%128) << 1;
}

void zeros_gen(uint8_t* key)
{
    for (int i = 0 ; i < 16 ; i ++)
    {
        key[i] = 0x00U;
    }
}

void ones_gen(uint8_t* key)
{
    for (int i = 0 ; i < 16 ; i ++)
    {
        key[i] = 0xffU;
    }
}

void print_uint8(uint8_t* key)
{
    for (int i = 0 ; i < 16; i ++)
    {
        std::cout << (int) key[i] << ' ';
    }
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    // Random seed generation
    uint32_t rk[44];
    uint8_t key[16];
    seed_gen(key);
    // print_uint8(key);
    int nrounds = expandEncryptKey(rk, key, 128);

    // Input generation
    uint8_t plain0[16];
    uint8_t plain1[16];
    zeros_gen(plain0);
    ones_gen(plain1);
    // print_uint8(plain0);
    // print_uint8(plain1);
  
    // AES encryption
    uint8_t cipher0[16];
    uint8_t cipher1[16];
    encryptAES(rk, nrounds, plain0, cipher0);
    encryptAES(rk, nrounds, plain1, cipher1);
  
    // Pseudo random key expansion
    uint8_t cipher[32];
    for (int i = 0; i < 16; i++)
    {
        cipher[i] = cipher0[i] ^ key[i];
        cipher[16 + i] = cipher1[i] ^ key[i];
    };
    // print_uint8(cipher);
    // print_uint8(cipher+16);

    // Verification
    nrounds = expandDecryptKey(rk, key, 128);
    decryptAES(rk, nrounds, cipher0, plain0);
    decryptAES(rk, nrounds, cipher1, plain1);
    print_uint8(plain0);
    print_uint8(plain1);
}