
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

    std::string mapdir="./../blkexact/mapping/";
    std::string inpdir="./../input_bitmap/";
    std::string keydir="./../blkexact/keystore/";
    std::string cxtdir="./../blkexact/encstore/";

    vector<uint8_t> inputdata;
    vector<uint8_t> chunkdata;
    char c;
    long loc;
    int ciph_len,msg_len,cpa_msg_len,cpa_ciph_len;
    uint8_t *cpakey=(unsigned char *)"dsMLtHNGAiNgRTwY"; 
    uint8_t *cpaiv=new uint8_t[BLOCK_SIZE];
    uint8_t *mlekey;
    uint8_t *chunktag;
    unsigned char *cpacipher;
    unsigned char *final_cipher;
    

    //---------------------------------------------------------------------   
    //                    IO OPEN
    //---------------------------------------------------------------------   
    int CHUNK_SIZE = atoi(argv[1]);
   // cerr << "chunk size"<<  CHUNK_SIZE <<"\n";
   // open the file for read   - input message file -close message file
    std::string inpfname=inpdir+std::string(argv[2]);
    ifstream infile (inpfname, ios::binary);
    if (!infile){
        cerr << "Could not open input file for reading!\n";
        return -1;
    }
    std::string keyfname=keydir+std::string(argv[2]);
    ofstream keyfile(keyfname, ios::out | ios::trunc);
    if (!keyfile) {
        cerr << "Could not open keyfile for writing!\n";
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

    //cerr << " I am here \n";
    //---------------------------------------------------------------------   
    //                   DEDUPLICATION PROCESSING
    //---------------------------------------------------------------------   
    
    //---------------------------------------------------------------------   
    //                   BASE ENCRYPTION
    //---------------------------------------------------------------------         
    int data_size=0,chunk_cnt=0,data_remain=msg_len,inp_data_ptr=0;
    // Read input file     
    while(data_remain > 0) 
    {          

        //cerr << " reading block \n";
        if (data_remain >= CHUNK_SIZE)        
            data_size=CHUNK_SIZE;
        else
            data_size=data_remain;
        for(auto i=0;i<data_size;i++)
        {        
            chunkdata.push_back(inputdata[inp_data_ptr++]);                 
        }   
        //cerr << "Chunk Data Size " <<    chunkdata.size();
        mlekey = mleKeygen(chunkdata);        
        keyfile.write((char *)mlekey,HKEY_SIZE);


        //initialize MLE ciphertext lenth   
        unsigned char *mlecipher=new unsigned char[data_size+BLOCK_SIZE];
        mleEncrypt(mlekey,(unsigned char *)&chunkdata[0],mlecipher,ciph_len,data_size);                

        vector<uint8_t> Vmleciph(mlecipher, mlecipher + ciph_len);
        chunktag = mleKeygen(Vmleciph);

    
        std::stringstream oss; 
        for ( int j = 0; j < HKEY_SIZE; j++ )
        {   
            oss << std::setfill('0') << std::setw(2) << std::hex << static_cast< int >(chunktag[j]);
        } 
        mapfile << oss.str();
        std::string chunkfname=cxtdir+oss.str();                    
        //cerr << "Chunk File name " <<   chunkfname << "\n";
        //CHUNK TO FILE MAPPING TABLE
        ofstream outfile(chunkfname, ios::out | ios::trunc);
        if (!outfile) {
            cerr << "Could not open file for writing!\n";
            return -1;
        }
        outfile.write((char *)mlecipher,ciph_len);
        outfile.close();
        data_remain=data_remain-data_size;
        chunk_cnt++; 
        chunkdata.resize(0);    
    }    
    infile.close();
    keyfile.close();
    mapfile.close();
}