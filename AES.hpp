#ifndef ___AES_HPP___
#define ___AES_HPP___

int expandEncryptKey(uint32_t *rk, const uint8_t *key, int keybits);
int expandDecryptKey(uint32_t *rk, const uint8_t *key, int keybits);
void encryptAES(const uint32_t *rk, int nrounds, const uint8_t plaintext[16], uint8_t ciphertext[16]);
void decryptAES(const uint32_t *rk, int nrounds, const uint8_t ciphertext[16], uint8_t plaintext[16]);

#endif