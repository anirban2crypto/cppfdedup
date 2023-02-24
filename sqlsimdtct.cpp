#include <iostream>
#include <sqlite3.h>
#include "dedup.h"

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
/*static int callback(void* data, int argc, char** argv, char** azColName)
{
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
  
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
  
    printf("\n");
    return 0;
}
  
static 
void  checkSimilarity()
{
    sqlite3* DB;
    int exit = 0;
    string data("CALLBACK FUNCTION");  
    string sql("SELECT FPRINT,PARITY FROM HSIMILARITY WHERE FPRINT=?");

    // Open database
    exit = sqlite3_open("similarity.db", &DB);
    if (exit)
    {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
        return (-1);
    }
    else
    {
        std::cout << "Opened Database Successfully!" << std::endl;
    }

    //execute select query
    int rc = sqlite3_exec(DB, sql.c_str(), callback, (void*)data.c_str(), NULL);  
    if (rc != SQLITE_OK)
    {    
        cerr << "Error SELECT" << endl;
    }    
    else
    {
        cout << "Operation OK!" << endl;
    }
    
    sqlite3_close(DB);
    return (0);
}
static 
void  insertParity(unsigned char* fprint,int fsizes, unsigned char *parity,int psizes)
{           
    int ret = sqlite_bind_array
    (
        db,                                                            // sqlite3* 
        "INSERT INTO HSIMILARITY(FPRINT ,PARITY) VALUES (?,?)"  
        1,                                                             // rows to insert
        SQLITE_BIND_ARRAY_BLOB(fprint, fsizes),                        // array of fprint and sizes for each
        SQLITE_BIND_ARRAY_BLOB(parity, psizes),                        // array of parity and sizes for each
        SQLITE_BIND_ARRAY_END                                          // end of variable args marker 
    );                                                                
}*/
void Access_Database::insertAirportJobs(float pay, int expire, float weight, Airport ap, Airport source)
{
    char* zErrMsg = 0; // Error message var
    const char* sql = "INSERT INTO JOBS (airport_ID,dest,pay,expire,weight) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    const char* pszTest;
    int rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, &pszTest);
    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, source.airport_ID);
        sqlite3_bind_int(stmt, 2, ap.airport_ID);
        sqlite3_bind_double(stmt, 3, pay);
        sqlite3_bind_int(stmt, 4, expire);
        sqlite3_bind_double(stmt, 5, weight);
        
        // Commit the binds

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

    }
    
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "\nSQL error: %s\n\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Operation done successfully\n");
    }
}
