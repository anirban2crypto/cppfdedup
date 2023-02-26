
#include <string>
#include <cstdlib>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>

using namespace std;

void reconst(vector<uint8_t> &erronsdata,vector<uint8_t> &paritydata,vector<uint8_t> &recovdata,vector<int> &intoffset){
    const int rs_n=255,rs_k=243,dis=(rs_n-rs_k);
    long i=0,data_size,edat_cnt=0,pdata_cnt=0;
    std::vector<int> erasures;
    std::vector<int> position;
    std::vector<uint8_t> data;
    ezpwd::RS<rs_n,rs_k> rs;
    data.resize(rs_n);
    int block_no=0;
    long data_remain=erronsdata.size();
    while(data_remain > 0) 
    {   
        block_no++;   
        if (data_remain >= rs_k)        
            data_size=rs_k;
        else
            data_size=data_remain;
        for(i=0;i<data_size;i++)
        {        
            data[i]=erronsdata[edat_cnt];
            edat_cnt++;                                
        }    
        for(i=0;i<dis;i++)
        {
            data[rs_k+i]=paritydata[pdata_cnt];
            pdata_cnt++;            
        }
        int fixed = rs.decode(data,erasures,&position );
        
        for (i=0;i<data_size;i++)
        {
            recovdata.push_back(data[i]);
        }
        intoffset.push_back(block_no);
        intoffset.push_back(position.size());
        for (i=0;i<position.size();i++)
        {
            intoffset.push_back(position[i]);
        }
        data_remain=data_remain-rs_k;
    } 
}