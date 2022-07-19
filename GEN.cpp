# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <fstream>

# include <gmp.h>
# include <gmpxx.h>
# include <assert.h>

# include "AES.hpp"
# include "GROUP.hpp"
# include "DPF.hpp"

int main(int argc, char **argv)
{
    assert(argc > 3);
    int flag;

    char* seed_char = argv[1];
    mpz_t seed;
    mpz_init(seed);
    mpz_set_ui(seed,0);
    flag = mpz_set_str(seed, seed_char, 2);
    assert (flag == 0); 
 
    char* alpha_char = argv[2];
    int alpha_bits = strlen(alpha_char);
    mpz_t alpha;
    mpz_init(alpha);
    mpz_set_ui(alpha,0);
    flag = mpz_set_str(alpha, alpha_char, 2);
    assert (flag == 0); 
 
    // std::cout <<  std::endl << " - - - BETA - - - " << std::endl;
    char* beta_char = argv[3];
    int beta_bits = strlen(beta_char);
    mpz_t beta;
    mpz_init(beta);
    mpz_set_ui(beta,0);
    flag = mpz_set_str(beta, beta_char, 2);
    assert (flag == 0); 
    // std::cout << beta << std::endl << std::endl;

    mpz_t* key_0 = (mpz_t *) malloc((alpha_bits + 3)*sizeof(mpz_t));
    if(key_0 == NULL) { std::cout << "Erreur d'allocation de mémoire" << std::endl; exit(0); }
    mpz_t* key_1 = (mpz_t *) malloc((alpha_bits + 3)*sizeof(mpz_t));
    if(key_1 == NULL) { std::cout << "Erreur d'allocation de mémoire" << std::endl; exit(0); }

    Gen(seed, alpha, alpha_bits, beta, beta_bits, key_0, key_1);
    mpz_clear(seed); mpz_clear(alpha); mpz_clear(beta);
  
    std::ofstream key_file;

    key_file.open("key0.txt");
    for (int i = 0; i < alpha_bits + 3; i ++)
    {
        key_file << key_0[i] << "\n" ;
        mpz_clear(key_0[i]);
    }
    key_file.close();

    key_file.open("key1.txt");
    for (int i = 0; i < alpha_bits + 3; i ++)
    {
        key_file << key_1[i] << "\n" ;
        mpz_clear(key_1[i]);
    }
    key_file.close();

    free(key_1); free(key_0);

    return 0;
}
