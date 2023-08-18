
#include <string>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <iomanip> 
#include<cmath>
#include <vector>
#include <ezpwd/rs>
#include "dedup.h"
#include "rscode.h"
#include "mlecrypto.h"
#include "cxttab.h"

using namespace std;

//const size_t BYTES_PER_INT = sizeof(int); 
void intToCharArray(unsigned char* buffer, int in,int BYTES_PER_INT)
{
	int x=in;
    std::copy(static_cast<const char*>(static_cast<const void*>(&x)),
          static_cast<const char*>(static_cast<const void*>(&x)) + BYTES_PER_INT,
          buffer);	
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
    vector<uint8_t> chunkdata;
    vector<uint8_t> mlekeydata;
    vector<int> intoffset;
    bool rowfound=false;
    char c;
    long loc;
    int ciph_len,msg_len,cpa_msg_len,cpa_ciph_len;
    uint8_t *cpakey=(unsigned char *)"dsMLtHNGAiNgRTwY"; 
    uint8_t *cpaiv=new uint8_t[BLOCK_SIZE];
    uint8_t *mlekey;
    uint8_t *chunktag;
    unsigned char *cpacipher;
    unsigned char *final_cipher;
    int CHUNK_SIZE = atoi(argv[2])*2;
    int BYTE_SIZE_LOC=ceil(ceil(log2(CHUNK_SIZE))/8);
    //int MAX_OFF_LEN=ECC_ERR_LMT*(BYTE_SIZE_LOC+1);
    auto st_start = std::chrono::high_resolution_clock::now();

    //---------------------------------------------------------------------   
    //                    IO OPEN
    //---------------------------------------------------------------------   

    // read location from param file - subsampling 
    ifstream parfile("param.txt");
    if (!parfile){
        cout << "Could not open file for reading!\n";
        return -1;
    }	
    while(!parfile.eof()){    
         parfile >> loc;
         locatn.push_back(loc);
    }

    // open the file for read  - input message file 
    std::string inpfname=inpdir+std::string(argv[1]);
    ifstream infile (inpfname, ios::binary);
    if (!infile){
        cout << "Could not open input file for reading!\n";
        return -1;
    }
    // open offset file - for write
    std::string offfname=offdir+std::string(argv[1]);
    ofstream offsetfile(offfname, ios::out | ios::trunc);
    if (!offsetfile) {
        cout << "Could not open file for writing!\n";
        return -1;
    }  
     
    // store mlekey file
    std::string keyfname=keydir+std::string(argv[1]);
    ofstream stormlekey(keyfname, ios::out | ios::trunc);
    if (!stormlekey) {
        cout << "Could not open file for writing!\n";
        return -1;
    } 
    std::string mapfname=mapdir+std::string(argv[1]);
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
    infile.close();
    msg_len=inputdata.size();
    
    //---------------------------------------------------------------------   
    //                   DEDUPLICATION PROCESSING
    //---------------------------------------------------------------------   
    int data_size=0,chunk_cnt=0,data_remain=msg_len,inp_data_ptr=0;
    while(data_remain > 0)
    {      
        chunk_cnt++;    
             cout <<"block number:" <<chunk_cnt<<endl;  
        if (data_remain >= CHUNK_SIZE)        
            data_size=CHUNK_SIZE;
        else
            data_size=data_remain;
        for(auto i=0;i<data_size;i++)
        {        
            chunkdata.push_back(inputdata[inp_data_ptr++]);                 
        } 
            
        //---------------------------------------------------------------------   
        //        LABEL,FINGERPRINT GENERATION - FIXED SUBSAMPLING
        //---------------------------------------------------------------------     
        // Create fingerprint of the file using subsampling.
        // Fixed subsample used. Location of subsample is in parfile   
        long sum = 0;
        int avg=0;
        for(int j=0; j<=chunkdata.size(); j++)
        {
            sum=sum+int(chunkdata[j]);
        }
        avg=sum/chunkdata.size();
        unsigned char avgchar=avg;
             cout <<"fingerprint:" <<avg<<endl;
        subsample.push_back(avgchar);
        /*while ( index < locatn.size()) {
            loc = locatn.at(index);
            if (loc <= chunkdata.size())
                subsample.push_back(chunkdata[loc]);
            index++;
        }*/
        //---------------------------------------------------------------------   
        // check parity table in database if any parity symbols exists for the file
        //---------------------------------------------------------------------     
        checkSimilarity((unsigned char *)&subsample[0],(int)subsample.size(),&rowfound,paritydata);        
        if (rowfound ==true)
        {
            cout <<" similarity found "<< endl;
            //---------------------------------------------------------------------   
            //                   SUBSEQUENT ENCRYPTION
            //---------------------------------------------------------------------  
            //---------------------------------------------------------------------   
            //                   BASE GENERATION - decode algorithm
            //---------------------------------------------------------------------  
            int rc=0;
            // cout <<"final paritydata data: "<< endl;
            // for (int j=0; j<paritydata.size();j++)
            // {
            //      cout << paritydata[j];
            // }
            rc=reconst(chunkdata,paritydata,recovdata,intoffset); 
            cout <<" Decode return code "<< rc<<endl;
            if (rc==-1){
                //generate parity
                genparity(chunkdata,paritydata);  
                // update the parity in database
                insertParity((unsigned char *)&subsample[0],(int)subsample.size(), (unsigned char *)&paritydata[0],(int)paritydata.size());

            }
            //---------------------------------------------------------------------   
            //                   OFFSET GENERATION
            //--------------------------------------------------------------------- 
            int offsize=intoffset.size();
            //Include the size of the offset in offset data
            char str_offsize[12];
            sprintf(str_offsize, "%06d", offsize);
            offsetdata.push_back(str_offsize[0]);
            offsetdata.push_back(str_offsize[1]);
            offsetdata.push_back(str_offsize[2]);
            offsetdata.push_back(str_offsize[3]);
            offsetdata.push_back(str_offsize[4]);
            offsetdata.push_back(str_offsize[5]);
            // read offset for block by block
            // read offset block by block, block length is EC parameter - message length   
            int ix=0;
            while( ix < offsize){
                // number of flip/error symbol of a block bound by EC parameter - correction capability        
                int noerr=intoffset[ix++];
                // read all offset one by one, offset is position and flip symbol         
                for(auto k=0;k<noerr;k++){
                    //get the position
                    int loc=intoffset[ix++];        
                    //convert the position from int to byte
                    unsigned char cbyte[BYTE_SIZE_LOC];
                    intToCharArray(cbyte, loc,BYTE_SIZE_LOC);                    
                    for(int i=0;i<BYTE_SIZE_LOC;i++){
                        offsetdata.push_back(cbyte[i]);
                    }
                    //get the flip symbol
                    offsetdata.push_back(inputdata[loc]);
                }                                           
            }
            //---------------------------------------------------------------------   
            //                   MLE ENCRYPTION BASE GET FROM DECODE
            //---------------------------------------------------------------------         
            // MLE KEY GENERATION
            mlekey = mleKeygen(recovdata);
            mlekeydata.insert(mlekeydata.end(), mlekey, mlekey+HKEY_SIZE);
            //initialize MLE ciphertext lenth   
            unsigned char *mlecipher=new unsigned char[data_size+BLOCK_SIZE];
            //MLE ENCRYPT
            mleEncrypt(mlekey,(unsigned char *)&recovdata[0],mlecipher,ciph_len,data_size);                                
            //Generate tag
            vector<uint8_t> Vmleciph(mlecipher, mlecipher + ciph_len);
            chunktag = mleKeygen(Vmleciph);
            //convert tag to hex    
            std::stringstream oss; 
            for ( int j = 0; j < HKEY_SIZE; j++ )
            {   
                oss << std::setfill('0') << std::setw(2) << std::hex << static_cast< int >(chunktag[j]);
            } 
            std::string  taginhex= oss.str();
            mapfile << taginhex;
            //std::string cxtfname=cxtdir+taginhex;  
            //ofstream cxtfile(cxtfname, ios::out | ios::trunc);
            //if (!cxtfile) {
            //    cout << "Could not open file for writing!\n";
            //     return -1;
            //}
            //cxtfile.write((char *)mlecipher,ciph_len);
            //cxtfile.close();
            char *tag = const_cast<char*>(taginhex.c_str());
            insertCxt(tag,taginhex.size(),ciph_len);
        }
        else
        {            
            cout <<"Decode fail, no similarity found "<< endl;
            //---------------------------------------------------------------------   
            //                   INITIAL ENCRYPTION
            //---------------------------------------------------------------------  
            //---------------------------------------------------------------------   
            //                   OFFSET GENERATION
            //---------------------------------------------------------------------
            // add offset size  as zero 
            char str_offsize[12];
            sprintf(str_offsize, "%06d", 0);
            offsetdata.push_back(str_offsize[0]);
            offsetdata.push_back(str_offsize[1]);
            offsetdata.push_back(str_offsize[2]);
            offsetdata.push_back(str_offsize[3]);
            offsetdata.push_back(str_offsize[4]);
            offsetdata.push_back(str_offsize[5]);
            //---------------------------------------------------------------------   
            //                   BASE ENCRYPTION - when no parity exists
            //--------------------------------------------------------------------- 
            //generate parity
            genparity(chunkdata,paritydata);  
            // update the parity in database
            // cout << "initial paritydata"<<endl;
            // for (int j=0; j<paritydata.size();j++)
            // {
            //      cout << paritydata[j];
            // }
            insertParity((unsigned char *)&subsample[0],(int)subsample.size(), (unsigned char *)&paritydata[0],(int)paritydata.size());
            // generate MLE KEY
            mlekey = mleKeygen(chunkdata);
            mlekeydata.insert(mlekeydata.end(), mlekey, mlekey+HKEY_SIZE);
            //initialize MLE ciphertext lenth
            unsigned char *mlecipher=new unsigned char[data_size+BLOCK_SIZE];
            mleEncrypt(mlekey,(unsigned char *)&chunkdata[0],mlecipher,ciph_len,data_size);
            //generate tag of the ciphertext
            vector<uint8_t> Vmleciph(mlecipher, mlecipher + ciph_len);
            chunktag = mleKeygen(Vmleciph); 
            //convert tag to hex   
            std::stringstream oss; 
            for ( int j = 0; j < HKEY_SIZE; j++ )
            {   
                oss << std::setfill('0') << std::setw(2) << std::hex << static_cast< int >(chunktag[j]);
            }             
            std::string  taginhex= oss.str();
            mapfile << taginhex;
            //create ciphertext file
            /*std::string cxtfname=cxtdir+taginhex;  
            ofstream cxtfile(cxtfname, ios::out | ios::trunc);
            if (!cxtfile) {
                cout << "Could not open file for writing!\n";
                return -1;
            }
            cxtfile.write((char *)mlecipher,ciph_len);
            cxtfile.close();*/
            char *tag = const_cast<char*>(taginhex.c_str());
            insertCxt(tag,taginhex.size(),ciph_len);

        }          
        //---------------------------------------------------------------------   
        //                    CHUNK PROCESS END
        //---------------------------------------------------------------------               
        data_remain=data_remain-data_size;
        chunkdata.resize(0);
        subsample.resize(0);
        recovdata.resize(0);
        paritydata.resize(0);
        intoffset.resize(0);   
    }
     cout <<"Number of block:" <<chunk_cnt<<endl;
    //---------------------------------------------------------------------   
    //                   OFFSET ENCRYPTION - CPA encrypt the offset file 
    //---------------------------------------------------------------------        
    /*cout <<"Offset data: "<< endl;
    for (int j=0; j<offsetdata.size();j++)
    {
             cout << offsetdata[j];
    }*/
    cout << endl;
    cpa_msg_len=offsetdata.size(); 
    cpacipher=new unsigned char[cpa_msg_len+BLOCK_SIZE];  
    cpaEncrypt(cpakey,cpaiv,(unsigned char *)&offsetdata[0],cpacipher,cpa_ciph_len,cpa_msg_len);  
    // cancatenate cpaiv cpacipher  == final cipher
    final_cipher = new unsigned char [BLOCK_SIZE + cpa_ciph_len];
    std::copy(cpaiv, cpaiv+BLOCK_SIZE, final_cipher);
    std::copy(cpacipher, cpacipher+cpa_ciph_len, final_cipher +BLOCK_SIZE);
    // write offset file       
    offsetfile.write((char *)final_cipher,BLOCK_SIZE + cpa_ciph_len); 
    //---------------------------------------------------------------------   
    //                  CHUNK KEY ENCRYPTION - cpa encrypt mle key
    //--------------------------------------------------------------------- 
    cpa_msg_len=mlekeydata.size();
    /*cout <<"MLE KEY data: "<< endl;
    for (int j=0; j<mlekeydata.size();j++)
    {
             cout << int(mlekeydata[j]);    
    }
    cout << endl;*/
    cpacipher=new unsigned char[cpa_msg_len+BLOCK_SIZE];  
    cpaEncrypt(cpakey,cpaiv,(unsigned char *)&mlekeydata[0],cpacipher,cpa_ciph_len,cpa_msg_len); 
    // cancatenate cpaiv cpacipher  == final cipher
    final_cipher = new unsigned char [BLOCK_SIZE + cpa_ciph_len];
    std::copy(cpaiv, cpaiv+BLOCK_SIZE, final_cipher);
    std::copy(cpacipher, cpacipher+cpa_ciph_len, final_cipher + BLOCK_SIZE);
    stormlekey.write((char *)final_cipher,BLOCK_SIZE + cpa_ciph_len);   
    //---------------------------------------------------------------------   
    //                    IO CLOSE
    //---------------------------------------------------------------------          
    offsetfile.close();   
    stormlekey.close();
    mapfile.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
}    