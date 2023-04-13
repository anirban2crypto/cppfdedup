
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
    uint8_t *cpakey=new uint8_t[KEY_SIZE];
    uint8_t *cpaiv=new uint8_t[BLOCK_SIZE];
    uint8_t *k_cpakey=new uint8_t[KEY_SIZE];
    uint8_t *k_cpaiv=new uint8_t[BLOCK_SIZE];
    

    // create database
    //setup();
    auto st_start = std::chrono::high_resolution_clock::now();

    //---------------------------------------------------------------------   
    //                    IO
    //---------------------------------------------------------------------   

    // read location from param file - subsampling 
    ifstream parfile("param.txt");
    if (!parfile){
        cerr << "Could not open file for reading!\n";
        return -1;
    }	
    while(!parfile.eof()){    
         parfile >> loc;
         locatn.push_back(loc);
    }

    // open the file for read   - input message file -close message file
    ifstream infile (argv[1], ios::binary);
    if (!infile){
        cerr << "Could not open input file for reading!\n";
        return -1;
    }
    //code file 
    ofstream outfile(argv[2], ios::out | ios::trunc);
    if (!outfile) {
        cerr << "Could not open file for writing!\n";
        return -1;
    }
    // offset file
    ofstream offsetfile(argv[3], ios::out | ios::trunc);
    if (!offsetfile) {
        cerr << "Could not open file for writing!\n";
        return -1;
    }  
    // store mlekey file
    ofstream stormlekey(argv[3], ios::out | ios::trunc);
    if (!stormlekey) {
        cerr << "Could not open file for writing!\n";
        return -1;
    } 
    //---------------------------------------------------------------------   
    //                   LABEL GENERATION - FIXED SUBSAMPLING
    //---------------------------------------------------------------------     

    // Create fingerprint of the file using subsampling. Fixed subsample used. Location of subsample is in parfile   
    int index = 0;
    while ( index < locatn.size() && (!infile.eof())) {
        loc = locatn.at(index);
        infile.seekg(loc, ios::beg);
        subsample.push_back(infile.get());
        index++;
    }
    infile.clear();   
	infile.seekg( 0, std::ios_base::beg );


    //---------------------------------------------------------------------   
    //                   IO
    //---------------------------------------------------------------------   

    // Re read the input file for deduplication
    while (true) {
        c = infile.get();
        if (!infile) 
            break;
        inputdata.push_back(c);
    }
    infile.clear();   //  Since ignore will have set eof.
	infile.seekg( 0, std::ios_base::beg ); 
    msg_len=inputdata.size();

    //---------------------------------------------------------------------   
    //                   DEDUPLICATION PROCESSING
    //---------------------------------------------------------------------   


    // check parity table in database if any parity symbols exists for the file
    // subsampling is used to generate the label
    checkSimilarity((unsigned char *)&subsample[0],(int)subsample.size(),&rowfound,paritydata);        
    if (rowfound ==true)
    {
        //subsequent encryption
        //Run decode algorithm to deduplicate the file
        cerr << "Parity found: "<<rowfound<<endl;
        reconst(inputdata,paritydata,recovdata,intoffset);     
        cerr << "Reconstruct Size: " <<recovdata.size()<<endl;
        //format the offset as byte string
        int offsize=intoffset.size();
        int ix=0;
        // read offset for block by block
        // read offset block by block, block length is EC parameter - message length   
        while( ix < offsize){
            // number of flip/error symbol of a block bound by EC parameter - correction capability        
            int noerr=intoffset[ix++];
            // read all offset one by one, offset is position and flip symbol         
            for(auto k=0;k<noerr;k++){
                //get the position
                int loc=intoffset[ix++];        
                //convert the position from int to byte
	            intToCharArray(cbyte, loc);                    
                for(int i=0;i<BYTES_PER_INT;i++){
                    offsetdata.push_back(cbyte[i]);
                }
                //get the flip symbol
                offsetdata.push_back(inputdata[loc]);
            }                                           
        }
        // encrypt the base file , get from decode
        uint8_t *mlekey = mleKeygen(recovdata);
        //initialize MLE ciphertext lenth   
        unsigned char *mlecipher=new unsigned char[msg_len+BLOCK_SIZE];
        mleEncrypt(mlekey,(unsigned char *)&recovdata[0],mlecipher,ciph_len,msg_len); 
        outfile.write((char *)mlecipher,ciph_len);
        // CPA encrypt the offset file 
        offset_len=offsetdata.size();
        unsigned char *offcipher=new unsigned char[offset_len+BLOCK_SIZE];  
        cpaEncrypt(cpakey,cpaiv,(unsigned char *)&offsetdata[0],offcipher,ociph_len,offset_len);  
        offsetfile.write((char *)offcipher,ociph_len);
        //cpa encrypt mle key
        mlekey_len=KEY_SIZE;
        unsigned char *keycipher=new unsigned char[KEY_SIZE+BLOCK_SIZE];  
        cpaEncrypt(K_cpakey,K_cpaiv,mlekey,keycipher,kciph_len,mlekey_len); 
        offsetfile.write((char *)keycipher,kciph_len);   
    }
    else
    {
        //initial encryption - when no parity exists for fingerprint
        //generate parity
        genparity(inputdata,paritydata);    
        // update the parity in database
        insertParity((unsigned char *)&subsample[0],(int)subsample.size(), (unsigned char *)&paritydata[0],(int)paritydata.size());
        //MLE Encrypt the input file  which is base file 
        uint8_t *mlekey = mleKeygen(inputdata);
        //initialize MLE ciphertext lenth
        unsigned char *mlecipher=new unsigned char[msg_len+BLOCK_SIZE];
        mleEncrypt(mlekey,(unsigned char *)&inputdata[0],mlecipher,ciph_len,msg_len);  
        outfile.write((char *)mlecipher,ciph_len);
        // fill zero in offset file
        int offset_f_len=ERR_H_CAP*5;
        for(auto ss=0;ss<offset_f_len;ss++)    
            offsetdata.push_back(0x00);
        //CPA Encrypt the offset file 
        offset_len=offsetdata.size();
        unsigned char *offcipher=new unsigned char[offset_len+BLOCK_SIZE];  
        cpaEncrypt(cpakey,cpaiv,(unsigned char *)&offsetdata[0],offcipher,ociph_len,offset_len); 
        offsetfile.write((char *)offcipher,ociph_len);    
        //cpa encrypt mle key
        mlekey_len=KEY_SIZE;
        unsigned char *keycipher=new unsigned char[KEY_SIZE+BLOCK_SIZE];  
        cpaEncrypt(k_cpakey,K_cpaiv,mlekey,keycipher,kciph_len,mlekey_len); 
        offsetfile.write((char *)keycipher,kciph_len);      
    }


    infile.close();
    outfile.close();
    offsetfile.close();
    stormlekey.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
}