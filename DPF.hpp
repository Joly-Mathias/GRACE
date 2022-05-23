#ifndef ___DPF_HPP___
#define ___DPF_HPP___

void key_gen(mpz_t seed_128);
void incrementation(mpz_t init_vect);
void decrementation(mpz_t init_vect);
void doubleKey(const mpz_t key_128, mpz_t key_256, mpz_t init_vect);
void inv_doubleKey(const mpz_t key_128, const mpz_t key_256, mpz_t init_vect);

void get_tLR(const mpz_t sLR, int* tLR);

void Gen(const mpz_t seed, const mpz_t alpha, const int alpha_bits, mpz_t beta, const int beta_bits, mpz_t* key_0, mpz_t* key_1);
void Eval(const mpz_t input, const int input_bits, mpz_t output, const int output_bits, const mpz_t* key, const int b);

#endif