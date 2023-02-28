#include <string.h>
#include <iostream>
#include <string>
#include <memory>
#include <limits>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include "mlecrypto.h"
#define SALT_SIZE 32
#define HKEY_SIZE 16
#define BLOCK_SIZE 16
#define KEY_SIZE 16 

unsigned char *ivstr = (unsigned char *)"DSmlThngaInGrtWy";  

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);            
void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}
void gen_params(unsigned char* key, unsigned char* iv)
{
    int rc = RAND_bytes(key, KEY_SIZE);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes key failed");

    rc = RAND_bytes(iv, BLOCK_SIZE);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes for iv failed");
}

void  cpaDecrypt(uint8_t *cpakey,uint8_t *cpaiv,unsigned char *cpacipher,unsigned char *recovmsg,int &dec_len,int &ciph_len){
    unsigned char key[KEY_SIZE];
    unsigned char iv[BLOCK_SIZE];
    unsigned char *ciphertext = (unsigned char *)&cpacipher[0];

    /* copy the key */
    for(int i = 0; i < KEY_SIZE; i ++){
        key[i]=cpakey[i];
    }
    /*  copy the iv  */
    for(int i = 0; i < BLOCK_SIZE; i ++){
        iv[i]=cpaiv[i];
    }
    /*
     * Buffer for decrypted text. Ensure the buffer is long enough for the
     * ciphertext which may be longer than the plaintext, depending on the
     * algorithm and mode.
     */
    /* Decrypt the plaintext */
    dec_len = decrypt(ciphertext, ciph_len, key, iv,recovmsg);
    
}

void  cpaEncrypt(uint8_t *cpakey,uint8_t *cpaiv,unsigned char *message,unsigned char *cpacipher, int &ciph_len,int &msg_len){
    unsigned char key[KEY_SIZE];
    unsigned char iv[BLOCK_SIZE];
    /*  gen random iv and key */
    gen_params(key, iv);
    /*  save the key  */
    for(int i = 0; i < KEY_SIZE; i ++){
         cpakey[i]=key[i];
    }
    for(int i = 0; i < BLOCK_SIZE; i ++){
         cpaiv[i]=iv[i];
    }
    /* get the message  */
    unsigned char *plaintext =(unsigned char *)&message[0];
    /*
     * Buffer for ciphertext. Ensure the buffer is long enough for the
     * ciphertext which may be longer than the plaintext, depending on the
     * algorithm and mode.
     */
  
    /* Encrypt the plaintext */
    ciph_len = encrypt (plaintext, msg_len, key, iv, cpacipher);
    

}
void  mleEncrypt(uint8_t *mlekey,unsigned char *message,unsigned char *mlecipher,int &ciph_len,int &msg_len){
    unsigned char key[KEY_SIZE];
    unsigned char iv[BLOCK_SIZE];
    for(int i = 0; i < HKEY_SIZE; i ++)
        key[i]=mlekey[i];
    for(int i = 0; i < HKEY_SIZE; i ++)
        iv[i]=ivstr[i];
    unsigned char *plaintext = (unsigned char *)&message[0];
    /*
     * Buffer for ciphertext. Ensure the buffer is long enough for the
     * ciphertext which may be longer than the plaintext, depending on the
     * algorithm and mode.
     */
    /* Buffer for the decrypted text */
    /* Encrypt the plaintext */
    ciph_len = encrypt (plaintext, msg_len, key, iv,mlecipher);                       
}
void  mleDecrypt(uint8_t *mlekey,unsigned char *mlecipher,unsigned char *recovmsg,int &ciph_len,int &dec_len){
    unsigned char key[KEY_SIZE];
    unsigned char iv[BLOCK_SIZE];
    //Get the MLE KEY
    for(int i = 0; i < HKEY_SIZE; i ++)
        key[i]=mlekey[i];
    //Get the fixed IV    
    for(int i = 0; i < HKEY_SIZE; i ++)
        iv[i]=ivstr[i];
    /*
     * Buffer for ciphertext. Ensure the buffer is long enough for the
     * ciphertext which may be longer than the plaintext, depending on the
     * algorithm and mode.
     */
    unsigned char *ciphertext = (unsigned char *)&mlecipher[0];
    /* Buffer for the decrypted text */
    /* Decrypt the plaintext */
    dec_len = decrypt(ciphertext, ciph_len, key, iv, recovmsg);
    
}
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 128 bit AES (i.e. a 128 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 128 bit AES (i.e. a 128 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}
uint8_t* mleKeygen(vector<uint8_t> &message)
{
    int size = message.size() + SALT_SIZE;
    uint8_t array[size];
    uint8_t *hashString = new uint8_t[SHA256_DIGEST_LENGTH];
    uint8_t *mlekey = new uint8_t[HKEY_SIZE];
    copy(message.begin(), message.end(), array);
    memcpy(&array[size-SALT_SIZE], "YodaSaid,DoOrDoNot.ThereIsNoTry.",SALT_SIZE);
    SHA256(array, size, hashString);
    memcpy(mlekey,hashString,HKEY_SIZE);
    return mlekey;
}
