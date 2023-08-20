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
    const int rs_n=65535,rs_k=37535,dis=(rs_n-rs_k),symbol_size=2;
    long i=0,string_size,edat_cnt=0,pdata_cnt=0;
    std::vector<int> erasures;
    std::vector<int> position;
    std::vector<uint16_t> rs_data;
    ezpwd::RS<rs_n,rs_k> rs;
    rs_data.resize(rs_n);
    int block_no=0,p_procss=0;
    long input_size=erronsdata.size();
    int fixed=-1;
    if (input_size < rs_k*2)                
        string_size=ceil(input_size/2);
    else
        string_size=rs_k;
    for(i=0;i<string_size;i++)
    {        
        rs_data[i]=int(erronsdata[2*i+1])+256*int(erronsdata[2*i]);                       
    }    
    int one_p_size=(dis*symbol_size);
    int no_of_p=paritydata.size()/one_p_size;
    while((fixed < 0) && (p_procss < no_of_p))
    { 
        int p_st_indx=p_procss*one_p_size;
        for(i=0;i<dis;i++)
        {        
            rs_data[rs_k+i]=int(paritydata[p_st_indx+2*i+1])+256*int(paritydata[p_st_indx+2*i]);                                                         
        }    
        /*cout <<"RS Data before decode: "<< endl;
        for (int j=0; j<string_size;j++)
        {
            cout << int(rs_data[j]);
        }*/        
        fixed = rs.decode(rs_data,erasures,&position );
        cout <<" Decode return code "<< fixed<<endl;
        /*cout <<"RS Data after decode: "<< endl;
        for (int j=0; j<string_size;j++)
        {
            cout << int(rs_data[j]);
        }*/
        p_procss++;
    }    

    if (fixed == -1)
    {
        return fixed; 
    }
    for (i=0;i<string_size;i++)
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
