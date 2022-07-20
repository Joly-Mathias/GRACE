# include <iostream>
# include <cstring>
# include <string>
# include <fstream>

# include <gmp.h>
# include <gmpxx.h>
# include <assert.h>

# include "AES.hpp"
# include "DPF.hpp"

void answer(std::vector<mpz_class> k, char id)
{
    int n = 100; int l = 32; int i = 0; int j = 0;
    std::string infilename = "DB_32.txt"; std::ifstream hDB_file (infilename); 
    std::string outfilename = "yX.txt"; outfilename[1] = id; std::ofstream y_file (outfilename);
    mpz_class power2_128(1); power2_128 = power2_128 << 128; // 2^128
    mpz_class y(0);


	if (hDB_file.is_open())
	{
        mpz_class hURL, y_i, x_i, init_vect;
        init_vect = (k[0] + 2*n) % power2_128;

		for(i=0; i<n; i++)
		{
            hURL = 0; y_i = 0; x_i = i;
			hDB_file >> hURL;
            y_i = Eval(x_i, 7, 1, k, (id-48));
            //std::cout << i << " : " << y_i << std::endl;
            if (mpz_tstbit(y_i.get_mpz_t(),0) == 1) 
            {
                // hURL = convert(hURL, l, init_vect);
                // std::cout << i << " : " << hURL << std::endl;
                y = y ^ hURL;
            }
		}
		hDB_file.close();
	}
    if (y_file.is_open())
    {
        y_file << y << "\n" ;
        y_file.close();
    }
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
        
            std::vector<mpz_class> key;
            mpz_class buffer;

            if (key_file.is_open())
            {
                for (int i = 0; i < 7 + 3; i ++)
                { 
                    key_file >> buffer; 
                    key.push_back(buffer);
                }
                key_file.close();
            }
            answer(key, c);
            std::cout << std::endl;
        }
    }
    return 0;
}