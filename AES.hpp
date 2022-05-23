#ifndef ___AES_HPP___
#define ___AES_HPP___

int expandEncryptKey(uint32_t *rk, const mpz_t key, int keybits);
void encryptAES(const uint32_t *rk, int nrounds, const mpz_t plain, mpz_t cipher);

#endif