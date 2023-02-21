

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
    long i=0,data_size;
    std::vector<int> erasures; // Report second symbol missing
    std::vector<int> position; // And get back corrected symbols here
    ezpwd::RS<rs_n,rs_k> rs;
    std::vector<uint8_t> data;
    data.resize(rs_n);

    auto st_start = std::chrono::high_resolution_clock::now();
    if (argc <3){
        cerr << "Please enter the file name\n";
        return -1;
    }
    
	ifstream datafile (argv[1], ios::binary);
    if (!datafile) {
        cerr << "Could not open data file for reading!\n";
        return -1;
    } 
    ifstream parityfile (argv[2], ios::binary);
    if (!parityfile)
    {
        cerr << "Could not open parity file for reading!\n";
        return -1;
    }   
    ofstream outfile(argv[3], ios::out | ios::trunc);
    if (!outfile) {
        cerr << "Could not open file for writing!\n";
        return -1;
    } 
    while(!datafile.eof() || !parityfile.eof())
    {
        for(i=0;i<rs_k;i++)
        {
            if(datafile.eof())
            {   
                i--;             
                break;
            }
            data[i]=datafile.get();
            
        }
        data_size=i;  
        char * pbuffer = new char [dis];
        parityfile.read (pbuffer,dis);
        for(i=0;i<dis;i++)
        {

            data[rs_k+i]=pbuffer[i];
            
        }
        delete [] pbuffer; 
        int fixed = rs.decode(data,erasures,&position );		// Correct errors, and
        unsigned char *buffer= new unsigned char[rs_k];
        for (i=0;i<rs_k;i++)
        {
            buffer[i]=data[i];
        }
        outfile.write(reinterpret_cast<const char *>(buffer),data_size);
        delete [] buffer;   
    }          
    datafile.close();
    parityfile.close();
    outfile.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
    return 0;

}