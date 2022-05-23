# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <string>
# include <fstream>

# include <gmp.h>
# include <assert.h>

# include "AES.hpp"
# include "DPF.hpp"

int main(int argc, char **argv)
{
    std::size_t pos{};
    std::string output_bits(argv[1]);
    const int beta_bits { std::stoi(output_bits, &pos) };

    mpz_t beta0, beta1;
    mpz_init(beta0);
    mpz_init(beta1);

    std::string line;
    long long bloc;
    uint64_t buffer;
    std::ifstream beta0_file ("beta0.txt");
    if (beta0_file.is_open())
    {
        beta0_file >> beta0;
        beta0_file.close();
    }
    std::ifstream beta1_file ("beta1.txt");
    if (beta1_file.is_open())
    {
        beta1_file >> beta1;
        beta1_file.close();
    }

    std::cout << " - - - BETA 0 - - -" << std::endl;
    std::cout << beta0 << std::endl << std::endl;

    std::cout << " - - - BETA 1 - - -" << std::endl;
    std::cout << beta1 << std::endl << std::endl;

    mpz_t power2;
    mpz_init(power2);
    mpz_ui_pow_ui(power2, 2, beta_bits);

    std::cout << " - - - BETA - - -" << std::endl;
    mpz_add(beta0, beta0, beta1);
    mpz_mod(beta0, beta0, power2);
    std::cout << beta0 << std::endl << std::endl;

    mpz_clear(beta0); mpz_clear(beta1);

    return 0;
}

// seed  : 1111111111111111111111111111111111111111111111111111111111111111
// alpha : 1011001110001111000011111000001111110000001111111000000011111111
// beta  : 1111111100000001111111000000111111000001111100001111000111001101
