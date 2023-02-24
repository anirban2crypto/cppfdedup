
#include <string>
#include <cstdlib>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>

using namespace std;


void genparity(istream &infile,vector<uint8_t> &parity){
    long i=0;
    const int rs_n=255,rs_k=243,dis=(rs_n-rs_k);
    ezpwd::RS<rs_n,rs_k> rs;
    std::vector<uint8_t> data;
    data.resize(rs_k);
    while(!infile.eof())
    {    
        for(i=0;i<rs_k;i++){
            if(infile.eof())
            {                
                break;
            }
            data[i]=infile.get();            
        }
        rs.encode(data);         
        for (i=0;i<dis;i++){
            parity.push_back(data[rs_k+i]);
        }        
        data.resize(data.size() - rs.nroots() );
    }       
}