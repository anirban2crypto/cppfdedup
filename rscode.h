#ifndef __RSCODE_H
#define __RSCODE_H
#include <string>
#include <cstdlib>
#include <iostream>
#include<cmath> 
#include <fstream>
#include <ezpwd/rs>
#include <vector> 
#include <chrono>
#define ECC_ERR_LMT 14000
#define ECC_BLK_LEN 65535
using namespace std; 
void genparity(vector<uint8_t> &msgdata,vector<uint8_t> &paritydata);
int reconst(vector<uint8_t> &erronsdata,vector<uint8_t> &paritydata,vector<uint8_t> &recovdata,vector<int> &intoffset);
#endif
