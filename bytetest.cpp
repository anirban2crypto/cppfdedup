#include <iostream>
#include <stdio.h>
using namespace std;


void intToCharArray(unsigned char* buffer, int in,int BYTES_PER_INT)
{
	int x=in;
    std::copy(static_cast<const char*>(static_cast<const void*>(&x)),
          static_cast<const char*>(static_cast<const void*>(&x)) + BYTES_PER_INT,
          buffer);	
}

int main()
{
	int loc=2*65535;
	unsigned char cbyte[3];
    intToCharArray(cbyte, loc,3); 
    cout<<int(cbyte[2])<<endl;
    cout<<int(cbyte[1])<<endl;
    cout<<int(cbyte[0])<<endl;
}

