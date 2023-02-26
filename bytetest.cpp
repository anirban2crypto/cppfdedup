#include <iostream>
#include <stdio.h>
using namespace std;

const size_t BYTES_PER_INT = sizeof(int); 
void intToCharArray(char* buffer, int in)
{
	for (size_t i = 0; i < BYTES_PER_INT; i++) {
		size_t shift = 8 * (BYTES_PER_INT - 1 - i);
		buffer[i] = (in >> shift) & 0xff;
	}	
}

int main()
{
	int in=255;
	char buffer[BYTES_PER_INT];
	intToCharArray(buffer, in);
    for(int i=0;i<BYTES_PER_INT;i++){
        cerr << int(buffer[i]) << endl;
    }

}

