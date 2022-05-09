# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cstring>
# include <string>
# include <fstream>

# include "AES.hpp"
# include "GROUP.hpp"
# include "DPF.hpp"

int main(int argc, char **argv)
{
    // Converting beta from char to uint64_t 
    // Left block incomplete : XXXXX110 10010111 01001011 
    std::size_t pos{};
    std::string output_bits(argv[1]);
    const int beta_bits { std::stoi(output_bits, &pos) };
    int p = beta_bits / 64;
    int q = beta_bits % 64;
    int n = p ;
    if (q != 0) { n += 1; }
    uint64_t* beta0 = (uint64_t*) calloc(n, 64);
    if (beta0==NULL) { exit(1); }
    uint64_t* beta1 = (uint64_t*) calloc(n, 64);
    if (beta1==NULL) { exit(1); }
    std::cout << std::endl;

    // Get betas from file betaX.txt
    std::string line;
    long long bloc;
    uint64_t buffer;
    std::ifstream beta0_file ("beta0.txt");
    if (beta0_file.is_open())
    {
        int i = 0;
        while ( i < n )
        {
            getline(beta0_file,line);
            bloc = std::stoll(line, &pos);
            buffer = bloc << 32;
            getline(beta0_file,line);
            bloc = std::stoll(line, &pos);
            buffer += bloc;
            beta0[i] = buffer ;
            i++;
            // std::cout << buffer << std::endl;
        }
        beta0_file.close();
    }
    std::ifstream beta1_file ("beta1.txt");
    if (beta1_file.is_open())
    {
        int i = 0;
        if (q != 0)
        {
            getline(beta1_file,line);
            bloc = std::stoll(line, &pos);
            buffer = 0;
            if (q >= 32)
            {
                buffer = bloc << 32;
                getline(beta1_file,line);
                bloc = std::stoll(line, &pos);
            }
            buffer += bloc;
            beta1[i] = buffer ;     
            beta1_file.close();
        }
        while ( i < p )
        {
            getline(beta1_file,line);
            bloc = std::stoll(line, &pos);
            buffer = bloc << 32;
            getline(beta1_file,line);
            bloc = std::stoll(line, &pos);
            buffer += bloc;
            beta1[i] = buffer ;
            i++;
            // std::cout << buffer << std::endl;
        }
    }

    std::cout << " - - - BETA 0 - - -" << std::endl;
    // beta0[0] = 10258214498830044236U;
    for (int i = 0; i < n; i++)
    {
        std::cout << beta0[i] << ' ';
    }
    std::cout << std::endl << std::endl;
    std::cout << " - - - BETA 1 - - -" << std::endl;
    // beta1[0] = 35095298128240004U;
    for (int i = 0; i < n; i++)
    {
        std::cout << beta1[i] << ' ';
    }
    std::cout << std::endl << std::endl;
    std::cout << " - - - BETA - - -" << std::endl;
    add(beta0, beta1, p, q);
    for (int i = 0; i < n; i++)
    {
        std::cout << beta0[i] << ' ';
    }
    std::cout << std::endl ;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            unsigned char temp = (beta0[i] >> (56 - 8*j));
            if (temp != 0)
            {
                std::cout << temp ;
            }
            // std::cout << temp ;
        }
    }
    std::cout << std::endl << std::endl;

    free(beta0); free(beta1);

    return 0;
}
