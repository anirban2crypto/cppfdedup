#include <iostream>
#include <sqlite3.h>
#include <cstring>
#include <vector> 
#include "cxttab.h"


using namespace std; 
 
void  initdb()
{
    sqlite3* DB;
    std::string sql1 ="DROP TABLE IF EXISTS CXTTABLE;";
    std::string sql2 ="CREATE TABLE CXTTABLE(TAG TEXT PRIMARY KEY,SIZE INTEGER);";    
    int exit = 0;
    char* messaggeError;

    exit = sqlite3_open("DBCXT.db", &DB);
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
void  insertCxt(char* tag,int tsizes, int cxtsize)
{
    char* zErrMsg = 0; // Error message var
    const char* sql = "INSERT INTO CXTTABLE(TAG ,SIZE) VALUES (?,?)";
    sqlite3_stmt* stmt=NULL;
    int exit = 0;
    sqlite3* DB;
    exit = sqlite3_open("DBCXT.db", &DB);
    int rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK)
    {        
        rc = sqlite3_bind_text(stmt, 1, tag, tsizes, SQLITE_STATIC);
        rc = sqlite3_bind_int(stmt, 2, cxtsize);
        
        // Commit the binds

        rc=sqlite3_step(stmt);
        sqlite3_finalize(stmt);

    }    
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_CONSTRAINT)
    {
        std::cerr << "Error in inserting Table:" << rc << std::endl;
        sqlite3_free(zErrMsg);
    }   
    sqlite3_close(DB);    
}
