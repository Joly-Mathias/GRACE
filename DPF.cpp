# include <random>
# include <ctime>
# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>

// # include <emmintrin.h>
// # include <smmintrin.h>

# include "AES.hpp"
# include "GROUP.hpp"

# define LAMBDA (127)
# define KEYBITS (128)

std::mt19937_64 gen;

uint64_t init_vect0[2];
uint64_t init_vect1[2];

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

void incrementation(uint64_t* init_vect)
{
    init_vect[1] += 1;
    if (init_vect[1] == 0)
    {
        init_vect[0] += 1;
    }
};

void decrementation(uint64_t* init_vect)
{
    if (init_vect[1] == 0)
    {
        init_vect[0] -= 1;
        init_vect[1] = 0xffffffffffffffffU;
    }
    else
    {
        init_vect[1] -=1;
    }
};

void doubleKey(const uint64_t* key_128, uint64_t* key_256)
{
    // Round Keys
    uint32_t rk[44];
    int nrounds = expandEncryptKey(rk, key_128, 128);

    // Initial vectors incrementation
    incrementation(init_vect0);
    incrementation(init_vect1);
  
    // AES encryption
    uint64_t cipher0[2];
    uint64_t cipher1[2];
    encryptAES(rk, nrounds, init_vect0, cipher0);
    encryptAES(rk, nrounds, init_vect1, cipher1);
  
    // AES(0) XOR s||0
    key_256[0] = cipher0[0] ^ key_128[0];
    key_256[1] = cipher0[1] ^ key_128[1];
    // AES(1) XOR s||0
    key_256[2] = (cipher1[0] ^ 0xffffffffffffffffU) ^ key_128[0];
    key_256[3] = (cipher1[1] ^ 0xffffffffffffffffU) ^ key_128[1];
};

void inv_doubleKey(const uint64_t* key_128, const uint64_t* key_256)
{
    // Round Keys
    uint32_t rk[44];
    int nrounds = expandEncryptKey(rk, key_128, 128);
  
    // AES encryption
    uint64_t cipher0[2];
    uint64_t cipher1[2];
    encryptAES(rk, nrounds, init_vect0, cipher0);
    encryptAES(rk, nrounds, init_vect1, cipher1);
  
    // Verification (all must be equal)
    std::cout << key_128[0] << ' ' << key_128[1] << std::endl;
    std::cout << (key_256[0] ^ cipher0[0]) << ' ' << (key_256[1] ^ cipher0[1]) << std::endl;
    std::cout << (key_256[2] ^ cipher1[0] ^ 0xffffffffffffffffU) << ' ' << (key_256[3] ^ cipher1[1] ^ 0xffffffffffffffffU) << std::endl;
};

void get_tLR(const uint64_t* sLR, int* tLR)
{
    tLR[0] = sLR[0] % 2; // tL
    tLR[1] = sLR[1] % 2; // tR
};

int get_tCW(const uint64_t* key, const int input_bits, const int round)
{
    int index = 2*(input_bits + 1) + (round / 32);
    int tLR = key[index] >> (2 * (31 - (round % 32)));
    return tLR % 4;
};

void put_tCW(uint64_t* key, const int input_bits, const int round, const int tLR)
{
    int index = 2*(input_bits + 1) + (round / 32);
    uint64_t factor = power2[2 * (32 - (round % 32))];
    key[index] += tLR * factor;
};

void Gen(const uint64_t seed, const uint64_t* alpha, const int alpha_bits, const uint64_t* beta, const int beta_bits, uint64_t* key_0, uint64_t* key_1)
{
    // Seeding mersene random generator
    gen.seed(seed);

    // Generating four random seeds
    key_gen(init_vect0);
    key_gen(init_vect1);
    uint64_t* s_0 = (uint64_t*) calloc(2 , 64);;
    if (s_0 == NULL) { exit(1); }
    key_gen(s_0);
    uint64_t* s_1 = (uint64_t*) calloc(2 , 64);;
    if (s_1 == NULL) { exit(1); }
    key_gen(s_0);
    
    // Initialisation
    int t[2] = {0, 0};
    key_0[0] = s_0[0];
    key_0[1] = s_0[1];
    key_1[0] = s_1[0];
    key_1[1] = s_1[1];

    // Declaration of variables
    int KEEP, LOSE; // L == 0 , R == 1
    int tLR_0[2];
    uint64_t* sLR_0 = (uint64_t*) calloc(4 , 64);;
    if (sLR_0 == NULL) { exit(1); }
    int tLR_1[2];
    uint64_t* sLR_1 = (uint64_t*) calloc(4 , 64);;
    if (sLR_1 == NULL) { exit(1); }
    int tCW;
    uint64_t* sCW = (uint64_t*) calloc(2 , 64);;
    if (sCW == NULL) { exit(1); }

    // Loop
    for (int i = 1; i <= alpha_bits; i++)
    {
        doubleKey(s_0, sLR_0);
        doubleKey(s_1, sLR_1);
        get_tLR(sLR_0, tLR_0);
        get_tLR(sLR_1, tLR_1);
        int inv_alpha_i = (alpha[i/64] & power2[64 - i] == 0); // alpha_i XOR 1

        if (inv_alpha_i == 1) { KEEP = 0; LOSE = 1; } else { KEEP = 1; LOSE = 0; } 

        sCW[0] = sLR_0[LOSE * 2] ^ sLR_1[LOSE * 2];
        sCW[1] = sLR_0[LOSE * 2 + 1] ^ sLR_1[LOSE * 2 + 1];
        // tCW sur 2 bits : tCW_L*2 + tCW_R
        tCW = 2*(tLR_0[0] ^ tLR_1[0] ^ inv_alpha_i) + tLR_0[1] ^ tLR_1[1] ^ inv_alpha_i ^ 1; 

        key_0[2*i] = sCW[0];
        key_0[2*i+1] = sCW[1];
        put_tCW(key_0, alpha_bits, i, tCW);

        key_1[2*i] = sCW[0];
        key_1[2*i+1] = sCW[1];
        put_tCW(key_0, alpha_bits, i, tCW);
    }

    // CW (n+1) 
    
};

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

    // Converting alpha from char to uint64_bit
    // Right block incomplete : 11010010 11101001 011XXXXX  
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

    // Converting beta from char to uint64_t 
    // Left block incomplete : XXXXX110 10010111 01001011 
    char* beta_char = argv[2];
    int beta_bits = strlen(beta_char);
    p = beta_bits / 64;
    q = beta_bits % 64;
    n = p ;
    if (q != 0) { n += 1; }
    uint64_t* beta = (uint64_t*) calloc(n, 64);
    if (beta==NULL) { exit(1); }
    for (int j = 0; j < q; j++)
    {
        temp = *beta_char != 0 ? *beta_char++ : 0;
        beta[0] ^= (uint64_t)temp << 8 * ( q - j);
    }
    for (int i = 0; i < p; i++)
    {
        for (int j = 0 ; j < 8 ; j++)
        {
            temp = *beta_char != 0 ? *beta_char++ : 0;
            beta[n - p + i] ^= (uint64_t)temp << (56 - 8 * j);
        }
    }


    // KEY REPRESENTATION (right block incomplete):
    // (alphabits + 1) * 127 bits in order to store the root seed and the correction word seeds sCW
    // 2*alpha_bits bits in order to store the values of tCW_L and tCW_R
    // beta_bits bits in order to store the value of CW(n+1)
    int n = (alpha_bits+1)*2 + ((2*alpha_bits) / 64) + p;
    if ((2*alpha_bits) % 64 != 0) { n += 1; }
    if (q != 0) { n += 1; }
    uint64_t* key_0 = (uint64_t*) calloc(n , 64);
    if (key_0 == NULL) { exit(1); }
    uint64_t* key_1 = (uint64_t*) calloc(n , 64);
    if (key_1 == NULL) { exit(1); }

    Gen(seed, alpha, alpha_bits, beta, beta_bits, key_0, key_1);

    free(key_0); free(key_1); free(beta); free(alpha);

    // TESTS :
    // // Seeding mersene random generator
    // gen.seed(seed);
    // key_gen(init_vect0);
    // key_gen(init_vect1);

    // std::cout << std::endl;
    // std::cout << "VALEURS" << std::endl;
    // std::cout << "vecteur initialisation 0 " << init_vect0[0] << ' ' << init_vect0[1] << std::endl;
    // std::cout << "vecteur initialisation 1 " << init_vect0[0] << ' ' << init_vect0[1] << std::endl;

    // uint64_t* key_127 = (uint64_t*) calloc(2 , 64);;
    // if (key_127 == NULL) { exit(1); }
    // key_gen(key_127);
    // std::cout << "clef 127 bits : " << key_127[0] << ' ' << key_127[1] << std::endl;

    // uint64_t* key_256 = (uint64_t*) calloc(4 , 64);
    // if (key_256 == NULL) { exit(1); }

    // std::cout << std::endl;
    // std::cout << "EXPANSION DE CLEF" << std::endl;
    // doubleKey(key_127, key_256);
    // std::cout << "clef 256 bits : " << key_256[0] << ' ' << key_256[1] << ' ' << key_256[2] << ' ' << key_256[3] << std::endl;
    // std::cout << std::endl;
    // std::cout << "VERIFICATION VALEURS" << std::endl;
    // std::cout << "vecteur initialisation 0 " << init_vect0[0] << ' ' << init_vect0[1] << std::endl;
    // std::cout << "vecteur initialisation 1 " << init_vect0[0] << ' ' << init_vect0[1] << std::endl;
    // std::cout << "clef 127 bits : " << key_127[0] << ' ' << key_127[1] << std::endl;
    // std::cout << std::endl;
    // std::cout << "DECLIN DE CLEF" << std::endl;
    // inv_doubleKey(key_127, key_256);
    // std::cout << std::endl;
    // std::cout << "VERIFICATION VALEURS" << std::endl;
    // std::cout << "vecteur initialisation 0 " << init_vect0[0] << ' ' << init_vect0[1] << std::endl;
    // std::cout << "vecteur initialisation 1 " << init_vect0[0] << ' ' << init_vect0[1] << std::endl;
    // std::cout << "clef 127 bits : " << key_127[0] << ' ' << key_127[1] << std::endl;
    
    // free(key_127); free(key_256);

    return 0;
}