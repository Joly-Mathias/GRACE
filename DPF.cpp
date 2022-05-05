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

uint64_t power2[65] =
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

void doubleKey(const uint64_t* key_128, uint64_t* key_256, uint64_t* init_vect)
{
    // Round Keys
    uint32_t rk[44];
    int nrounds = expandEncryptKey(rk, key_128, 128);    
  
    // AES encryption
    uint64_t cipher0[2];
    incrementation(init_vect);
    encryptAES(rk, nrounds, init_vect, cipher0);
    uint64_t cipher1[2];
    incrementation(init_vect);
    encryptAES(rk, nrounds, init_vect, cipher1);
  
    // AES(0) XOR s||0
    key_256[0] = cipher0[0] ^ key_128[0];
    key_256[1] = cipher0[1] ^ key_128[1];
    // AES(1) XOR s||0
    key_256[2] = (cipher1[0] ^ 0xffffffffffffffffU) ^ key_128[0];
    key_256[3] = (cipher1[1] ^ 0xffffffffffffffffU) ^ key_128[1];
};

void inv_doubleKey(const uint64_t* key_128, const uint64_t* key_256, uint64_t* init_vect)
{
    // Round Keys
    uint32_t rk[44];
    int nrounds = expandEncryptKey(rk, key_128, 128);
  
    // AES encryption
    uint64_t cipher0[2];
    encryptAES(rk, nrounds, init_vect, cipher0);
    decrementation(init_vect);
    uint64_t cipher1[2];
    encryptAES(rk, nrounds, init_vect, cipher1);
    decrementation(init_vect);
  
    // Verification (all must be equal)
    std::cout << key_128[0] << ' ' << key_128[1] << std::endl;
    std::cout << (key_256[0] ^ cipher0[0]) << ' ' << (key_256[1] ^ cipher0[1]) << std::endl;
    std::cout << (key_256[2] ^ cipher1[0] ^ 0xffffffffffffffffU) << ' ' << (key_256[3] ^ cipher1[1] ^ 0xffffffffffffffffU) << std::endl;
};

void get_tLR(const uint64_t* sLR, int* tLR)
{
    tLR[0] = sLR[1] % 2; // tL
    tLR[1] = sLR[3] % 2; // tR
};

void get_tCW(const uint64_t* key, const int input_bits, const int round, int* tCW)
{
    int index = 2 + 2*(input_bits + 1) + (round / 32);
    uint64_t temp = key[index] >> (2 * (31 - (round % 32)));
    tCW[0] = (temp % 4) / 2;
    tCW[1] = temp % 2;
};

void put_tCW(uint64_t* key, const int input_bits, const int round, const int tCW)
{
    int index = 2 + 2*(input_bits + 1) + (round / 32);
    uint64_t factor = power2[2 * (31 - (round % 32))];
    key[index] += tCW * factor;
    if (round == 64) 
    { 
        std::cout << key[index-1] << " " << key[index] << " = " << (tCW * factor) << std::endl;
    }
};

void Gen(const uint64_t seed, const uint64_t* alpha, const int alpha_bits, uint64_t* beta, const int beta_bits, uint64_t* key_0, uint64_t* key_1)
{
    // Seeding mersene random generator
    gen.seed(seed);

    // Generating four random seeds
    uint64_t* init_vect0 = (uint64_t*) calloc(2 , 64);;
    if (init_vect0 == NULL) { exit(1); }
    key_gen(init_vect0);
    uint64_t* init_vect1 = (uint64_t*) calloc(2 , 64);;
    if (init_vect1 == NULL) { exit(1); }
    key_gen(init_vect1);
    uint64_t* s_0 = (uint64_t*) calloc(2 , 64);;
    if (s_0 == NULL) { exit(1); }
    key_gen(s_0);
    uint64_t* s_1 = (uint64_t*) calloc(2 , 64);;
    if (s_1 == NULL) { exit(1); }
    key_gen(s_1);

    // std::cout << "T_0 : 0 1" << std::endl;

    // Initialisation
    int t[2] = {0, 1};
    key_0[0] = init_vect0[0];
    key_0[1] = init_vect0[1];
    key_0[2] = s_0[0];
    key_0[3] = s_0[1];
    key_1[0] = init_vect1[0];
    key_1[1] = init_vect1[1];
    key_1[2] = s_1[0];
    key_1[3] = s_1[1];

    // Declaration of variables : L == 0 , R == 1
    int KEEP, LOSE;
    int tLR_0[2];
    uint64_t* sLR_0 = (uint64_t*) calloc(4 , 64);;
    if (sLR_0 == NULL) { exit(1); }
    int tLR_1[2];
    uint64_t* sLR_1 = (uint64_t*) calloc(4 , 64);;
    if (sLR_1 == NULL) { exit(1); }
    int tCW[2];
    uint64_t* sCW = (uint64_t*) calloc(2 , 64);;
    if (sCW == NULL) { exit(1); }

    // Loop
    for (int i = 0; i < alpha_bits; i++)
    {
        // Seed expansion of s||0
        s_0[1] &= 0xfffffffffffffffeU;
        s_1[1] &= 0xfffffffffffffffeU;

        // if (i == 63)
        // {
        //     std::cout << std::endl;
        //     std::cout << "SEED " << i << " de 0" << std::endl;
        //     std::cout << s_0[0] << ' ' << s_0[1] << std::endl;
        //     std::cout << "T :" << t[0] << std::endl;
        //     std::cout << "T_CW :" << tCW[0] << ' ' << tCW[1] << std::endl;
        //     std::cout << "S_CW :" << sCW[0] << ' ' << (sCW[1] & 0xfffffffffffffffeU) << std::endl;
        //     std::cout << std::endl;
        //     std::cout << "SEED " << i << " de 1" << std::endl;
        //     std::cout << s_1[0] << ' ' << s_1[1] << std::endl;
        //     std::cout << "T :" << t[1] << std::endl;
        //     std::cout << "T_CW :" << tCW[0] << ' ' << tCW[1] << std::endl;
        //     std::cout << "S_CW :" << sCW[0] << ' ' << (sCW[1] & 0xfffffffffffffffeU) << std::endl;
        // }

        doubleKey(s_0, sLR_0, init_vect0);
        doubleKey(s_1, sLR_1, init_vect1);
        get_tLR(sLR_0, tLR_0);
        get_tLR(sLR_1, tLR_1);

        // if (i == 1) 
        // {
        //     std::cout << "0) " << sLR_0[0] << ' ' << sLR_0[1] << ' ' << sLR_0[2] << ' ' << sLR_0[3] << std::endl;
        //     std::cout << "1) " << sLR_1[0] << ' ' << sLR_1[1] << ' ' << sLR_1[2] << ' ' << sLR_1[3] << std::endl;
        // }
        // if (i > alpha_bits - 2) 
        // {
        //     std::cout << "0) " << tLR_0[0] << ' ' << tLR_0[1] << std::endl;
        //     std::cout << "1) " << tLR_1[0] << ' ' << tLR_1[1] << std::endl;
        //     std::cout << "T_CW : " << tCW[0] << ' ' << tCW[1] << std::endl;
        // }

        // Variables
        int inv_alpha_i = ((alpha[i/64] & power2[63 - (i % 64)]) == 0); // alpha_i XOR 1
        if (inv_alpha_i == 1) { KEEP = 0; LOSE = 1; } else { KEEP = 1; LOSE = 0; } 
        // if (i > alpha_bits - 3)
        // {
        //     std::cout << std::endl;
        //     std::cout << "alpha " << i << " : "  << (1 - inv_alpha_i) << std::endl;
        // }

        // Correcting words
        sCW[0] = sLR_0[LOSE * 2] ^ sLR_1[LOSE * 2];
        sCW[1] = sLR_0[LOSE * 2 + 1] ^ sLR_1[LOSE * 2 + 1];
        tCW[0] = tLR_0[0] ^ tLR_1[0] ^ inv_alpha_i;
        tCW[1] = tLR_0[1] ^ tLR_1[1] ^ inv_alpha_i ^ 1; 

        // Store Correcting  Words
        key_0[2*(i+2)] = sCW[0];
        key_0[2*(i+2)+1] = sCW[1];
        put_tCW(key_0, alpha_bits, i, tCW[0]*2 + tCW[1]);
        key_1[2*(i+2)] = sCW[0];
        key_1[2*(i+2)+1] = sCW[1];
        put_tCW(key_1, alpha_bits, i, tCW[0]*2 + tCW[1]);

        // if (i > alpha_bits - 2)
        // {
        //     std::cout << std::endl << "ROUND " << i  << " : " << power2[2 * (31 - ((i - 1) % 32))] << " : " << 2 * (31 - ((i - 1) % 32)) << std::endl;
        //     int temp[2];
        //     get_tCW(key_0, alpha_bits, i, temp);
        //     std::cout << "T_CW : " << temp[0] << ' ' << temp[1] << " : KEY_0" << std::endl;
        //     get_tCW(key_1, alpha_bits, i, temp);
        //     std::cout << "T_CW : " << temp[0] << ' ' << temp[1] << " : KEY_1" << std::endl;
        //     std::cout << "T_CW : " << tCW[0] << ' ' << tCW[1] << std::endl;
        // }

        // if (i == 64)
        // {
        //     std::cout << std::endl;
        //     std::cout << "sLR final 0 : " << sLR_0[0] << ' ' << sLR_0[1] << ' ' << sLR_0[2] << ' ' << sLR_0[3] << std::endl;
        //     std::cout << "sLR final 1 : " << sLR_1[0] << ' ' << sLR_1[1] << ' ' << sLR_1[2] << ' ' << sLR_1[3] << std::endl;
        //     int temp[2];
        //     get_tCW(key_0, alpha_bits, i, temp);
        //     std::cout << "tCW final 0 : " << temp[0] << ' ' << temp[1] << std::endl;
        //     get_tCW(key_1, alpha_bits, i, temp);
        //     std::cout << "tCW final 1 : " << temp[0] << ' ' << temp[1] << std::endl;
        // }
        // New seeds
        s_0[0] = sLR_0[KEEP * 2];
        s_0[1] = sLR_0[KEEP * 2 + 1];
        if(t[0] == 1)
        {
            s_0[0] ^= sCW[0];
            s_0[1] ^= sCW[1];
            t[0] = tCW[KEEP] ;
        }
        t[0] ^= tLR_0[KEEP];
        s_1[0] = sLR_1[KEEP * 2];
        s_1[1] = sLR_1[KEEP * 2 + 1];
        if(t[1] == 1)
        {
            s_1[0] ^= sCW[0];
            s_1[1] ^= sCW[1];
            t[1] = tCW[KEEP] ;
        }
        t[1] ^= tLR_1[KEEP];
    }

    // std::cout << std::endl;
    // std::cout << "SEED final de 0" << std::endl;
    // std::cout << s_0[0] << ' ' << s_0[1] << std::endl;
    // std::cout << "T : " << t[0] << std::endl;
    // std::cout << "T_CW :" << tCW[0] << ' ' << tCW[1] << std::endl;
    // std::cout << "S_CW :" << sCW[0] << ' ' << (sCW[1] & 0xfffffffffffffffeU) << std::endl;
    // std::cout << std::endl;
    // std::cout << "SEED final de 1" << std::endl;
    // std::cout << s_1[0] << ' ' << s_1[1] << std::endl;
    // std::cout << "T : " << t[1] << std::endl;
    // std::cout << "T_CW :" << tCW[0] << ' ' << tCW[1] << std::endl;
    // std::cout << "S_CW :" << sCW[0] << ' ' << (sCW[1] & 0xfffffffffffffffeU) << std::endl;

    get_final_CW(beta, beta_bits, s_0, s_1, t[1]);
    
    int deb = 2 + (alpha_bits+1)*2 + ((2*alpha_bits) / 64);
    if ((2*alpha_bits) % 64 != 0) { deb ++; }
    int fin = deb + (beta_bits / 64);
    if (beta_bits % 64 != 0) { fin ++; }
    for (int i = deb; i < fin; i++)
    {
        uint64_t temp = beta[i-deb];
        // std::cout << "CW final " << temp << std::endl;
        key_0[i] = temp;
        key_1[i] = temp;
    }

    std::cout << std::endl;
    std::cout << "fin KEY 0 : " << key_0[fin - 3] << ' ' << key_0[fin - 2] << ' ' << key_0[fin - 1] << std::endl;
    std::cout << "BETA 0 : " << beta[0] << ' ' << beta[1] << std::endl;
    std::cout << "fin KEY 1 : " << key_1[fin - 3] << ' ' << key_1[fin - 2] << ' ' << key_1[fin - 1] << std::endl;
    std::cout << "BETA 1 : " << beta[0] << ' ' << beta[1] << std::endl;

    free(s_0); free(s_1); free(init_vect0); free(init_vect1); free(sLR_0); free(sLR_1); free(sCW);
};

void Eval(const uint64_t* input, const int input_bits, uint64_t* output, const int output_bits, const uint64_t* key, const int b)
{
    // Initialisation
    int t = b;
    uint64_t* init_vect = (uint64_t*) calloc(2 , 64);
    if (init_vect == NULL) { exit(1); };
    uint64_t* s = (uint64_t*) calloc(2 , 64);
    if (s == NULL) { exit(1); }
    init_vect[0] = key[0];
    init_vect[1] = key[1];
    s[0] = key[2];
    s[1] = key[3];

    // std::cout << "T_0 " << b << " : " << t << std::endl;

    // Declaration of variables
    int tCW[2];
    uint64_t* sCW = (uint64_t*) calloc(2 , 64);;
    if (sCW == NULL) { exit(1); }
    uint64_t* sLR = (uint64_t*) calloc(4 , 64);;
    if (sLR == NULL) { exit(1); }

    // Loop
    for (int i = 0; i < input_bits; i++)
    {
        // Seed expansion of s||0
        s[1] &= 0xfffffffffffffffeU;
        // if (i == 63)
        // {
        //     std::cout << std::endl;
        //     std::cout << "SEED " << i << " de " << b << std::endl;
        //     std::cout << s[0] << ' ' << s[1] << std::endl;
        //     std::cout << "T :" << t << std::endl;
        //     std::cout << "T_CW :" << tCW[0] << ' ' << tCW[1] << std::endl;
        //     std::cout << "S_CW :" << sCW[0] << ' ' << (sCW[1] & 0xfffffffffffffffeU) << std::endl;
        // }

        // Correcting Words
        sCW[0] = key[2*(i+2)];
        sCW[1] = key[2*(i+2)+1];
        get_tCW(key, input_bits, i, tCW);

        // if (i < 3) 
        // {
        //     std::cout << b << ") " << sLR[0] << ' ' << sLR[1] << ' ' << sLR[2] << ' ' << sLR[3] << std::endl;
        //     std::cout << b << ") " << (sLR[1] % 2) << ' ' << (sLR[3] % 2) << std::endl;
        //     std::cout << b << ") " << ((sLR[1] % 2) ^ tCW[0]) << ' ' << ((sLR[3] % 2) ^tCW[1]) << std::endl;
        // }

        doubleKey(s, sLR, init_vect);

        // if (i == 64)
        // {
        //     std::cout << std::endl;
        //     std::cout << "sLR final " << b << " : " << sLR[0] << ' ' << sLR[1] << ' ' << sLR[2] << ' ' << sLR[3] << std::endl;
        // }

        if (t==1)
        {
            sLR[0] ^= sCW[0];
            sLR[1] ^= (sCW[1] & 0xfffffffffffffffeU) ^ tCW[0];
            sLR[2] ^= sCW[0];
            sLR[3] ^= (sCW[1] & 0xfffffffffffffffeU) ^ tCW[1];
        }

        int inv_input_i = ((input[i/64] & power2[63 - (i % 64)]) == 0); // input_i XOR 1
        // if (i > input_bits - 3)
        // {
        //     std::cout << std::endl;
        //     std::cout << "alpha " << i << " : "  << (1 - inv_input_i) << std::endl;
        // }

        // if (i == 64)
        // {
        //     std::cout << "sLR XOR sCW final " << b << " : " << sLR[0] << ' ' << sLR[1] << ' ' << sLR[2] << ' ' << sLR[3] << std::endl;
        // }
        if (inv_input_i == 1)
        {
            // KEEP = 0, LOSE == 1
            s[0] = sLR[0];
            s[1] = sLR[1];
            t = sLR[1] % 2;
        }
        else
        {
            // KEEP = 0, LOSE == 1
            s[0] = sLR[2];
            s[1] = sLR[3];
            t = sLR[3] % 2;
        }   

    }

    // std::cout << std::endl;
    // std::cout << "SEED final de " << b << std::endl;
    // std::cout << s[0] << ' ' << s[1] << std::endl;
    // std::cout << "T : " << t << std::endl;
    // std::cout << "T_CW :" << tCW[0] << ' ' << tCW[1] << std::endl;
    // std::cout << "S_CW :" << sCW[0] << ' ' << (sCW[1] & 0xfffffffffffffffeU) << std::endl;
    convert(s, output, output_bits);
    // std::cout << std::endl;
    // std::cout << "GSEED" << b << std::endl;
    // for (int i = 0; i < p; i++)
    // {
    //     std::cout << output[i] << ' ';
    // }
    // std::cout << std::endl;

    int p = output_bits / 64;
    int q = output_bits % 64;
    int deb = 2 + (input_bits+1)*2 + ((2*input_bits) / 64);
    int fin = deb + p;
    if ((2 * input_bits) % 64 != 0) { deb++; }
    if (q != 0) { fin ++; }
    if (t == 1)
    {
        add(output, (key + deb), p, q);
        // std::cout << std::endl;
        // std::cout << "CW + GSEED" << b << std::endl;
        // for (int i = 0; i < p; i++)
        // {
        //     std::cout << output[i] << ' ';
        // }
        // std::cout << std::endl;
    }
    if (b == 1)
    {
        negative(output, p, q);
        // std::cout << std::endl;
        // std::cout << "- 1 * ( 'CW +' GSEED" << b << ")" << std::endl;
        // for (int i = 0; i < p; i++)
        // {
        //     std::cout << output[i] << ' ';
        // }
        // std::cout << std::endl;
    }

    for (int i = deb; i < fin; i++)
    {
        uint64_t temp = output[i-deb];
        // std::cout << "CW final de " << b << " " << temp << std::endl;
    }

    std::cout << std::endl;
    std::cout << "fin KEY " << b << " : " << key[fin - 3] << ' ' << key[fin - 2] << ' ' << key[fin - 1] << std::endl;
    std::cout << "BETA " << b << " : " << output[0] << ' ' << output[1] << std::endl;

    free(s); free(init_vect); free(sLR); free(sCW);
}

int main(int argc, char **argv)
{
    // char mode = '0'; // NULL
    // if (argc == 4) { mode = 'G'; } // Gen
    // if (argc == 5) { mode = 'E'; } // Eval
    // if (mode == '0') { std::cout << "Wrong number of arguments" << std::endl; return 1; }

    // Converting seed from char to uint64_bit
    // The seed is composed of 8 ASCII characters stored on 8 bits
    char* seed_char = argv[1];
    uint64_t seed = 0x0000000000000000U;
    uint8_t temp;
    for (int i = 0; i < 8; i++)
    {
        temp = *seed_char != 0 ? *seed_char++ : 0;
        seed ^= (uint64_t)temp << (56 - 8 * i);
    }

    // Converting alpha from char to uint64_bit
    // Right block incomplete : 11010010 11101001 011XXXXX  
    char* alpha_char = argv[2];
    int alpha_bits = strlen(alpha_char)*8;
    int p = alpha_bits / 64;
    int q = alpha_bits % 64;
    int n1 = p ;
    if (q != 0) { n1 += 1; }
    uint64_t* alpha = (uint64_t*) calloc(n1, 64);
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
    char* beta_char = argv[3];
    int beta_bits = strlen(beta_char)*8;
    p = beta_bits / 64;
    q = beta_bits % 64;
    int n2 = p ;
    if (q != 0) { n2 += 1; }
    uint64_t* beta = (uint64_t*) calloc(n2, 64);
    if (beta==NULL) { exit(1); }
    std::cout << std::endl;
    std::cout << " - - - BETA - - -" << std::endl;
    std::cout << std::endl;
    std::cout << beta_char << std::endl;
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
            beta[n2 - p + i] ^= (uint64_t)temp << (56 - 8 * j);
        }
    }
    for (int i = 0; i < n2; i++)
    {
        std::cout << beta[i] << ' ';
    }
    std::cout << std::endl;

    // KEY REPRESENTATION (right block incomplete):
    // 128 bits for the initial vector
    // (alphabits + 1) * 127 bits in order to store the root seed and the correction word seeds sCW
    // 2*alpha_bits bits in order to store the values of tCW_L and tCW_R
    // beta_bits bits in order to store the value of CW(n+1)
    int n = 2 + (alpha_bits+1)*2 + ((2*alpha_bits) / 64) + p;
    if ((2*alpha_bits) % 64 != 0) { n += 1; }
    if (q != 0) { n += 1; }
    uint64_t* key_0 = (uint64_t*) calloc(n , 64);
    if (key_0 == NULL) { exit(1); }
    uint64_t* key_1 = (uint64_t*) calloc(n , 64);
    if (key_1 == NULL) { exit(1); }

    std::cout << std::endl;
    std::cout << " - - - GEN - - - " <<  std::endl;
    Gen(seed, alpha, alpha_bits, beta, beta_bits, key_0, key_1);

    // std::cout << std::endl;
    // std::cout << "KEYS" << std::endl;
    // for (int i = 0; i < n; i++)
    // {
    //     std::cout << key_0[i] << ' ' << key_1[i] << std::endl;
    // }

    // std::cout << std::endl;
    // std::cout << "CW final" << std::endl;
    // for (int i = 0; i < n2; i++)
    // {
    //     std::cout << beta[i] << ' ';
    // }
    // std::cout << std::endl;

    std::cout << std::endl;
    std::cout << " - - - EVAL 0 - - - " <<  std::endl;

    uint64_t* beta0 = (uint64_t*) calloc(n2, 64);
    if (beta0==NULL) { exit(1); }
    Eval(alpha, alpha_bits, beta0, beta_bits, key_0, 0);
    // std::cout << std::endl;
    // std::cout << "BETA (decoded by key0)" << std::endl;
    // for (int i = 0; i < n2; i++)
    // {
    //     std::cout << beta0[i] << ' ';
    // }
    // std::cout << std::endl;

    std::cout << std::endl;
    std::cout << " - - - EVAL 1 - - - " <<  std::endl;

    uint64_t* beta1 = (uint64_t*) calloc(n2, 64);
    if (beta1==NULL) { exit(1); }
    Eval(alpha, alpha_bits, beta1, beta_bits, key_1, 1);
    // std::cout << std::endl;
    // std::cout << "BETA (decoded by key1)" << std::endl;
    // for (int i = 0; i < n2; i++)
    // {
    //     std::cout << beta1[i] << ' ';
    // }
    // std::cout << std::endl;

    std::cout << std::endl;
    std::cout << " - - - XOR - - -" << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < n2; i++)
    {
        std::cout << (beta0[i] ^ beta1[i]) << ' ';
    }
    std::cout << std::endl << std::endl;

    free(beta1); free(beta0); free(key_1); free(key_0); free(beta); free(alpha);

    return 0;
}