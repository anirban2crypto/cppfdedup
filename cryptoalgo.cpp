#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <memory.h>
#include <openssl/sha.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include "mlecrypto.h"
using namespace std;
// g++ -Wall -std=c++11 evp-encrypt.cxx -o evp-encrypt.exe -lcrypto
int main(int argc, char* argv[])
{
    vector<uint8_t> message;
    uint8_t *cpakey=new uint8_t[KEY_SIZE];
    uint8_t *cpaiv=new uint8_t[BLOCK_SIZE];
    int ciph_len,msg_len,dec_len;
    auto st_start = std::chrono::high_resolution_clock::now();
    if (argc < 2){
        cerr << "Please enter the file name\n";
        return -1;
    }
    ifstream datafile (argv[1], ios::binary);
    if (!datafile) {
        cerr << "Could not open data file for reading!\n";
        return -1;
    }
    char c;
    while (true) {
        c = datafile.get();
        if (!datafile)
            break;
        message.push_back(c);
    }
    uint8_t *mlekey = mleKeygen(message);
    std::cout << "Original message:\n" ;
    for (char i: message)
        std::cout << i;
    std::cout << endl; 
    
    msg_len=message.size();   
    unsigned char *mlecipher=new unsigned char[msg_len+BLOCK_SIZE];
    mleEncrypt(mlekey,(unsigned char *)&message[0],mlecipher,ciph_len,msg_len);  
    unsigned char *recovmsg=new unsigned char[ciph_len+BLOCK_SIZE];  
    mleDecrypt(mlekey,mlecipher,recovmsg,ciph_len,dec_len);
    /*cout << "MLE Decryption"<<endl;
    for(int i=0 ; i<dec_len ; i++)
    {
        cout << recovmsg[i];
    }
    cout << endl;*/
    /*for (char i: recovmsg)
        std::cout << recovmsg[i];
    std::cout << endl;
    std::cout << "Recovered message:\n";  

    //cpa encryption

    std::cout << "Original message:\n" ;
    for (char i: message)
        std::cout << i;
    std::cout << endl;    
    msg_len=message.size();*/
    unsigned char *cpacipher=new unsigned char[msg_len+BLOCK_SIZE];  
    cpaEncrypt(cpakey,cpaiv,(unsigned char *)&message[0],cpacipher,ciph_len,msg_len); 

    //  cancatenate cpaiv cpacipher  == final cipher
    unsigned char* final_cipher = new unsigned char [BLOCK_SIZE + msg_len+BLOCK_SIZE];
    std::copy(cpaiv, cpaiv+BLOCK_SIZE, final_cipher);
    std::copy(cpacipher, cpacipher+msg_len+BLOCK_SIZE, final_cipher + BLOCK_SIZE);

    //  some thing 
    //  seperate out  final cipher ===> cpaiv cpacipher 
    unsigned char *deccipher=new unsigned char[msg_len+BLOCK_SIZE];  
    unsigned char *deciv=new unsigned char[BLOCK_SIZE];
    unsigned char *cparecovmsg=new unsigned char [ciph_len+BLOCK_SIZE];  
    std::copy(final_cipher, final_cipher+BLOCK_SIZE, deciv);
    std::copy(final_cipher+BLOCK_SIZE, final_cipher+msg_len+BLOCK_SIZE+BLOCK_SIZE, deccipher);
    cpaDecrypt(cpakey,deciv,deccipher,cparecovmsg,dec_len,ciph_len);
    cout <<"CPA Decryption:"<< endl;
    for(int i=0 ; i<dec_len ; i++)
    {
        cout << cparecovmsg[i];
    }
    cout << endl;
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
    return 0;
}
