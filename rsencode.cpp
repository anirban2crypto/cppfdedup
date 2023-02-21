

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

    const int rs_n=255,rs_k=243,dis=(rs_n-rs_k);
    long i=0;
    std::vector<int> erasures; // Report second symbol missing
    std::vector<int> position; // And get back corrected symbols here
    ezpwd::RS<rs_n,rs_k> rs;
    std::vector<uint8_t> data;
    data.resize(rs_k);



    auto st_start = std::chrono::high_resolution_clock::now();
    if (argc <3){
        cerr << "Please enter the file name\n";
        return -1;
    }
    
    
	ofstream outfile(argv[2], ios::out | ios::trunc);
    if (!outfile) {
        cerr << "Could not open file for writing!\n";
        return -1;
    } 
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
        rs.encode( data );         
        unsigned char *buffer= new unsigned char[dis];
        for (i=0;i<dis;i++)
        {
            buffer[i]=data[rs_k+i];
        }
        outfile.write(reinterpret_cast<const char *>(buffer),dis);
        delete [] buffer; 
        data.resize( data.size() - rs.nroots() );
    }          
    infile.close();
    outfile.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
    return 0;

}