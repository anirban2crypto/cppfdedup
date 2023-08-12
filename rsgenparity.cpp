
#include <string>
#include <cstdlib>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>

using namespace std;


void genparity(vector<uint8_t> &msgdata,vector<uint8_t> &paritydata){

    const int rs_n=2047,rs_k=1947,dis=(rs_n-rs_k);
    ezpwd::RS<rs_n,rs_k> rs;
    std::vector<uint8_t> data;
    data.resize(rs_k);
    int block_no=0,data_size,msg_cnt=0;
    long data_remain=msgdata.size();
    // Read input file     
    while(data_remain > 0) 
    {          

        if (data_remain >= rs_k)        
            data_size=rs_k;
        else
            data_size=data_remain;
        for(auto i=0;i<data_size;i++)
        {        
            data[i]=msgdata[msg_cnt++];                 
        }    
        rs.encode(data);         
        for (auto i=0;i<dis;i++){
            paritydata.push_back(data[rs_k+i]);
        }        
        data.resize(data.size() - rs.nroots() );
        data_remain=data_remain-rs_k;
        block_no++; 
    }    
}