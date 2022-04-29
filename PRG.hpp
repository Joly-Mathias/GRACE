#ifndef ___PRG_HPP___
#define ___PRG_HPP___

void zeros_gen(uint64_t* key);
void ones_gen(uint64_t* key);
void concatenateKey(const uint64_t* key_1, const int n1, const uint64_t* key_2, const int n2, uint64_t* key);
void doubleKey(const uint64_t* key_128, uint64_t* key_256);

#endif