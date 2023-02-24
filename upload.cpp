
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream> 
#include<cmath>
#include <vector> 
#include <ezpwd/rs>
#include "dedup.h"
#include "rscode.h"

using namespace std;
int main(int argc, char** argv)
{

    vector<long> locatn;
    vector<uint8_t> parity; 
    unsigned char *oparity;
    vector<uint8_t> subsample;
    bool rowfound=false;

    long loc;


    setup();
    ifstream parfile("param.txt");
    if (!parfile){
        cerr << "Could not open file for reading!\n";
        return -1;
    }	
    while(!parfile.eof()){    
         parfile >> loc;
         locatn.push_back(loc);
    }
    ifstream infile (argv[1], ios::binary);
    if (!infile){
        cerr << "Could not open input file for reading!\n";
        return -1;
    }   
     
    int index = 0;
    while ( index < locatn.size() && (!infile.eof())) {
        loc = locatn.at(index);
        infile.seekg(loc, ios::beg);
        subsample.push_back(infile.get());
        index++;
    }  
    infile.clear();   //  Since ignore will have set eof.
	infile.seekg( 0, std::ios_base::beg );
    genparity(infile,parity);    
    insertParity((unsigned char *)&subsample[0],(int)subsample.size(), (unsigned char *)&parity[0],(int)parity.size());
    checkSimilarity((unsigned char *)&subsample[0],(int)subsample.size(),&rowfound,oparity);    
    cerr << "size in main"<<strlen((char *)oparity)<<endl;
    infile.close();

}