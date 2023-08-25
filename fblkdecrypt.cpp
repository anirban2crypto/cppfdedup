#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream> 
#include<cmath>
#include <vector>
#include "mlecrypto.h"
#include <chrono>


using namespace std;
int buffToInteger(char *buffer,int BYTES_PER_INT)
{
    int out;
    switch (BYTES_PER_INT) {
        case 1:
            out = int((unsigned char)(buffer[0]));
            break;
        case 2:
            out = int((unsigned char)(buffer[0]) << 8 |
              (unsigned char)(buffer[1]));
            break;
        case 3:
            out = int((unsigned char)(buffer[0]) << 16 |
              (unsigned char)(buffer[1]) << 8 |
              (unsigned char)(buffer[2]));
            break;
        case 4:
            out = int((unsigned char)(buffer[0]) << 24 |
              (unsigned char)(buffer[1]) << 16 |
              (unsigned char)(buffer[2]) << 8  |
              (unsigned char)(buffer[3]));
            break;
    }    
    return out;              
}

int main(int argc, char** argv)
{

    std::string mapdir="./../blkfuzzy/mapping/";
    std::string outdir="./../blkfuzzy/decstore/";
    std::string keydir="./../blkfuzzy/keystore/";
    std::string cxtdir="./../blkfuzzy/encstore/";
    std::string offdir="./../blkfuzzy/offstore/";
    vector<uint8_t> bcipher;
    vector<uint8_t> kcipher;
    vector<uint8_t> ocipher; 
    vector<uint8_t> mapdata; 
    vector<uint8_t> offsetdata;
    bool rowfound=false;
    char c;
    long loc;
    int ociph_len,kciph_len,bciph_len,cpa_ciph_len,cpa_dec_len,offset_len,msg_len,ciph_len,dec_len,map_len;
    uint8_t *cpakey=(unsigned char *)"dsMLtHNGAiNgRTwY"; 
    unsigned char *inpcipher;
    unsigned char *deccipher;
    unsigned char *deciv;
    const int RS_BLK_SYM=65535;    
    const int RS_LMT_SYM=15000;    
    const int SYM_TO_BYT=2;
    const int CHUNK_SIZE = (RS_BLK_SYM-(2*RS_LMT_SYM))*SYM_TO_BYT;
    const int TAG_HEX_SIZE=32;
    int PARITY_SIZE = (2*RS_LMT_SYM)*SYM_TO_BYT;
    int BYTE_SIZE_LOC=ceil(ceil(log2(CHUNK_SIZE))/8);
    auto st_start = std::chrono::high_resolution_clock::now();
    //---------------------------------------------------------------------   
    //   IO OPEN MAPFILE, OFFSET CXT FILE, KEY CXT FILE, PLAINTEXT
    //---------------------------------------------------------------------       
    // map file
    std::string mapfname=mapdir+std::string(argv[1]);
    ifstream mapfile(mapfname, ios::binary);
    if (!mapfile) {
        cerr << "Could not open offset cipher file for reading!\n";
        return -1;
    }  
    // offset cipher file
    std::string offfname=offdir+std::string(argv[1]);
    ifstream ocfile(offfname, ios::binary);
    if (!ocfile) {
        cerr << "Could not open offset cipher file for reading!\n";
        return -1;
    }  
    // key cipher file
    std::string keyfname=keydir+std::string(argv[1]);
    ifstream kcfile(keyfname, ios::binary);
    if (!kcfile) {
        cerr << "Could not open key cipher file for reading!\n";
        return -1;
    } 
    //plaintext file
    std::string plnfname=outdir+std::string(argv[1]);
    ofstream outfile(plnfname, ios::out | ios::trunc);
    if (!outfile) {
        cerr << "Could not open output file for writing!\n";
        return -1;
    }
    //---------------------------------------------------------------------   
    //                    READ MAPFILE, OFFSET CXT FILE, KEY CXT FILE
    //---------------------------------------------------------------------   
    // Read the map file
    while (true) {
        c = mapfile.get();
        if (!mapfile) 
            break;
        mapdata.push_back(c);
    }
    map_len=mapdata.size();    
    // Read the offset cipher
    while (true) {
        c = ocfile.get();
        if (!ocfile) 
            break;
        ocipher.push_back(c);
    }
    ociph_len=ocipher.size();
    // Read the mle key cipher
    while (true) {
        c = kcfile.get();
        if (!kcfile) 
            break;
        kcipher.push_back(c);
    }
    kciph_len=kcipher.size();
    //---------------------------------------------------------------------   
    //                  KEY DECRYPTION 
    //---------------------------------------------------------------------   
    // CPA Decrypt encrypted mlekey         
    //  seperate iv from cipher
    //  seperate out  final cipher ===> cpaiv cpacipher 
    inpcipher=(unsigned char *)&kcipher[0];
    cpa_ciph_len=kciph_len-BLOCK_SIZE;
    deccipher=new unsigned char[cpa_ciph_len];  
    deciv=new unsigned char[BLOCK_SIZE];
    unsigned char *mlekey=new unsigned char [cpa_ciph_len+BLOCK_SIZE];  
    std::copy(inpcipher, inpcipher+BLOCK_SIZE, deciv);
    std::copy(inpcipher+BLOCK_SIZE, inpcipher+cpa_ciph_len+BLOCK_SIZE, deccipher);
    cpaDecrypt(cpakey,deciv,deccipher,mlekey,cpa_dec_len,cpa_ciph_len);
    //---------------------------------------------------------------------   
    //                  OFFSET DECRYPTION 
    //---------------------------------------------------------------------       
    // CPA Decrypt offset file 
    //  seperate iv from cipher
    //  seperate out  final cipher ===> cpaiv cpacipher 
    inpcipher=(unsigned char *)&ocipher[0];
    cpa_ciph_len=ociph_len-BLOCK_SIZE;
    deccipher=new unsigned char[cpa_ciph_len];  
    deciv=new unsigned char[BLOCK_SIZE];
    unsigned char *offset=new unsigned char [cpa_ciph_len+BLOCK_SIZE];  
    std::copy(inpcipher, inpcipher+BLOCK_SIZE, deciv);
    std::copy(inpcipher+BLOCK_SIZE, inpcipher+cpa_ciph_len+BLOCK_SIZE, deccipher);
    cpaDecrypt(cpakey,deciv,deccipher,offset,cpa_dec_len,cpa_ciph_len);
    offset_len=cpa_dec_len;
    //---------------------------------------------------------------------   
    //                   OFFSET UNCOMPRESS
    //---------------------------------------------------------------------       
    ofstream ctofile("temp_off.dat.gz", ios::out | ios::trunc);
    ctofile.write((char *)offset,offset_len);
    ctofile.close();
    system("gunzip -f temp_off.dat.gz");        
    offsetdata.resize(0);    
    ifstream tofile ("temp_off.dat", ios::binary);
    while (true) {
        c = tofile.get();
        if (!tofile) 
            break;
        offsetdata.push_back(c);
    }
    tofile.close();  
    //---------------------------------------------------------------------   
    //                  CHUNK PROCESSING 
    //---------------------------------------------------------------------
    for()
    {   
        //---------------------------------------------------------------------   
        //                  BASE CXT FILE READ
        //---------------------------------------------------------------------     
        // open the base ciphertext    
        ifstream bcfile (argv[1], ios::binary);
        if (!bcfile){
            cerr << "Could not open base cipher file for reading!\n";
            return -1;
        }
        // Read the base cipher
        bcipher.resize(0);
        while (true) {
            c = bcfile.get();
            if (!bcfile) 
                break;
            bcipher.push_back(c);
        }
        bciph_len=bcipher.size();
    
        //---------------------------------------------------------------------   
        //                  BASE FILE DECRYPTION 
        //---------------------------------------------------------------------  
        // MLE decrypt the base file
        inpcipher=(unsigned char *)&bcipher[0];
        unsigned char *recovmsg=new unsigned char[bciph_len]; 
        ciph_len=bciph_len; 
        mleDecrypt(mlekey,inpcipher,recovmsg,ciph_len,dec_len);
        //---------------------------------------------------------------------   
        //                  RECONSTRUCTION OF MESSAGE
        //---------------------------------------------------------------------                  
        //Reconstruction original file from basefile and offset file
        int count=0;
        while(count < offset_len)
        {
            char cbyte[BYTE_SIZE_LOC];
            std::copy(offset+count, offset+count+BYTE_SIZE_LOC, cbyte);
            int loc=buffToInteger(cbyte,BYTE_SIZE_LOC);        
            //cerr << "Location : " <<loc <<endl;
            count=count+BYTE_SIZE_LOC;
            if (loc>0)
                recovmsg[loc]=offset[count];
           // cerr << "Symbol : " <<symbol <<endl;
            count=count+1;  
        }
        //---------------------------------------------------------------------   
        //                  WRITE PLAINTEXT
        //---------------------------------------------------------------------  
        outfile.write((char *)recovmsg,dec_len);
    }    
    //---------------------------------------------------------------------   
    //                   IO CLOSE
    //---------------------------------------------------------------------   

    bcfile.close();
    ocfile.close();
    kcfile.close();
    outfile.close();    
    

    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
}