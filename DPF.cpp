# include <random>
# include <ctime>
# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>

// # include <emmintrin.h>
// # include <smmintrin.h>

# include "AES.hpp"
# include "PRG.hpp"

# define LAMBDA (127)

std::mt19937_64 gen;

uint64_t power2[64] =
{
    0x0000000000000001U, 0x0000000000000002U, 0x0000000000000004U, 0x0000000000000008U,
    0x0000000000000010U, 0x0000000000000020U, 0x0000000000000040U, 0x0000000000000080U,
    0x0000000000000100U, 0x0000000000000200U, 0x0000000000000400U, 0x0000000000000800U,
    0x0000000000001000U, 0x0000000000002000U, 0x0000000000004000U, 0x0000000000008000U,
    0x0000000000010000U, 0x0000000000020000U, 0x0000000000040000U, 0x0000000000080000U,
    0x0000000000100000U, 0x0000000000200000U, 0x0000000000400000U, 0x0000000000800000U,
    0x0000000001000000U, 0x0000000002000000U, 0x0000000004000000U, 0x0000000008000000U,
    0x0000000010000000U, 0x0000000020000000U, 0x0000000040000000U, 0x0000000080000000U,
    0x0000000100000000U, 0x0000000200000000U, 0x0000000400000000U, 0x0000000800000000U,
    0x0000001000000000U, 0x0000002000000000U, 0x0000004000000000U, 0x0000008000000000U,
    0x0000010000000000U, 0x0000020000000000U, 0x0000040000000000U, 0x0000080000000000U,
    0x0000100000000000U, 0x0000200000000000U, 0x0000400000000000U, 0x0000800000000000U,
    0x0001000000000000U, 0x0002000000000000U, 0x0004000000000000U, 0x0008000000000000U,
    0x0010000000000000U, 0x0020000000000000U, 0x0040000000000000U, 0x0080000000000000U,
    0x0100000000000000U, 0x0200000000000000U, 0x0400000000000000U, 0x0800000000000000U,
    0x1000000000000000U, 0x2000000000000000U, 0x4000000000000000U, 0x8000000000000000U, 
};

void key_gen(uint64_t* seed_128)
{
    seed_128[0] = gen();
    seed_128[1] = gen() & 0xfffffffffffffffeU;
    // The seed is actually 127 bits, so one bit is free
    // We set the last bit at 0 to facilitate concatenation
};

void Gen(const uint64_t seed, const uint64_t* alpha, const int alpha_bits, const uint64_t* beta, const int beta_bits, uint64_t* key_0, uint64_t* key_1)
{
    // // Seeding mersene random generator
    // gen.seed(seed);

    // // Initialisation
    // uint64_t* s_0 = (uint64_t*) calloc(2 , 64);;
    // if (s_0 == NULL) { exit(1); }
    // key_gen(s_0);
    // int t0 = 0;
    // uint64_t* s_1 = (uint64_t*) calloc(2 , 64);;
    // if (s_1 == NULL) { exit(1); }
    // key_gen(s_0);
    // int t1 = 1;

    // key_0[0] = s_0[0];
    // key_0[1] = s_0[1];
    // key_1[0] = s_1[0];
    // key_1[1] = s_1[1];
    // int key_bits = 127;

    // uint64_t* sCW = (uint64_t*) calloc(2 , 64);;
    // if (sCW == NULL) { exit(1); }
    // int tCW[2]; // L and R

    // int KEEP, LOSE; // L == 0 , R == 1
    // uint64_t* sLR_0 = (uint64_t*) calloc(4 , 64);;
    // if (sLR_0 == NULL) { exit(1); }
    // uint64_t* sLR_1 = (uint64_t*) calloc(4 , 64);;
    // if (sLR_1 == NULL) { exit(1); }
    // uint64_t* sLR_0 = (uint64_t*) calloc(4 , 64);;
    // if (sLR_0 == NULL) { exit(1); }

    // // Loop
    // for (int i = 1; i <= alpha_bits; i++)
    // {
    //     doubleKey(s_0, sLR_0);
    //     doubleKey(s_1, sLR_1);
    //     int inv_alpha_i = (alpha[i/64] & power2[64 - i] == 0); // alpha_i XOR 1
    //     if (inv_alpha_i == 1) { KEEP = 0; LOSE = 1; } else { KEEP = 1; LOSE = 0; } 
    //     sCW[0] = sLR_0[LOSE * 2] ^ sLR_1[LOSE * 2];
    //     sCW[1] = sLR_0[LOSE * 2 + 1] ^ sLR_1[LOSE * 2 + 1];



    // }
    



    
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cout << "Missing argument\n" << std::endl;
        return 1;
    }

    // Converting seed from char to uint64_bit
    // The seed is composed of 8 ASCII characters stored on 8 bits
    char* seed_char = argv[1];
    uint64_t seed = 0x0000000000000000U;
    uint8_t temp;
    for (int i = 0; i < 8; i++)
    {
        temp = *seed_char != 0 ? *seed_char++ : 0;
        seed ^= (uint64_t)temp << (56 - 8 * i);
        // std::cout << seed << std::endl;
    }

    // Converting alpha from char to uint8_bit
    char* alpha_char = argv[2];
    int alpha_bits = strlen(alpha_char);
    int p = alpha_bits / 64;
    int q = alpha_bits % 64;
    int n = p ;
    if (q != 0) { n += 1; }
    uint64_t* alpha = (uint64_t*) calloc(n, 64);
    if (alpha==NULL) { exit(1); }
    for (int i = 0; i < p; i++)
    {
        for (int j = 0 ; j < 8 ; j++)
        {
            temp = *alpha_char != 0 ? *alpha_char++ : 0;
            alpha[i] ^= (uint64_t)temp << (56 - 8 * j);
        }
    }
    for (int j = 0; j < q; j++)
    {
        temp = *alpha_char != 0 ? *alpha_char++ : 0;
        alpha[p] ^= (uint64_t)temp << (56 - 8 * j);
    }

    // Converting beta from char to uint8_bit
    char* beta_char = argv[2];
    int beta_bits = strlen(beta_char);
    p = beta_bits / 64;
    q = beta_bits % 64;
    n = p ;
    if (q != 0) { n += 1; }
    uint64_t* beta = (uint64_t*) calloc(n, 64);
    if (beta==NULL) { exit(1); }
    for (int i = 0; i < p; i++)
    {
        for (int j = 0 ; j < 8 ; j++)
        {
            temp = *beta_char != 0 ? *beta_char++ : 0;
            beta[i] ^= (uint64_t)temp << (56 - 8 * j);
        }
    }
    for (int j = 0; j < q; j++)
    {
        temp = *beta_char != 0 ? *beta_char++ : 0;
        beta[p] ^= (uint64_t)temp << (56 - 8 * j);
    }

    int key_bits = alpha_bits*(LAMBDA + 2) + LAMBDA + beta_bits;
    p = key_bits / 64;
    q = key_bits % 64;
    n = p ;
    if (q != 0) { n += 1; }
    uint64_t* key_0 = (uint64_t*) calloc(n , 64);
    if (key_0 == NULL) { exit(1); }
    uint64_t* key_1 = (uint64_t*) calloc(n , 64);
    if (key_1 == NULL) { exit(1); }

    Gen(seed, alpha, alpha_bits, beta, beta_bits, key_0, key_1);

    free(key_0); free(key_1); free(beta); free(alpha);

    TESTS :
    // Seeding mersene random generator
    gen.seed(seed);
    uint64_t* a = (uint64_t*) calloc(2 , 64);;
    if (a == NULL) { exit(1); }
    key_gen(a);
    uint64_t* b = (uint64_t*) calloc(2 , 64);
    if (b == NULL) { exit(1); }
    key_gen(b);

    std::cout << std::endl;
    std::cout << "avant : " << a[0] << ' ' << a[1] << std::endl;
    std::cout << "avant : " << b[0] << ' ' << b[1] << std::endl;

    std::cout << std::endl;
    uint64_t* c = (uint64_t*) calloc(4 , 64);;
    if (c == NULL) { exit(1); }
    concatenateKey(a, 128, b, 128, c);

    std::cout << std::endl;
    uint64_t* d = (uint64_t*) calloc(4 , 64);;
    if (d == NULL) { exit(1); }
    concatenateKey(a, 127, b, 127, d);

    std::cout << std::endl;
    uint64_t* e = (uint64_t*) calloc(4 , 64);;
    if (e == NULL) { exit(1); }
    concatenateKey(a+1, 63, b, 127, e+1);

    std::cout << std::endl;
    std::cout << "apres : " << a[0] << ' ' << a[1] << std::endl;
    std::cout << "apres : " << b[0] << ' ' << b[1] << std::endl;
    std::cout << std::endl;
    std::cout << "externe : " << c[0] << ' ' << c[1] << ' ' << c[2] << ' ' << c[3] << std::endl;
    std::cout << "externe : " << d[0] << ' ' << d[1] << ' ' << d[2] << ' ' << d[3] << std::endl;
    std::cout << "externe : " << e[0] << ' ' << e[1] << ' ' << e[2] << ' ' << e[3] << std::endl;
    
    free(a); free(b); free(c); free(d); free(e);

    // uint64_t f[2];
    // f[0] = 0xffffffffffffffffU;
    // f[1] = 0x1111111100000000U;
    // uint64_t g[2];
    // g[0] = 0xaaaaaaaaaaaaaaaaU;
    // g[1] = 0xcccccccc00000000U;
    // uint64_t h[3];
    // concatenateKey(f, 96, g, 96, h);

    // std::cout << std::endl;
    // std::cout << f[0] << ' ' << f[1] << std::endl;
    // std::cout << g[0] << ' ' << g[1] << std::endl;
    // std::cout << h[0] << ' ' << h[1] << ' ' << h[2] << std::endl;
    // std::cout << std::endl;

    return 0;
}