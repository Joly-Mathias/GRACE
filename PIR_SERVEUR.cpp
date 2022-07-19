# include <iostream>
# include <cstring>
# include <string>
# include <fstream>

# include <gmp.h>
# include <gmpxx.h>
# include <assert.h>

# include "AES.hpp"
# include "DPF.hpp"

void answer(mpz_t* k, char id)
{
    int n = 100; int l = 32; int i = 0; int j = 0;
    std::string infilename = "DB_32.txt"; std::ifstream hDB_file (infilename); 
    std::string outfilename = "yX.txt"; outfilename[1] = id; std::ofstream y_file (outfilename);
    mpz_t y; mpz_init(y); mpz_set_ui(y, 0);
    
	if (hDB_file.is_open())
	{
        mpz_t hURL; mpz_init(hURL);
        mpz_t y_i; mpz_init(y_i);
        mpz_t x; mpz_init(x);
        mpz_t init_vect; mpz_init(init_vect); 
        mpz_add_ui(init_vect, k[0], n);
        if (mpz_sizeinbase(init_vect, 2) == 129) { mpz_mod_ui(init_vect, init_vect, n); }

		for(i=0; i<n; i++)
		{
            mpz_set_ui(hURL, 0); mpz_set_ui(y_i, 0); mpz_set_ui(x,i);
			hDB_file >> hURL;
            // std::cout << std::endl << x << std::endl;
            Eval(x, 7, y_i, 1, k, (id-48));
            if (mpz_tstbit(y_i,0) == 1) 
            {
                convert(hURL, l, hURL, init_vect);
                mpz_xor(y, y, hURL); 
            }
		}
		hDB_file.close();
        mpz_clear(y_i); mpz_clear(hURL); mpz_clear(x);
	}
    if (y_file.is_open())
    {
        y_file << y << "\n" ;
        y_file.close();
    }
    mpz_clear(y);
}

int main()
{
    int n = 100; int l = 32; int logn = 7;
    char c = ' ';

    while(c != 'T')
    {
        std::cout << "Entrer '0' pour le serveur 0, '1' pour le serveur 1, 'T' pour terminer l'algorithme" << std::endl;
        std::cin >> c ;

        if(c == '0' || c == '1')
        {
            std::string infilename = "kX.txt";
            infilename[1] = c;
            std::ifstream key_file (infilename);
        
            mpz_t* key = (mpz_t *) malloc((logn + 3)*sizeof(mpz_t));
            if(key == NULL) { std::cout << "Erreur d'allocation de mémoire" << std::endl; exit(0); }

            if (key_file.is_open())
            {
                for (int i = 0; i < 7 + 3; i ++)
                {
                    mpz_init(key[i]);
                    key_file >> key[i];
                }
                key_file.close();
            }
            answer(key, c);
            free(key);
        }
    }
    return 0;
}