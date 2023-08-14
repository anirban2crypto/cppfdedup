#include <iostream>
#include <bitset>
#include <climits>
#include <array>

using namespace std;

template<size_t numBytes>
std::bitset<numBytes * CHAR_BIT> bytesToBitset(uint8_t *data)
{
    std::bitset<numBytes * CHAR_BIT> b;

    for(int i = 0; i < numBytes; ++i)
    {
        uint8_t cur = data[i];
        int offset = i * CHAR_BIT;

        for(int bit = 0; bit < CHAR_BIT; ++bit)
        {
            b[offset] = cur & 1;
            ++offset;   // Move to next bit in b
            cur >>= 1;  // Move to next bit in array
        }
    }

    return b;
}


int main()
{
    std::array<uint8_t, 9> bytes = { 0x2E, 0xAD, 0xBE, 0x5F, 0x75,0x89,0x23,0x92,0x1B };
    auto bits = bytesToBitset<bytes.size()>(bytes.data());
    std::cout << "bits:" << bits << std::endl;

    const uint slen = bits.size()/9;
    cout << "Size of bitset = " << slen << endl;
    // Need to convert bit.size() many bits into slen short integers

    bitset<slen*16> sbits;
    std::array<uint16_t, slen> shorts;
    uint newlen = slen*2/16;

    uint i=0,j, k=0;
    while(i < slen*16)
    {
    	for(j = 0; j < 7; j++) 
    	{
    		sbits.reset(i+j);
    	}
    	for(j = 7; j < 16; j++)
    	{
    		//printf("i= %d, j=%d, bits[%d] = %d\t", i,j,k,(int)bits[k]);
    		sbits[i+j] = bits[k++];
    	}
    	i = i+j;
    }
    std::cout << endl << "sbits:" << sbits << std::endl;

    string newstr = sbits.to_string();

    cout << "newstr = " << newstr << endl;
	
}

/*

#include <iostream>
#define MAX_BIT_LENGTH 9

int main()
{
	u_char str[] = {0x23, 0x45, 0x56, 0x78, 0xAB, 0x59, 0x94, 0x72, 0x40};
}

ushort *convert(char *str, int len)
{
	ushort *buf = new ushort(len/9);
	uint n = buf[0];
	for(uint j = 0; j < MAX_BIT_LENGTH; j++)
	{
		buf[MAX_BIT_LENGTH-1-j] = static_cast<bool>(n % 2);
		n = n / 2;      
	}
	return buf;

}	
*/