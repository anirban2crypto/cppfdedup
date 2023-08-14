#include <string>
#include <cstdlib>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>
#include <iomanip>

using namespace std;

int reconst(vector<uint8_t> &erronsdata,vector<uint8_t> &paritydata,vector<uint8_t> &recovdata,vector<int> &intoffset){
    const int rs_n=65535,rs_k=35535,dis=(rs_n-rs_k);
    long i=0,string_size,edat_cnt=0,pdata_cnt=0;
    std::vector<int> erasures;
    std::vector<int> position;
    std::vector<uint16_t> rs_data;
    ezpwd::RS<rs_n,rs_k> rs;
    rs_data.resize(rs_n);
    int block_no=0;
    long input_size=erronsdata.size();
    int fixed=0;
    if (input_size < rs_k*2)                
        string_size=ceil(input_size/2);
    else
        string_size=rs_k;
    for(i=0;i<string_size;i++)
    {        
        rs_data[i]=int(erronsdata[2*i+1])+256*int(erronsdata[2*i]);                       
    }    
    for(i=0;i<dis;i++)
    {        
        rs_data[rs_k+i]=int(paritydata[2*i+1])+256*int(paritydata[2*i]);                                                         
    }
            /*cerr <<"Before Decode "<< endl;            
            for (int j=0; j<data.size();j++)
            {
                 cerr << data[j];
            }
            cerr <<endl;*/               
    fixed = rs.decode(rs_data,erasures,&position );
            /*cerr <<"After decode: "<< endl;            
            for (int j=0; j<data.size();j++)
            {
                 cerr << data[j];
            }
            cerr <<endl;*/    
    for (i=0;i<string_size;i+=2)
    {
        recovdata.push_back(int(int(rs_data[i])/256));
        recovdata.push_back(int(rs_data[i])%256);
    }           
    if(position.size() > 0)
    {            
        intoffset.push_back(2*position.size());
        for (i=0;i<position.size();i++)
        {
            intoffset.push_back(2*position[i]);
            intoffset.push_back(2*position[i]+1);
        }    
    }    
    return fixed; 
}
