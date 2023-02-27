#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
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
    vector<uint8_t> mlecipher;
    vector<uint8_t> recovmsg;
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
    mleEncrypt(mlekey,message,mlecipher);    
    mleDecrypt(mlekey,mlecipher,recovmsg);
    for (char i: recovmsg)
        std::cout << i;
    std::cout << endl;
    std::cout << "Recovered message:\n";    
    return 0;
}
