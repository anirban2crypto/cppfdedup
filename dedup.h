#ifndef __DEDUP_H
#define __DEDUP_H
using namespace std; 
void  setup();
void  insertParity(unsigned char* fprint,int fsizes, unsigned char *parity,int psizes);
void checkSimilarity(unsigned char* ofprint, int ofsizes,bool* ofound,unsigned char * oparity);
#endif
