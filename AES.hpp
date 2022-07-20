#ifndef ___AES_HPP___
#define ___AES_HPP___

int expandEncryptKey(uint32_t *rk, const mpz_class key, int keybits);
mpz_class encryptAES(const uint32_t *rk, const int nrounds, const mpz_class plain);

#endif