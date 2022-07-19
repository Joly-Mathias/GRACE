# include <iostream>
# include <cstring>
# include <string>
# include <fstream>

# include <gmp.h>
# include <gmpxx.h>
# include <assert.h>

# include "AES.hpp"
# include "GROUP.hpp"
# include "DPF.hpp"

int main(int argc, char **argv)
{
    int flag;
    std::size_t pos{};

    char* input_char = argv[1];
    int input_bits = strlen(input_char);
    mpz_t input;
    mpz_init(input);
    mpz_set_ui(input,0);
    flag = mpz_set_str(input, input_char, 2);
    assert (flag == 0); 

    std::string out_bits_str(argv[2]);
    const int output_bits { std::stoi(out_bits_str, &pos) };
    mpz_t output;
    mpz_init(output);

    char b = argv[3][0];
    std::string infilename = "keyX.txt";
    infilename[3] = b;
    std::ifstream key_file (infilename);
    
    mpz_t* key = (mpz_t *) malloc((input_bits + 3)*sizeof(mpz_t));
    if(key == NULL) { std::cout << "Erreur d'allocation de mémoire" << std::endl; exit(0); }

    if (key_file.is_open())
    {
        for (int i = 0; i < input_bits + 3; i ++)
        {
            mpz_init(key[i]);
            key_file >> key[i];
        }
        key_file.close();
    }

    Eval(input, input_bits, output, output_bits, key, (b-48));

    // std::cout << " - - - BETA " << b << " - - - " << std::endl ;
    // std::cout << output << std::endl << std::endl;

    std::ofstream beta_file;
    std::string betafilename = "betaX.txt";
    betafilename[4] = b;
    beta_file.open(betafilename);
    if (beta_file.is_open())
    {
        beta_file << output << "\n" ;
        beta_file.close();
    }

    mpz_clear(input); mpz_clear(output);
    for (int i = 0; i < input_bits + 3; i ++) { mpz_clear(key[i]); }
  
    free(key);

    return 0;
}