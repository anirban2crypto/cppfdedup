
#include <string>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream> 
#include<cmath>
#include <vector>
#include <ezpwd/rs>
#include "dedup.h"
#include "rscode.h"
#include "mlecrypto.h"

using namespace std;

//const size_t BYTES_PER_INT = sizeof(int); 
void intToCharArray(char* buffer, int in,int BYTES_PER_INT)
{
	for (size_t i = 0; i < BYTES_PER_INT; i++) {
		size_t shift = 8 * (BYTES_PER_INT - 1 - i);
		buffer[i] = (in >> shift) & 0xff;
	}	
}
int main(int argc, char** argv)
{

    std::string mapdir="./../blkfuzzy/mapping/";
    std::string inpdir="./../input_bitmap/";
    std::string keydir="./../blkfuzzy/keystore/";
    std::string cxtdir="./../blkfuzzy/encstore/";
    std::string offdir="./../blkfuzzy/offstore/";
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
    int ciph_len,msg_len,cpa_msg_len,cpa_ciph_len;
    uint8_t *cpakey=(unsigned char *)"dsMLtHNGAiNgRTwY"; 
    uint8_t *cpaiv=new uint8_t[BLOCK_SIZE];
    uint8_t *mlekey;
    unsigned char *cpacipher;
    unsigned char *final_cipher;
    int CHUNK_SIZE = 255;
    int BYTE_SIZE_LOC=ceil(ceil(log2(CHUNK_SIZE))/8);
    int MAX_OFF_LEN=ECC_ERR_LMT*(BYTE_SIZE_LOC+1);
    auto st_start = std::chrono::high_resolution_clock::now();

    //---------------------------------------------------------------------   
    //                    IO OPEN
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
    std::string inpfname=inpdir+std::string(argv[1]);
    ifstream infile (inpfname, ios::binary);
    if (!infile){
        cerr << "Could not open input file for reading!\n";
        return -1;
    }
    
    // offset file
    std::string offfname=offdir+std::string(argv[1]);
    ofstream offsetfile(offfname, ios::out | ios::trunc);
    if (!offsetfile) {
        cerr << "Could not open file for writing!\n";
        return -1;
    }  
    // store mlekey file
    std::string keyfname=keydir+std::string(argv[1]);
    ofstream stormlekey(keyfname, ios::out | ios::trunc);
    if (!stormlekey) {
        cerr << "Could not open file for writing!\n";
        return -1;
    } 
    std::string mapfname=mapdir+std::string(argv[2]);
    std::ofstream mapfile(mapfname);


    //---------------------------------------------------------------------   
    //                   INPUT READ
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
    int data_size=0,chunk_cnt=0,data_remain=msg_len,inp_data_ptr=0;
    while(data_remain > 0)
    {            
            
        cerr << " reading block \n";
        
        if (data_remain >= CHUNK_SIZE)        
            data_size=CHUNK_SIZE;
        else
            data_size=data_remain;
        for(auto i=0;i<data_size;i++)
        {        
            chunkdata.push_back(inputdata[inp_data_ptr++]);                 
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


            checkSimilarity((unsigned char *)&subsample[0],(int)subsample.size(),&rowfound,paritydata);        
            if (rowfound ==true)
            {
                //---------------------------------------------------------------------   
                //                   SUBSEQUENT ENCRYPTION
                //---------------------------------------------------------------------  
                //cerr << "Parity found: "<<rowfound<<endl;
                //---------------------------------------------------------------------   
                //                   BASE GENERATION
                //---------------------------------------------------------------------  
                //Run decode algorithm to deduplicate the file
                reconst(inputdata,paritydata,recovdata,intoffset);     
                //cerr << "Reconstruct Size: " <<recovdata.size()<<endl;
                //---------------------------------------------------------------------   
                //                   OFFSET GENERATION
                //--------------------------------------------------------------------- 
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
                        char cbyte[BYTE_SIZE_LOC];
                        intToCharArray(cbyte, loc,BYTE_SIZE_LOC);                    
                        for(int i=0;i<BYTE_SIZE_LOC;i++){
                            offsetdata.push_back(cbyte[i]);
                        }
                        //get the flip symbol
                        offsetdata.push_back(inputdata[loc]);
                    }                                           
                }
                // PAD THE OFFSET
                int OFF_PAD_SIZE=MAX_OFF_LEN - offsetdata.size();
                for(auto ss=0;ss<OFF_PAD_SIZE;ss++)    
                    offsetdata.push_back(0x00);
                //---------------------------------------------------------------------   
                //                   BASE ENCRYPTION
                //---------------------------------------------------------------------         
                // encrypt the base file , get from decode
                mlekey = mleKeygen(recovdata);
                //initialize MLE ciphertext lenth   
                unsigned char *mlecipher=new unsigned char[msg_len+BLOCK_SIZE];
                mleEncrypt(mlekey,(unsigned char *)&recovdata[0],mlecipher,ciph_len,msg_len);                                

                //code file 
                std::string cxtfname=cxtdir+std::string(argv[1]);
                ofstream outfile(cxtfname, ios::out | ios::trunc);
                if (!outfile) {
                    cerr << "Could not open file for writing!\n";
                    return -1;
                }
                outfile.write((char *)mlecipher,ciph_len);
            }
            else
            {
                //---------------------------------------------------------------------   
                //                   INITIAL ENCRYPTION
                //---------------------------------------------------------------------  
                //---------------------------------------------------------------------   
                //                   OFFSET GENERATION
                //--------------------------------------------------------------------- 
                // fill zero in offset file
                for(auto ss=0;ss<MAX_OFF_LEN;ss++)    
                    offsetdata.push_back(0x00);
                //---------------------------------------------------------------------   
                //                   BASE ENCRYPTION
                //--------------------------------------------------------------------- 
                //initial encryption - when no parity exists for fingerprint
                //generate parity
                genparity(inputdata,paritydata);    
                // update the parity in database
                insertParity((unsigned char *)&subsample[0],(int)subsample.size(), (unsigned char *)&paritydata[0],(int)paritydata.size());
                //MLE Encrypt the input file  which is base file 
                mlekey = mleKeygen(inputdata);
                //initialize MLE ciphertext lenth
                unsigned char *mlecipher=new unsigned char[msg_len+BLOCK_SIZE];
                mleEncrypt(mlekey,(unsigned char *)&inputdata[0],mlecipher,ciph_len,msg_len);
                outfile.write((char *)mlecipher,ciph_len);
                
            }
            // CPA encrypt the offset file 
            //---------------------------------------------------------------------   
            //                   OFFSET ENCRYPTION
            //--------------------------------------------------------------------- 
            cpa_msg_len=offsetdata.size();
            cpacipher=new unsigned char[cpa_msg_len+BLOCK_SIZE];  
            cpaEncrypt(cpakey,cpaiv,(unsigned char *)&offsetdata[0],cpacipher,cpa_ciph_len,cpa_msg_len);  
            // cancatenate cpaiv cpacipher  == final cipher
            final_cipher = new unsigned char [BLOCK_SIZE + cpa_ciph_len];
            std::copy(cpaiv, cpaiv+BLOCK_SIZE, final_cipher);
            std::copy(cpacipher, cpacipher+cpa_ciph_len, final_cipher +BLOCK_SIZE);
            // write offset file
            offsetfile.write((char *)final_cipher,BLOCK_SIZE + cpa_ciph_len);
            //cpa encrypt mle key
            //---------------------------------------------------------------------   
            //                   KEY ENCRYPTION
            //--------------------------------------------------------------------- 
            cpa_msg_len=KEY_SIZE;
            cpacipher=new unsigned char[cpa_msg_len+BLOCK_SIZE];  
            cpaEncrypt(cpakey,cpaiv,mlekey,cpacipher,cpa_ciph_len,cpa_msg_len); 
            // cancatenate cpaiv cpacipher  == final cipher
            final_cipher = new unsigned char [BLOCK_SIZE + cpa_ciph_len];
            std::copy(cpaiv, cpaiv+BLOCK_SIZE, final_cipher);
            std::copy(cpacipher, cpacipher+cpa_ciph_len, final_cipher + BLOCK_SIZE);
            stormlekey.write((char *)final_cipher,BLOCK_SIZE + cpa_ciph_len);   
            //---------------------------------------------------------------------   
            //                    IO CLOSE
            //---------------------------------------------------------------------               
            outfile.close();
            offsetfile.close();
        }
        infile.close();
        stormlekey.close();
        auto st_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
        std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
    }    