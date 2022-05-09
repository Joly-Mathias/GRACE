# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <fstream>

# include "AES.hpp"
# include "GROUP.hpp"
# include "DPF.hpp"

int main(int argc, char **argv)
{
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
    // std::cout << std::endl;
    // std::cout << " - - - BETA - - -" << std::endl;
    // std::cout << std::endl << beta_char << std::endl;
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
    std::cout << std::endl << std::endl;

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

    // std::cout << " - - - GEN - - - " <<  std::endl << std::endl;
    Gen(seed, alpha, alpha_bits, beta, beta_bits, key_0, key_1);

    std::ofstream output;
    output.open("key0.txt");
    for (int i = 0; i < n; i ++)
    {;
        uint64_t temp = key_0[i];
        output << (temp >> 32) << "\n" ;
        output << (temp % 0x0000000100000000) << "\n" ;
    }
    output.close();
    output.open("key1.txt");
    for (int i = 0; i < n; i ++)
    {
        uint64_t temp = key_1[i];
        output << (temp >> 32) << "\n" ;
        output << (temp % 0x0000000100000000) << "\n" ;
        // std::cout << temp << std::endl;
    }
    output.close();

    free(key_1); free(key_0); free(beta); free(alpha);

    return 0;
}
