
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
    int ciph_len,msg_len;

    // create database
    //setup();
    auto st_start = std::chrono::high_resolution_clock::now();

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

    // Create fingerprint of the file using subsampling   
    int index = 0;
    while ( index < locatn.size() && (!infile.eof())) {
        loc = locatn.at(index);
        infile.seekg(loc, ios::beg);
        subsample.push_back(infile.get());
        index++;
    }
    infile.clear();   
	infile.seekg( 0, std::ios_base::beg );

    // Read input file 
    while (true) {
        c = infile.get();
        if (!infile) 
            break;
        inputdata.push_back(c);
    }
    infile.clear();   //  Since ignore will have set eof.
	infile.seekg( 0, std::ios_base::beg ); 
    msg_len=inputdata.size();   
    unsigned char *mlecipher=new unsigned char[msg_len+BLOCK_SIZE];

    // check parity table in database if any parity symbols exists for the file
    checkSimilarity((unsigned char *)&subsample[0],(int)subsample.size(),&rowfound,paritydata);        
    if (rowfound ==true)
    {
        //subsequent encryption
        //Run decode algorithm to deduplicate the file
        cerr << "Parity found: "<<rowfound<<endl;
        reconst(inputdata,paritydata,recovdata,intoffset);     
        cerr << "Reconstruct Size: " <<recovdata.size()<<endl;
        int offsize=intoffset.size();
        int ix=0;
        while( ix < offsize){            
            int noerr=intoffset[ix++];        
            for(auto k=0;k<noerr;k++){
                int loc=intoffset[ix++];        
	            intToCharArray(cbyte, loc);                    
                for(int i=0;i<BYTES_PER_INT;i++){
                    offsetdata.push_back(cbyte[i]);
                }
                offsetdata.push_back(inputdata[loc]);
            }                                           
        }
        offsetfile.write((char *)&offsetdata[0],offsetdata.size());
        uint8_t *mlekey = mleKeygen(recovdata);
        mleEncrypt(mlekey,(unsigned char *)&recovdata[0],mlecipher,ciph_len,msg_len); 
        //MLE Encrypt the recovdata - code file
    }
    else
    {
        //initial encryption - when no parity exists for fingerprint
        genparity(inputdata,paritydata);    
        insertParity((unsigned char *)&subsample[0],(int)subsample.size(), (unsigned char *)&paritydata[0],(int)paritydata.size());
        //MLE Encrypt the input file - initial encryption - inputdata
        uint8_t *mlekey = mleKeygen(inputdata);
        mleEncrypt(mlekey,(unsigned char *)&inputdata[0],mlecipher,ciph_len,msg_len);  
        
    }

    outfile.write((char *)mlecipher,ciph_len);
    infile.close();
    outfile.close();
    offsetfile.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
}