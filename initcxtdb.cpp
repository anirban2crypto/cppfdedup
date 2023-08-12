#include "cxttab.h"
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    int flag = atoi(argv[1]);
    if (flag==1){
        cerr << "Fuzzy ciphertext table initialized" << endl;
        initdb();
    }
    else if (flag==2){
        cerr << "Exact ciphertext table initialized" << endl;
        eInitdb();
    }
    else{
        cerr << "accept only - flag value 1 or 2 " << endl;
    }
    /*unsigned char *tag = (unsigned char *)"DSmlThngaInGrtWy"; 
    int tsizes=16;
    int cxtsize=255;
    insertCxt((char *)tag,tsizes,cxtsize);*/

}