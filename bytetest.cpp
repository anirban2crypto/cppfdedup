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
	for (int i=0;i<54;i++)
    {
	    unsigned char cbyte[3];
        intToCharArray(cbyte,i,3);     
        cout<<int(cbyte[0])<<endl;
        cout<<int(cbyte[1])<<endl;
        cout<<int(cbyte[2])<<endl;
    }    

}

