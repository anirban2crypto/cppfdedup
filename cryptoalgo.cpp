#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>
#include <openssl/sha.h>
using namespace std;
uint8_t* saltGen(vector<uint8_t> &message);
#define SALT_SIZE 32 int main(int argc, char** argv)
{
    vector<uint8_t> message;
    if (argc <3){
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
    uint8_t *a = saltGen(message);
    printf("The salted hashString is: \n");
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i ++)
    printf("%2X",a[i]);
    cout << endl;
    delete [] a;
}
uint8_t* saltGen(vector<uint8_t> &message)
{
    int size = message.size() + SALT_SIZE;
    uint8_t array[size];
    uint8_t *hashString = new uint8_t[SHA256_DIGEST_LENGTH];
    copy(message.begin(), message.end(), array);
    memcpy(&array[size-SALT_SIZE], "01234567",SALT_SIZE);
    SHA256(array, size, hashString);
    return hashString;
}


#ifdef COMMENT
#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>
using namespace std;
int main(int argc, char** argv)
{

    vector<uint8_t> message;
    if (argc <3){
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
}   
#endif
