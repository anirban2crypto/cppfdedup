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
using namespace std; 
void genparity(istream &infile,vector<uint8_t> &paritydata);
void reconst(vector<uint8_t> &erronsdata,vector<uint8_t> &paritydata,vector<uint8_t> &recovdata,vector<int> &intoffset);
#endif
