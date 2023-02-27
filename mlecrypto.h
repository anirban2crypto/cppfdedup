#ifndef __CRYPTO_H
#define __CRYPTO_H
using namespace std; 
uint8_t* mleKeygen(vector<uint8_t> &message);
void  mleDecrypt(uint8_t *mlekey,vector<uint8_t> &mlecipher,vector<uint8_t> &recovmsg);
void  mleEncrypt(uint8_t *mlekey,vector<uint8_t> &message,vector<uint8_t> &mlecipher);
#endif

