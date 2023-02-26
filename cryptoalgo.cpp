#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>
using namespace std;
int main(int argc, char** argv)
{

    vector<uint8_t> message;
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
        message.push_back(c);
    }
}   