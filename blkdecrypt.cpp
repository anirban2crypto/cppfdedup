
#include <string>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <iomanip> 
#include<cmath>
#include <vector>
#include "mlecrypto.h"

using namespace std;


int main(int argc, char** argv)
{


    vector<uint8_t> chunkdata;
    vector<uint8_t> inputdata;
    vector<uint8_t> cipherdata;    
    vector<uint8_t> mlekeydata; 
    char c;
    long loc;
    int ciph_len,map_len,msg_len,cpa_msg_len,cpa_ciph_len,dec_len;
    uint8_t *cpakey=(unsigned char *)"dsMLtHNGAiNgRTwY"; 
    uint8_t *cpaiv=new uint8_t[BLOCK_SIZE];
    uint8_t *mlekey;
    uint8_t *chunktag;
    unsigned char *cpacipher;
    unsigned char *final_cipher;
    unsigned char *inpcipher;
    unsigned char *deccipher;
    unsigned char *keydata;

    //---------------------------------------------------------------------   
    //                    IO OPEN
    //---------------------------------------------------------------------   
    // open the file for read   - input message file -close message file
    std::string mapfname="./mapping/"+std::string(argv[1]);
    ifstream mapfile (mapfname);
    if (!mapfile){
        cerr << "Could not open map file for reading!\n";
        return -1;
    }
    std::string keyfname="./keystore/"+std::string(argv[1]);
    ifstream keyfile (keyfname, ios::binary);
    if (!keyfile){
        cerr << "Could not open key file for reading!\n";
        return -1;
    }

    std::string decfname="./decstore/"+std::string(argv[1]);
    ofstream decfile(decfname,  ios::out | ios::trunc);
    if (!decfile) {
        cerr << "Could not open decrypted file for writing!\n";
        return -1;
    }

    //---------------------------------------------------------------------   
    //                   INPUT READ
    //---------------------------------------------------------------------   
    while (true) {
        c = keyfile.get();
        if (!keyfile) 
            break;
        mlekeydata.push_back(c);
    }
    keydata=(unsigned char *)&mlekeydata[0];
    for(int j=0 ; j<mlekeydata.size() ; j++)
    {
            cout << int(mlekeydata[j]);
    }

    std::string mapdata;
    mapfile >> mapdata;
    map_len=mapdata.length();
    //---------------------------------------------------------------------   
    //                   DEDUPLICATION PROCESSING
    //---------------------------------------------------------------------   
    
    //---------------------------------------------------------------------   
    //                   BASE ENCRYPTION
    //---------------------------------------------------------------------         
    int data_size=0,chunk_cnt=0,data_remain=map_len,inp_data_ptr,chunk_name_size=2*HKEY_SIZE;
    // Read input file 
    chunk_cnt=map_len/chunk_name_size;        
    for ( auto i = 0; i < chunk_cnt; i++)
    {
        std::string chunk_name=mapdata.substr(i*chunk_name_size,chunk_name_size);
        std::string ciphfname="./output/"+chunk_name;
        cerr << "\nFile name: " << ciphfname <<"\n";
        ifstream ciphfile (ciphfname, ios::binary);
        if (!ciphfile){
            cerr << "Could not open cipher file for reading!\n";
            return -1;
        }
        while (true)
        {
            c = ciphfile.get();
            if (!ciphfile) 
                break;
            cipherdata.push_back(c);
        }
        unsigned char *mlekey=new unsigned char [HKEY_SIZE];  
        std::copy(keydata+(HKEY_SIZE*i), keydata+(HKEY_SIZE*(i+1)), mlekey);
        
        for(int j=0 ; j<HKEY_SIZE ; j++)
        {
            cout << int(mlekey[j]);
        }
        ciph_len=cipherdata.size();  
        inpcipher=(unsigned char *)&cipherdata[0];
        unsigned char *recovmsg=new unsigned char[ciph_len]; 
        mleDecrypt(mlekey,inpcipher,recovmsg,ciph_len,dec_len);
        cout << "Decyption length \n"<< dec_len;        
        decfile.write((char *)recovmsg,dec_len);
        cipherdata.resize(0);
    }

}    