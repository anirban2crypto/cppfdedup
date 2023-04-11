#include <string>
#include <cstdlib>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>
#include "rscode.h"
int main(int argc, char** argv)
{
    vector<uint8_t> parity;    
    vector<uint8_t> msgdata;    
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
    if (!infile){
        cerr << "Could not open file for reading!\n";
        return -1;
    } 
    char c;
    while (true) {
        c = infile.get();
        if (!infile) 
            break;
        msgdata.push_back(c);
    }
    cerr << "Input Data Size: " <<msgdata.size()<<endl;
    genparity(msgdata,parity);
    outfile.write((char *)&parity[0],parity.size());               
    infile.close();
    outfile.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total  elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
    return 0;

}