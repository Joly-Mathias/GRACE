# include <iostream>
# include <string>
# include <fstream>

# include <gmp.h>
# include <gmpxx.h>
# include <memory.h>
# include "sha256.h"

void sha256(const char* URL, mpz_t hURL)
{
	BYTE buf[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;

	sha256_init(&ctx);
	sha256_update(&ctx, (BYTE*)URL, strlen((char*)URL));
	sha256_final(&ctx, buf);

	mpz_t power2_8; mpz_init(power2_8); mpz_ui_pow_ui(power2_8, 2, 8);
	mpz_init(hURL); mpz_set_ui(hURL, (unsigned long) buf[0]);
	for( int i = 1; i < SHA256_BLOCK_SIZE; i ++)
	{
		mpz_mul(hURL, hURL, power2_8);
		mpz_add_ui(hURL, hURL, (unsigned long) buf[i]);
	}
	mpz_clear(power2_8);
}

void sha32(const char* URL, mpz_t hURL)
{
	BYTE buf[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;

	sha256_init(&ctx);
	sha256_update(&ctx, (BYTE*)URL, strlen((char*)URL));
	sha256_final(&ctx, buf);

	mpz_t power2_8; mpz_init(power2_8); mpz_ui_pow_ui(power2_8, 2, 8);
	mpz_init(hURL); mpz_set_ui(hURL, (unsigned long) buf[0]);
	for( int i = 1; i < 4; i ++)
	{
		mpz_mul(hURL, hURL, power2_8);
		mpz_add_ui(hURL, hURL, (unsigned long) buf[i]);
	}
	/*
	for( int i = 4; i < SHA256_BLOCK_SIZE; i ++)
	{
		mpz_mul(hURL, hURL, power2_8);
	}
	*/
	mpz_clear(power2_8);
}

int main()
{
	int l = 256; //32 
	int n = 100;
	int i = 0;

	mpz_t* DB = (mpz_t *) malloc(n*sizeof(mpz_t));
    std::string infilename = "DB.txt"; 
	std::string outfilename = "DB_32.txt";
	//std::string outfilename = "DB_256.txt";
    std::ifstream DB_file (infilename); std::ofstream hDB_file (outfilename);
	char URL[16];
    mpz_t hURL; mpz_init(hURL);
    
	if (DB_file.is_open())
	{
		for(i=0; i<n; i++)
		{
			DB_file >> URL;
			sha32(URL, hURL);
			//sha256(URL, hURL);
			mpz_init(DB[i]);
			mpz_set(DB[i], hURL);
		}
		DB_file.close();
	}

	if (hDB_file.is_open())
	{
    	for (int i = 0; i < n; i ++)
    	{
        	hDB_file << DB[i] << "\n" ;
        	mpz_clear(DB[i]);
    	}
    	hDB_file.close();
	}

	mpz_clear(hURL); free(DB);
	return 0;
}