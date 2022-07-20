#ifndef ___DPF_HPP___
#define ___DPF_HPP___

mpz_class convert(const mpz_class seed, const int g_bits, mpz_class init_vect);
void Gen(const mpz_class seed, const mpz_class alpha, const int alpha_bits, mpz_class beta, const int beta_bits, std::vector<mpz_class>& key_0, std::vector<mpz_class>& key_1);
mpz_class Eval(const mpz_class input, const int input_bits, const int output_bits, const std::vector<mpz_class>& key, const int b);

#endif