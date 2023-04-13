#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream> 
#include<cmath>
#include <vector>
#include <ezpwd/rs>
#include "dedup.h"
#include "rscode.h"
#include "mlecrypto.h"

using namespace std;

const size_t BYTES_PER_INT = sizeof(int); 
void intToCharArray(char* buffer, int in)
{
	for (size_t i = 0; i < BYTES_PER_INT; i++) {
		size_t shift = 8 * (BYTES_PER_INT - 1 - i);
		buffer[i] = (in >> shift) & 0xff;
	}	
}
int main(int argc, char** argv)
{

    vector<long> locatn;
    vector<uint8_t> paritydata;
    vector<uint8_t> recovdata;
    vector<uint8_t> inputdata; 
    vector<uint8_t> subsample;
    vector<uint8_t> offsetdata;
    vector<int> intoffset;
    bool rowfound=false;
    char c;
    long loc;
    char cbyte[BYTES_PER_INT];
    int ociph_len,kciph_len,ciph_len,msg_len,offset_len,mlekey_len;
    uint8_t *cpakey=(unsigned char *)"dsMLtHNGAiNgRTwY"; 
    uint8_t *cpaiv=new uint8_t[BLOCK_SIZE];
    uint8_t *k_cpakey=new uint8_t[KEY_SIZE];
    uint8_t *k_cpaiv=new uint8_t[BLOCK_SIZE];
    

    // create database
    //setup();
    auto st_start = std::chrono::high_resolution_clock::now();

    //---------------------------------------------------------------------   
    //                    IO OPEN FILE
    //---------------------------------------------------------------------   

    // open the file for read   - basefile/code file/mle ciphertext
    ifstream basefile (argv[1], ios::binary);
    if (!basefile){
        cerr << "Could not open base file for reading!\n";
        return -1;
    }    
    // offset file
    ofstream offsetfile(argv[2], ios::binary);
    if (!offsetfile) {
        cerr << "Could not open offset file for reading!\n";
        return -1;
    }  
    // store mlekey file
    ofstream stormlekey(argv[3], ios::binary);
    if (!stormlekey) {
        cerr << "Could not open encrypted mlekey file for reading!\n";
        return -1;
    } 
    //final output file/ message file/ reconstructed file
    ofstream outfile(argv[4], ios::out | ios::trunc);
    if (!outfile) {
        cerr << "Could not open output file for writing!\n";
        return -1;
    }

    //---------------------------------------------------------------------   
    //                   IO READ FILE 
    //---------------------------------------------------------------------   

    // Re read the input file for deduplication
    while (true) {
        c = infile.get();
        if (!infile) 
            break;
        inputdata.push_back(c);
    }
    msg_len=inputdata.size();

    //---------------------------------------------------------------------   
    //                   DEDUPLICATION PROCESSING
    //---------------------------------------------------------------------   

        
        // seperate iv from cipher
        // CPA Decrypt encrypted mlekey 
        // get the mle key


        // MLE decrypt the base file

        // seperate iv from cipher
        // CPA Decrypt offset file 
        
        //Reconstruction original file from basefile and offset file 


        uint8_t *mlekey = mleKeygen(recovdata);
        //initialize MLE ciphertext lenth   
        unsigned char *mlecipher=new unsigned char[msg_len+BLOCK_SIZE];
        mleEncrypt(mlekey,(unsigned char *)&recovdata[0],mlecipher,ciph_len,msg_len); 
        outfile.write((char *)mlecipher,ciph_len);

        // CPA encrypt the offset file 
        offset_len=offsetdata.size();
        unsigned char *offcipher=new unsigned char[offset_len+BLOCK_SIZE];  
        cpaEncrypt(cpakey,cpaiv,(unsigned char *)&offsetdata[0],offcipher,ociph_len,offset_len);  
        // add the random iv to cpa cipher
        unsigned char* final_cipher = new unsigned char [BLOCK_SIZE + msg_len+BLOCK_SIZE];
        std::copy(cpaiv, cpaiv+BLOCK_SIZE, final_cipher);
        std::copy(offcipher, offcipher+msg_len+BLOCK_SIZE, final_cipher + BLOCK_SIZE);
        // write offset file
        offsetfile.write((char *)final_cipher,ociph_len+BLOCK_SIZE);


        //cpa encrypt mle key
        mlekey_len=KEY_SIZE;
        unsigned char *keycipher=new unsigned char[KEY_SIZE+BLOCK_SIZE];  
        cpaEncrypt(k_cpakey,k_cpaiv,mlekey,keycipher,kciph_len,mlekey_len); 
        // add the random iv to cpa cipher
        final_cipher = new unsigned char [BLOCK_SIZE + msg_len+BLOCK_SIZE];
        std::copy(cpaiv, cpaiv+BLOCK_SIZE, final_cipher);
        std::copy(keycipher, keycipher+msg_len+BLOCK_SIZE, final_cipher + BLOCK_SIZE);
        stormlekey.write((char *)keycipher,kciph_len+BLOCK_SIZE);   

    }


    infile.close();
    outfile.close();
    offsetfile.close();
    stormlekey.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
}