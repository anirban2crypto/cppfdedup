
#include <string>
#include <cstdlib>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>
#include <iomanip>

using namespace std;


void genparity(vector<uint8_t> &msgdata,vector<uint8_t> &paritydata){

    const int rs_n=65535,rs_k=37535,dis=(rs_n-rs_k);
    ezpwd::RS<rs_n,rs_k> rs;
    std::vector<uint16_t> rs_data;
    rs_data.resize(rs_k);
    int string_size,msg_cnt=0;
    long input_size=msgdata.size();
    // Read input file
    if (input_size < rs_k*2)                
        string_size=ceil(input_size/2);
    else
        string_size=rs_k;     
    //while(data_remain > 0) 
    {          

        /*if (data_remain >= rs_k)        
            data_size=rs_k;
        else
            data_size=data_remain;*/
        for(auto i=0;i<string_size;i++)
        {        
            rs_data[i]=(msgdata[2*i+1])+256*int(msgdata[2*i]);                 
        }
        /* for (int j=0; j<string_size;j++)
        {
            std::cout  << std::setfill('0') << std::setw(5) << int(rs_data[j]);
            cout<<endl;
        } */    
        rs.encode(rs_data);        
        for (auto i=0;i<dis;i++){
            paritydata.push_back(int(int(rs_data[rs_k+i])/256));
            paritydata.push_back(int(rs_data[rs_k+i])%256);
        }        
        rs_data.resize(rs_k);//data.size() - rs.nroots() );
        //data_remain=data_remain-rs_k;
        //block_no++; 
    }    
}