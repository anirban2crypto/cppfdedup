

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

    const int rs_n=65535,rs_k=35535;
    long i=0;
    std::vector<int> erasures; // Report second symbol missing
    std::vector<int> position; // And get back corrected symbols here
    ezpwd::RS<rs_n,rs_k> rs;
    std::vector<uint8_t> file_data;
    std::vector<uint16_t> rs_input;
    rs_input.resize(rs_k);
    file_data.resize(rs_k*2);
    char c;

    int rs_msg_len_bytes=(rs_k*2);

    auto st_start = std::chrono::high_resolution_clock::now();
    ifstream infile (argv[1], ios::binary);
    if (!infile)
    {
              cout << "Could not open file for reading!\n";
              return -1;
    }   
     
    //while(!infile.eof())
    {
        //start chunk processing
        //read the chunk from file >> file_data is byte array
        for(i=0;i<rs_msg_len_bytes;i++){
            c = infile.get();
            if (!infile) 
            {
                break;
            }
            //cout<<"loop entered"<<endl;
            file_data[i]=c;
            //cout<<"block enetered"<<endl;
        }
        //convert file_data >> rs_input  
        //Girisha code goes here
        cout<<"file read"<<endl;
        for(i=0;i<rs_k;i++){

            rs_input[i]=int(65535);//(int(file_data[2*i+1]))+(256*(int(file_data[2*i])));
            //cout<<"block"<<i<<" read"<<endl;
        }    
        // encode stretch rs_input from rs_k to rs_n symbols 
        rs.encode( rs_input ); // Adds 4 Reed-Solomon parity symbols (255-251 == 4)    

        // manually induced error
        rs_input[0]=77;
        rs_input[1]=78;
        rs_input[2]=79;
        rs_input[4]=77;

        // print the erronous codeword
        cout << "Before decode"  <<endl;
        for(int k=0;k<100;k++){
            cout << int(rs_input[k]) ;
        }
        cout  <<endl;

        // run the decode algorithm
        int fixed = rs.decode(rs_input,erasures,&position );		// Correct errors, and
        cout << "Decode return: " << fixed <<  endl;    

        //initialize the rs_input- size shrink 
        //rs_input.resize(rs_k);	
       
        //print the recovered codeword           
        cout << "after decode"  <<endl;
        for(int k=0;k<100;k++){
            cout << int(rs_input[k]);
        }
        cout  <<endl;
        //end chunk processing
    }          
    infile.close();
    auto st_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> st_float_ms = st_end - st_start;
    std::cout << "Total Setup stage elapsed time is " <<  st_float_ms.count() << " milliseconds" << std::endl;
    return 0;

}