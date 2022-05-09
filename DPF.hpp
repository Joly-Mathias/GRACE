#ifndef ___DPF_HPP___
#define ___DPF_HPP___

void key_gen(uint64_t* seed_128);
void incrementation(uint64_t* init_vect);
void decrementation(uint64_t* init_vect);
void doubleKey(const uint64_t* key_128, uint64_t* key_256, uint64_t* init_vect);
void inv_doubleKey(const uint64_t* key_128, const uint64_t* key_256, uint64_t* init_vect);

void get_tLR(const uint64_t* sLR, int* tLR);
void get_tCW(const uint64_t* key, const int input_bits, const int round, int* tCW);
void put_tCW(uint64_t* key, const int input_bits, const int round, const int tCW);

void Gen(const uint64_t seed, const uint64_t* alpha, const int alpha_bits, uint64_t* beta, const int beta_bits, uint64_t* key_0, uint64_t* key_1);
void Eval(const uint64_t* input, const int input_bits, uint64_t* output, const int output_bits, const uint64_t* key, const int b);

#endif