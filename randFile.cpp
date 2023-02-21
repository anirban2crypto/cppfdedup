
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream> 
#include<cmath> 
using namespace std;
int main(int argc, char** argv)
{

    int i,j,count,asciiVal,size;
    string filename;
    char cht;
    char *a = argv[1];
    int num = atoi(a);
    if (num > 0)
    {        
        filename= "randfile_" + std::to_string(num)+".txt";
        ofstream outfile(filename, ios::out | ios::trunc);
        if (!outfile)
        {
              cerr << "Could not open file for writing!\n";
              return -1;
        }
        size=pow(2,num);    
        count=size*1024;
        for (i = 0; i < count; i++)
        {
            asciiVal=33+(rand()%92);
            cht = char(asciiVal);		
		    outfile <<cht; 
        }
        outfile.close();
    }
    return 0;

}