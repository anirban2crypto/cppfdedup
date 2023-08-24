#include <stdio.h>
#include <stdlib.h>
int main()
{
  system("ls -l similarity.db");      
  system("gzip similarity.db");  
  system("ls -l similarity.db.gz");       
  return 0;
}