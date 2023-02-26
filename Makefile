
CXXFLAGS = -I  ezpwd-reed-solomon/c++



all:	rstest randFile rsencode rsdecode upload dbsetup

rstest: rstest.cpp
	g++ $(CXXFLAGS)  rstest.cpp -o rstest
randFile: randFile.cpp
	g++   randFile.cpp -o randFile
rsencode: rsencode.cpp rsgenparity.cpp
	g++ $(CXXFLAGS)  rsgenparity.cpp rsencode.cpp -o rsencode	
rsdecode: rsdecode.cpp reconstruct.cpp
	g++ $(CXXFLAGS)  reconstruct.cpp rsdecode.cpp -o rsdecode
dbsetup: dbsetup.cpp sqlsimdtct.cpp 
	g++  $(CXXFLAGS) dbsetup.cpp sqlsimdtct.cpp  -lsqlite3 -o dbsetup	
upload: upload.cpp sqlsimdtct.cpp rsgenparity.cpp reconstruct.cpp
	g++  $(CXXFLAGS) sqlsimdtct.cpp rsgenparity.cpp reconstruct.cpp upload.cpp -lsqlite3 -o upload


clean:
	rm  rsencode.o rsdecode.o randFile.o  upload.o
