# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <fstream>

# include <time.h>
# include <gmp.h>
# include <gmpxx.h>
# include <assert.h>

# include "AES.hpp"
# include "DPF.hpp"

void query(mpz_t alpha, mpz_t* key_0, mpz_t* key_1)
{
    srand(time(NULL));
    mpz_t seed; mpz_init(seed); mpz_set_ui(seed,rand());    
    mpz_t beta; mpz_init(beta); mpz_set_ui(beta, 1);

    Gen(seed, alpha, 7, beta, 1, key_0, key_1);
    mpz_clear(seed); mpz_clear(alpha); mpz_clear(beta);
}

void reconstruct(mpz_t y0, mpz_t y1) 
{
    mpz_t power2_32; mpz_init(power2_32); mpz_ui_pow_ui(power2_32, 2, 32);
    mpz_add(y0, y0, y1); mpz_mod(y0, y0, power2_32);
    std::cout << y0 << std::endl << std::endl;
}

int main()
{
    int n = 100; int l = 32; int logn = 7;
    char c = ' ';

    while(c != 'T')
    {
        std::cout << "Entrer 'Q' pour une requete, 'R' pour reconstruire la reponse, 'T' pour terminer l'algorithme" << std::endl;
        std::cin >> c ;

        mpz_t i, y0, y1;
        mpz_init(i); mpz_init(y0); mpz_init(y1);

        if (c == 'Q')
        {
            
            mpz_t* key_0 = (mpz_t *) malloc((logn + 3)*sizeof(mpz_t));
            mpz_t* key_1 = (mpz_t *) malloc((logn + 3)*sizeof(mpz_t));
            if(!key_0 || !key_1) { std::cout << "Erreur d'allocation de mémoire" << std::endl; return 1; }

            std::cout << "Entrer l'indice i :" << std::endl;
            std::cin >> i ;

            //std::cout << "1 : \n" << key_0[0] << std::endl;

            query(i, key_0, key_1);
            
            //std::cout << "2 : \n" << key_0[0] << std::endl << std::endl;

            std::ofstream key_file;

            key_file.open("k0.txt", std::ofstream::trunc);
            for (int i = 0; i < logn + 3; i ++)
            {
                key_file << key_0[i] << "\n" ;
                //std::cout << key_0[i] << std::endl;
                mpz_clear(key_0[i]);
                //std::cout << key_0[i] << std::endl;
            }
            key_file.close();

            key_file.open("k1.txt", std::ofstream::trunc);
            for (int i = 0; i < logn + 3; i ++)
            {
                key_file << key_1[i] << "\n" ;
                //std::cout << key_1[i] << std::endl;
                mpz_clear(key_1[i]);
            }
            key_file.close();
            
            free(key_1); free(key_0);
        }
        if (c == 'R')
        {
            std::string line;
            long long bloc;
            uint64_t buffer;
            std::ifstream y0_file ("y0.txt");
            if (y0_file.is_open())
            {
                y0_file >> y0;
                y0_file.close();
            }
            std::ifstream y1_file ("y1.txt");
            if (y1_file.is_open())
            {
                y1_file >> y1;
                y1_file.close();
            }
            reconstruct(y0, y1);
        }
        mpz_clear(i); mpz_clear(y0); mpz_clear(y1); 
    }
    return 0;
}