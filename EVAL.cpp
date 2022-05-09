# include <iostream>
# include <cstring>
# include <string>
# include <fstream>

# include "AES.hpp"
# include "GROUP.hpp"
# include "DPF.hpp"

int main(int argc, char **argv)
{
    // Create input holder knowing the number of bits
    char* input_char = argv[1];
    int input_bits = strlen(input_char)*8;
    int p1 = input_bits / 64;
    int q1 = input_bits % 64;
    int n1 = (q1 == 0) ? p1 : p1+1 ;
    uint64_t* input = (uint64_t*) calloc(n1, 64);
    if (input==NULL) { exit(1); }

    // Convert input from char to uint64_bit
    // Right block incomplete : 11010010 11101001 011XXXXX  
    uint8_t temp;
    for (int i = 0; i < p1; i++)
    {
        for (int j = 0 ; j < 8 ; j++)
        {
            temp = *input_char != 0 ? *input_char++ : 0;
            input[i] ^= (uint64_t)temp << (56 - 8 * j);
        }
    }
    for (int j = 0; j < q1; j++)
    {
        temp = *input_char != 0 ? *input_char++ : 0;
        input[p1] ^= (uint64_t)temp << (56 - 8 * j);
    }

    // Create output holder knowing the number of bits
    std::size_t pos{};
    std::string beta_bits(argv[2]);
    const int output_bits { std::stoi(beta_bits, &pos) };
    int p2 = output_bits / 64;
    int q2 = output_bits % 64;
    int n2 = (q2 == 0) ? p2 : p2+1 ;
    uint64_t* output = (uint64_t*) calloc(n2, 64);
    if (output==NULL) { exit(1); }

    // Create key holder knowing the number of bits
    int n = 2 + (input_bits+1)*2 + ((2*input_bits) / 64) + (output_bits / 64);
    if ((2*input_bits) % 64 != 0) { n += 1; }
    if (output_bits % 64 != 0) { n += 1; }
    uint64_t* key = (uint64_t*) calloc(n , 64);
    if (key == NULL) { exit(1); }

    // Get key from file keyX.txt
    char b = argv[3][0];
    std::string infilename = "keyX.txt";
    infilename[3] = b;
    std::ifstream key_file (infilename);
    std::string line;
    long long bloc;
    uint64_t buffer;
    if (key_file.is_open())
    {
        int i = 0;
        while ( i < n )
        {
            getline(key_file,line);
            bloc = std::stoll(line, &pos);
            buffer = bloc << 32;
            getline(key_file,line);
            bloc = std::stoll(line, &pos);
            buffer += bloc;
            key[i] = buffer ;
            i++;
            // std::cout << buffer << std::endl;
        }
        key_file.close();
    }

    // std::cout << std::endl << " - - - EVAL " << b << " - - - " <<  std::endl;
    Eval(input, input_bits, output, output_bits, key, (b-48));
    std::cout << std::endl << "BETA " << b << std::endl;
    for (int i = 0; i < n2; i++)
    {
        std::cout << output[i] << ' ';
    }
    std::cout << std::endl << std::endl;

    std::ofstream beta;
    std::string outfilename = "betaX.txt";
    outfilename[4] = b;
    beta.open(outfilename);
    int i = 0;
    uint64_t tmp = output[i];
    if(q2 != 0)
    {
        if(q2 > 32)
        {
            beta << (tmp >> 32) << "\n" ;
        }
        beta << (tmp % 0x0000000100000000) << "\n" ;
        i++;
    }
    while (i < n2)
    {
        tmp = output[i];
        beta << (tmp >> 32) << "\n" ;
        beta << (tmp % 0x0000000100000000) << "\n" ;
        i++;
    }

    beta.close();

    free(key); free(input); free(output);

    return 0;
}