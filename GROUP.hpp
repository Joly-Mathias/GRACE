#ifndef ___GROUP_HPP___
#define ___GROUP_HPP___

void concatenate(const uint64_t* key_1, const int n1, const uint64_t* key_2, const int n2, uint64_t* key);
void convert(const uint64_t* seed, uint64_t* g_seed, const int g_bits);
void addition(uint64_t* g1, const uint64_t* g2, const int p, const int q);
void substraction(uint64_t* g1, const uint64_t* g2, const int p, const int q);
void operation(const int t, const uint64_t* beta, const int beta_bits, const uint64_t* s0, const uint64_t* s1, uint64_t* CW);

#endif