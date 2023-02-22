#include <iostream>
#include <sqlite3.h>
  
static 
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
        std::cerr << "Error Create Table" << std::endl;
        sqlite3_free(messaggeError);
    }    
    sqlite3_close(DB);
    return (0);
}
static int callback(void* data, int argc, char** argv, char** azColName)
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
}
