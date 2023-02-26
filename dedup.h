#ifndef __DEDUP_H
#define __DEDUP_H
using namespace std; 
void  setup();
void  insertParity(unsigned char* fprint,int fsizes, unsigned char *paritydata,int psizes);
void checkSimilarity(unsigned char* ofprint, int ofsizes,bool* ofound,vector<uint8_t> &paritydata);
#endif
