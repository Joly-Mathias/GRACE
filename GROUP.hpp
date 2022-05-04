#ifndef ___GROUP_HPP___
#define ___GROUP_HPP___

void concatenate(const uint64_t* key_1, const int n1, const uint64_t* key_2, const int n2, uint64_t* key);
void convert(const uint64_t* seed, uint64_t* g_seed, const int g_bits);
void add(uint64_t* g1, const uint64_t* g2, const int p, const int q);
void subtract(uint64_t* g1, const uint64_t* g2, const int p, const int q);
void negative(uint64_t* g, const int p, const int q);
void get_final_CW(uint64_t* beta, const int beta_bits, const uint64_t* s0, const uint64_t* s1, const int t);

#endif