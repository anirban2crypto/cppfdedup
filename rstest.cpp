

#include <string>
#include <cstdlib>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>

using namespace std;
int main(int argc, char** argv)
{

    const int rs_n=255,rs_k=243;
    long i=0;
    std::vector<int> erasures; // Report second symbol missing
    std::vector<int> position; // And get back corrected symbols here
    ezpwd::RS<rs_n,rs_k> rs;
    std::vector<uint8_t> data;
    data.resize(rs_k);
    auto st_start = std::chrono::high_resolution_clock::now();
    ifstream infile (argv[1], ios::binary);
    if (!infile)
    {
              cerr << "Could not open file for reading!\n";
              return -1;
    }   
     
    while(!infile.eof())
    {
    
        for(i=0;i<rs_k;i++){
            if(infile.eof())
            {
                break;
            }
            data[i]=infile.get();
        }
        rs.encode( data ); // Adds 4 Reed-Solomon parity symbols (255-251 == 4)    
        data[0]=77;
        data[1]=78;
        data[2]=79;
        data[4]=77;
        data[5]=78;
        data[6]=79;
        data[7]=77;
        data[8]=78;
        data[9]=79;
        cerr << "Before decode"  <<endl;
        for(int k=0;k<rs_k;k++){
            cerr << int(data[k]) ;
        }
        cerr  <<endl;
        int fixed = rs.decode(data,erasures,&position );		// Correct errors, and
        cerr << "Decode return: " << fixed <<  endl;    
        data.resize( data.size() - rs.nroots() );	// Discard the 4 R-S parity symbol
        //cerr << "Decode return: " << fixed <<  endl;    
        /*for (int i: position)
            std::cout << i << ' ';
        cerr <<  endl;              */
        cerr << "after decode"  <<endl;
        for(int k=0;k<rs_k;k++){
            cerr << int(data[k]);
        }
        cerr  <<endl;
    }          
    infile.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total Setup stage elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
    return 0;

}