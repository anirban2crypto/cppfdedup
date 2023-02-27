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
#include <string.h>
#include "mlecrypto.h"
#define SALT_SIZE 32
#define HKEY_SIZE 16 
static const unsigned int KEY_SIZE = 16;
static const unsigned int BLOCK_SIZE = 16;
using namespace std;
unsigned char *ivstr = (unsigned char *)"DSmlThngaInGrtWy";    
template <typename T>
struct zallocator
{
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    pointer address (reference v) const {return &v;}
    const_pointer address (const_reference v) const {return &v;}

    pointer allocate (size_type n, const void* hint = 0) {
        if (n > std::numeric_limits<size_type>::max() / sizeof(T))
            throw std::bad_alloc();
        return static_cast<pointer> (::operator new (n * sizeof (value_type)));
    }

    void deallocate(pointer p, size_type n) {
        OPENSSL_cleanse(p, n*sizeof(T));
        ::operator delete(p); 
    }
    
    size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof (T);
    }
    
    template<typename U>
    struct rebind
    {
        typedef zallocator<U> other;
    };

    void construct (pointer ptr, const T& val) {
        new (static_cast<T*>(ptr) ) T (val);
    }

    void destroy(pointer ptr) {
        static_cast<T*>(ptr)->~T();
    }

#if __cpluplus >= 201103L
    template<typename U, typename... Args>
    void construct (U* ptr, Args&&  ... args) {
        ::new (static_cast<void*> (ptr) ) U (std::forward<Args> (args)...);
    }

    template<typename U>
    void destroy(U* ptr) {
        ptr->~U();
    }
#endif
};

typedef unsigned char byte;
typedef std::basic_string<char, std::char_traits<char>, zallocator<char> > secure_string;
using EVP_CIPHER_CTX_free_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

void gen_params(byte key[KEY_SIZE], byte iv[BLOCK_SIZE]);
void aes_encrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ptext, secure_string& ctext);
void aes_decrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ctext, secure_string& rtext);

void  mleEncrypt(uint8_t *mlekey,vector<uint8_t> &message,vector<uint8_t> &mlecipher){
    //GEN SHA256 HASH FROM MESSAGE

    // Load the necessary cipher
    EVP_add_cipher(EVP_aes_128_cbc());

    // plaintext, ciphertext, recovered text   
    secure_string ptext((char *)&message[0]);;
    secure_string ctext;
    byte key[KEY_SIZE], iv[BLOCK_SIZE];
    //gen_params(key, iv);

    //Get the MLE KEY
    for(int i = 0; i < HKEY_SIZE; i ++)
        key[i]=mlekey[i];

    //Get the fixed IV    
    for(int i = 0; i < HKEY_SIZE; i ++)
        iv[i]=ivstr[i];

    aes_encrypt(key, iv, ptext, ctext);
    copy(ctext.begin(), ctext.end(), std::back_inserter(mlecipher));
    
    OPENSSL_cleanse(key, KEY_SIZE);
    OPENSSL_cleanse(iv, BLOCK_SIZE);
    //std::cout << "Original message:\n" << ptext << std::endl;
}
void  mleDecrypt(uint8_t *mlekey,vector<uint8_t> &mlecipher,vector<uint8_t> &recovmsg){
    
    // Load the necessary cipher
    EVP_add_cipher(EVP_aes_128_cbc());

    // plaintext, ciphertext, recovered text   
    secure_string rtext;
    secure_string ctext((char *)&mlecipher[0]);

    //declare key and iv
    byte key[KEY_SIZE], iv[BLOCK_SIZE];
    //gen_params(key, iv);

    //Get the MLE KEY
    for(int i = 0; i < HKEY_SIZE; i ++)
        key[i]=mlekey[i];

    //Get the fixed IV    
    for(int i = 0; i < HKEY_SIZE; i ++)
        iv[i]=ivstr[i];
    
    aes_decrypt(key, iv, ctext, rtext);
    
    OPENSSL_cleanse(key, KEY_SIZE);
    OPENSSL_cleanse(iv, BLOCK_SIZE);
    copy(rtext.begin(), rtext.end(), std::back_inserter(recovmsg));
    //std::cout << "Recovered message:\n" << rtext << std::endl;
}
void gen_params(byte key[KEY_SIZE], byte iv[BLOCK_SIZE])
{
    int rc = RAND_bytes(key, KEY_SIZE);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes key failed");

    rc = RAND_bytes(iv, BLOCK_SIZE);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes for iv failed");
}

void aes_encrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ptext, secure_string& ctext)
{
    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_128_cbc(), NULL, key, iv);
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptInit_ex failed");

    // Recovered text expands upto BLOCK_SIZE
    ctext.resize(ptext.size()+BLOCK_SIZE);
    int out_len1 = (int)ctext.size();

    rc = EVP_EncryptUpdate(ctx.get(), (byte*)&ctext[0], &out_len1, (const byte*)&ptext[0], (int)ptext.size());
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptUpdate failed");
  
    int out_len2 = (int)ctext.size() - out_len1;
    rc = EVP_EncryptFinal_ex(ctx.get(), (byte*)&ctext[0]+out_len1, &out_len2);
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptFinal_ex failed");

    // Set cipher text size now that we know it
    ctext.resize(out_len1 + out_len2);
}

void aes_decrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ctext, secure_string& rtext)
{
    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_cbc(), NULL, key, iv);
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptInit_ex failed");

    // Recovered text contracts upto BLOCK_SIZE
    rtext.resize(ctext.size());
    int out_len1 = (int)rtext.size();

    rc = EVP_DecryptUpdate(ctx.get(), (byte*)&rtext[0], &out_len1, (const byte*)&ctext[0], (int)ctext.size());
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptUpdate failed");
  
    int out_len2 = (int)rtext.size() - out_len1;
    rc = EVP_DecryptFinal_ex(ctx.get(), (byte*)&rtext[0]+out_len1, &out_len2);
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptFinal_ex failed");

    // Set recovered text size now that we know it
    rtext.resize(out_len1 + out_len2);
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

