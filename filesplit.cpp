#include <string>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <iomanip> 
#include<cmath>
#include <vector>


using namespace std;

int main(int argc, char** argv)
{

    std::string inpdir="./../input_bitmap/";
    vector<uint8_t> inputdata; 
    vector<uint8_t> chunkdata;
    char c;    
    int msg_len;
    int CHUNK_SIZE = 37535*2;    
    //---------------------------------------------------------------------   
    //                    IO OPEN
    //---------------------------------------------------------------------   
    std::string inpfname=inpdir+std::string(argv[1]);
    ifstream infile (inpfname, ios::binary);
    if (!infile){
        cout << "Could not open input file for reading!\n";
        return -1;
    }

    //---------------------------------------------------------------------   
    //                   INPUT READ
    //---------------------------------------------------------------------   
    while (true) {
        c = infile.get();
        if (!infile) 
            break;
        inputdata.push_back(c);
    }
    infile.close();
    msg_len=inputdata.size();
    
    //---------------------------------------------------------------------   
    //                   Block SPLIT
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
        std::string cxtfname=inpdir+std::string(argv[1])+to_string(chunk_cnt);  
        ofstream cxtfile(cxtfname, ios::out | ios::trunc);
        if (!cxtfile) {
            cout << "Could not open file for writing!\n";
            return -1;
        }
        cxtfile.write((char *)&chunkdata[0], data_size);
        cxtfile.close();        

        data_remain=data_remain-data_size;
        chunkdata.resize(0);           
    }              
    cout <<"Number of block:" <<chunk_cnt<<endl;    
}    