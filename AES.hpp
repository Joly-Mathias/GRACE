#ifndef ___AES_HPP___
#define ___AES_HPP___

int expandEncryptKey(uint32_t *rk, const uint64_t *key, int keybits);
int expandDecryptKey(uint32_t *rk, const uint64_t *key, int keybits);
void encryptAES(const uint32_t *rk, int nrounds, const uint64_t plain[16], uint64_t cipher[16]);
void decryptAES(const uint32_t *rk, int nrounds, const uint64_t cipher[16], uint64_t plain[16]);

#endif