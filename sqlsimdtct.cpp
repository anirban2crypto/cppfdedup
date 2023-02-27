#include <iostream>
#include <sqlite3.h>
#include <cstring>
#include <vector> 
#include "dedup.h"
#include "rscode.h"

using namespace std; 
 
void  setup()
{
    sqlite3* DB;
    std::string sql1 ="DROP TABLE IF EXISTS HSIMILARITY;";
    std::string sql2 ="CREATE TABLE HSIMILARITY(FPRINT BLOB,PARITY BLOB);";    
    int exit = 0;
    char* messaggeError;

    exit = sqlite3_open("similarity.db", &DB);
    if (exit != SQLITE_OK)
    {
        std::cerr << "Open database error" << std::endl;
        sqlite3_free(messaggeError);
    }

    exit = sqlite3_exec(DB, sql1.c_str(), NULL, 0, &messaggeError);
  
    if (exit != SQLITE_OK)
    {
        std::cerr << "Error in drop table" << std::endl;
        sqlite3_free(messaggeError);
    }
    exit = sqlite3_exec(DB, sql2.c_str(), NULL, 0, &messaggeError);
  
    if (exit != SQLITE_OK)
    {
        std::cerr << "Error in Create Table" << std::endl;
        sqlite3_free(messaggeError);
    }
    else{
        std::cout << "Table created Successfully!" << std::endl;
    }    
    sqlite3_close(DB);    
}
void checkSimilarity(unsigned char* ofprint, int ofsizes,bool* ofound,vector<uint8_t> &paritydata)
{
    char* zErrMsg = 0; // Error message var
    const char* sql = "SELECT FPRINT,PARITY FROM HSIMILARITY WHERE FPRINT=?";
    sqlite3_stmt* stmt=NULL;
    int exit = 0;
    sqlite3* DB;
    char * dbvalue;


    exit = sqlite3_open("similarity.db", &DB);
    if (exit != SQLITE_OK)
    {
        std::cerr << "Open database error" << std::endl;
        sqlite3_free(zErrMsg);
    }
    int rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK)
    {        
        rc = sqlite3_bind_blob(stmt, 1, ofprint, ofsizes, SQLITE_STATIC);        
    }    

    // execute sql statement, and while there are rows returned, print ID
    int ret_code = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {  
        *ofound=true;  
        long size=sqlite3_column_bytes(stmt, 1);
        dbvalue=(char *)sqlite3_column_blob(stmt, 1);
        for(long i=0;i<size;i++)
            paritydata.push_back(dbvalue[i]);        
    }
    if(ret_code != SQLITE_DONE) {
        std::cerr << "Error in inserting Table" << std::endl;
        sqlite3_free(zErrMsg);
    }

    //release resources
    sqlite3_finalize(stmt);
    sqlite3_close(DB);    
}
void  insertParity(unsigned char* fprint,int fsizes, unsigned char *parity,int psizes)
{
    char* zErrMsg = 0; // Error message var
    const char* sql = "INSERT INTO HSIMILARITY(FPRINT ,PARITY) VALUES (?,?)";
    sqlite3_stmt* stmt=NULL;
    int exit = 0;
    sqlite3* DB;
    exit = sqlite3_open("similarity.db", &DB);
    int rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK)
    {        
        rc = sqlite3_bind_blob(stmt, 1, fprint, fsizes, SQLITE_STATIC);
        rc = sqlite3_bind_blob(stmt, 2, parity, psizes, SQLITE_STATIC);
        
        // Commit the binds

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

    }    
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error in inserting Table" << std::endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        std::cerr << "Insertion OK!" << endl;
    }
    sqlite3_close(DB);    
}
