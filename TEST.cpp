# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <fstream>

# include <time.h>
# include <gmp.h>
# include <gmpxx.h>
# include <assert.h>

int main()
{
    std::vector<mpz_class> d;
    mpz_class c ;
    mpz_class b;
    uint32_t a;

    /*
    std::ifstream key_file ("k0.txt");
    if (key_file.is_open())
    {
        key_file >> c ;
        key_file.close();
    }
    */

    c = 4294967295;
    c = (c << 32) + 4294967294;
    c = (c << 32) + 4294967293;
    c = (c << 32) + 4294967292;
    std::cout << (c) << std::endl;
    b = c % 4294967296;
    d.push_back(b);
    a = b.get_ui();
    std::cout << a << std::endl;
    b = (c >> 32) % 4294967296;
    d.push_back(b);
    a = b.get_ui();
    std::cout << a << std::endl;
    b = (c >> 64) % 4294967296;
    d.push_back(b);
    a = b.get_ui();
    std::cout << a << std::endl;
    b = (c >> 96) % 4294967296;
    d.push_back(b);
    a = b.get_ui();
    std::cout << a << std::endl << std::endl;

    std::cout << (mpz_tstbit(c.get_mpz_t(),0)) << std::endl;
    std::cout << (mpz_tstbit(c.get_mpz_t(),32)) << std::endl;
    std::cout << (mpz_tstbit(c.get_mpz_t(),64)) << std::endl;
    std::cout << (mpz_tstbit(c.get_mpz_t(),96)) << std::endl << std::endl;

    std::cout << d[0] << std::endl;
    std::cout << d[1] << std::endl;
    std::cout << d[2] << std::endl;
    std::cout << d[3] << std::endl;

    return 0;
}