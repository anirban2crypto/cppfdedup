#ifndef __CRYPTO_H
#define __CRYPTO_H
using namespace std; 
#define SALT_SIZE 32
#define HKEY_SIZE 16
#define BLOCK_SIZE 16
#define KEY_SIZE 16 

uint8_t* mleKeygen(vector<uint8_t> &message);
void  cpaDecrypt(uint8_t *cpakey,uint8_t *cpaiv,unsigned char *cpacipher,unsigned char *recovmsg,int &dec_len,int &ciph_len);
void  cpaEncrypt(uint8_t *cpakey,uint8_t *cpaiv,unsigned char *message,unsigned char *cpacipher, int &ciph_len,int &msg_len);
void  mleEncrypt(uint8_t *mlekey,unsigned char *message,unsigned char *mlecipher,int &ciph_len,int &msg_len);
void  mleDecrypt(uint8_t *mlekey,unsigned char *mlecipher,unsigned char *recovmsg,int &ciph_len,int &dec_len);
void gen_params(unsigned char key[KEY_SIZE], unsigned char iv[BLOCK_SIZE]);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);  



#endif

