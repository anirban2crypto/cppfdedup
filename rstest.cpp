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

    std::string inpdir="./../input_bitmap/";
    std::string offdir="./../blkfuzzy/offstore/";
    vector<long> locatn;
    vector<uint8_t> paritydata;
    vector<uint8_t> recovdata;
    vector<uint8_t> inputdata; 
    vector<uint8_t> subsample;
    vector<uint8_t> offsetdata;
    vector<uint8_t> chunkdata;
    vector<int> intoffset;
    bool rowfound=false;
    char c;
    long loc;
    int msg_len;
    const int RS_BLK_SYM=65535;    
    const int RS_LMT_SYM=15000;    
    const int SYM_TO_BYT=2;
    const int CHUNK_SIZE = (RS_BLK_SYM-(2*RS_LMT_SYM))*SYM_TO_BYT;
    int PARITY_SIZE = (2*RS_LMT_SYM)*SYM_TO_BYT;
    int BYTE_SIZE_LOC=ceil(ceil(log2(CHUNK_SIZE))/8);

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
        /*for (int j=0; j<chunkdata.size();j++)
        {
            cout << int(chunkdata[j]);
        }*/ 
            
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
            cout <<" similarity found, parity size "<< paritydata.size() << endl;
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
            cout <<" Reconstruction return code "<< rc<<endl;

            if (rc==-1){
                paritydata.resize(0);
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
            paritydata.resize(0);
            cout<<"before calling genparity"<<endl;
            genparity(chunkdata,paritydata);
            cout<<"after calling genparity"<<endl;  
            // update the parity in database
            // cout << "initial paritydata"<<endl;
            // for (int j=0; j<paritydata.size();j++)
            // {
            //      cout << paritydata[j];
            // }
            insertParity((unsigned char *)&subsample[0],(int)subsample.size(), (unsigned char *)&paritydata[0],(int)paritydata.size());
            // generate MLE KEY

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
    offsetfile.write((char *)&offsetdata[0],offsetdata.size()); 
    //---------------------------------------------------------------------   
    //                    IO CLOSE
    //---------------------------------------------------------------------          
    offsetfile.close(); 

    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
}    