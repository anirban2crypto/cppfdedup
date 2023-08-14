

#include <string>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <chrono>

using namespace std;
int main(int argc, char** argv)
{

    const int rs_n=511,rs_k=503;
    long i=0;
    std::vector<int> erasures; // Report second symbol missing
    std::vector<int> position; // And get back corrected symbols here
    ezpwd::RS<rs_n,rs_k> rs;
    std::vector<uint8_t> file_data;
    std::vector<uint16_t> rs_input;
    rs_input.resize(rs_k);
    int rs_msg_len_bytes=floor((rs_k*9)/8);

    auto st_start = std::chrono::high_resolution_clock::now();
    ifstream infile (argv[1], ios::binary);
    if (!infile)
    {
              cerr << "Could not open file for reading!\n";
              return -1;
    }   
     
    while(!infile.eof())
    {
        //start chunk processing
        //read the chunk from file >> file_data is byte array
        for(i=0;i<rs_msg_len_bytes;i++){
            if(infile.eof())
            {
                break;
            }
            file_data[i]=infile.get();
        }
        //convert file_data >> rs_input  
        //Girisha code goes here

        // encode stretch rs_input from rs_k to rs_n symbols 
        rs.encode( rs_input ); // Adds 4 Reed-Solomon parity symbols (255-251 == 4)    

        // manually induced error
        rs_input[0]=77;
        rs_input[1]=78;
        rs_input[2]=79;
        rs_input[4]=77;

        // print the erronous codeword
        cerr << "Before decode"  <<endl;
        for(int k=0;k<rs_k;k++){
            cerr << int(rs_input[k]) ;
        }
        cerr  <<endl;

        // run the decode algorithm
        int fixed = rs.decode(rs_input,erasures,&position );		// Correct errors, and
        cerr << "Decode return: " << fixed <<  endl;    

        //initialize the rs_input- size shrink 
        rs_input.resize(rs_k);	
       
        //print the recovered codeword           
        cerr << "after decode"  <<endl;
        for(int k=0;k<rs_k;k++){
            cerr << int(rs_input[k]);
        }
        cerr  <<endl;
        //end chunk processing
    }          
    infile.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total Setup stage elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
    return 0;

}