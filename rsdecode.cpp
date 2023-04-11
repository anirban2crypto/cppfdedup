#include <string>
#include <cstdlib>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>
#include "rscode.h"
using namespace std;
int main(int argc, char** argv)
{

    vector<uint8_t> recovdata;
    vector<uint8_t> paritydata;
    vector<uint8_t> erronsdata;  
    vector<int> intoffset;
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
    char c;
    while (true) {
        c = datafile.get();
        if (!datafile) 
            break;
        erronsdata.push_back(c);
    }
    cerr << "Erroneous Data Size: " <<erronsdata.size()<<endl;
    ifstream parityfile (argv[2], ios::binary);
    if (!parityfile)
    {
        cerr << "Could not open parity file for reading!\n";
        return -1;
    }
    while (true) {
        c = parityfile.get();
        if (!parityfile) 
            break;
        paritydata.push_back(c);
    }
    cerr << "Parity Size: " <<paritydata.size()<<endl;
    ofstream outfile(argv[3], ios::out | ios::trunc);
    if (!outfile) {
        cerr << "Could not open file for writing!\n";
        return -1;
    } 
    reconst(erronsdata,paritydata,recovdata,intoffset);     
    cerr << "Reconstruct Size: " <<recovdata.size()<<endl;
    outfile.write((char *)&recovdata[0],recovdata.size());    
    datafile.close();
    parityfile.close();
    outfile.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
    return 0;

}