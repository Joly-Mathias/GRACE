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

void query(mpz_class alpha, std::vector<mpz_class>& key_0, std::vector<mpz_class>& key_1)
{
    srand(time(NULL));
    mpz_class seed = rand();   
    mpz_class beta(1);
    Gen(seed, alpha, 7, beta, 1, key_0, key_1);
}

void reconstruct(mpz_class y0, mpz_class y1) 
{
    std::cout << (y0 ^ y1) << std::endl << std::endl;
}

int main()
{
    int n = 100; int l = 32; int logn = 7;
    char c = ' ';

    while(c != 'T')
    {
        std::cout << "Entrer 'Q' pour une requete, 'R' pour reconstruire la reponse, 'T' pour terminer l'algorithme" << std::endl;
        std::cin >> c ;

        mpz_class i, y0, y1;

        if (c == 'Q')
        {
            
            std::vector<mpz_class> key_0;
            std::vector<mpz_class> key_1;
            std::cout << "Entrer l'indice i :" << std::endl;
            std::cin >> i ;
            std::cout << std::endl;

            query(i, key_0, key_1);

            //std::cout << "check" << std::endl;

            std::ofstream key_file;
            key_file.open("k0.txt", std::ofstream::trunc);
            for (mpz_class buffer : key_0) { key_file << buffer << "\n" ; }
            key_file.close();

            key_file.open("k1.txt", std::ofstream::trunc);
            for (mpz_class buffer : key_1) { key_file << buffer << "\n" ; }
            key_file.close();
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
    }
    return 0;
}