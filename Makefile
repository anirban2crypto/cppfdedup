
CXXFLAGS = -I  ezpwd-reed-solomon/c++



all:	rstest randFile rsencode rsdecode dedup dbsetup cryptoalgo

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
cryptoalgo: cryptoalgo.cpp evpaes.cpp
	g++ evpaes.cpp cryptoalgo.cpp  -o cryptoalgo -lssl -Wall -std=c++11 -lcrypto
dedup: dedup.cpp sqlsimdtct.cpp rsgenparity.cpp reconstruct.cpp
	g++  $(CXXFLAGS) evpaes.cpp sqlsimdtct.cpp rsgenparity.cpp reconstruct.cpp dedup.cpp -lsqlite3 -lssl  -std=c++11 -lcrypto -o dedup


clean:
	rm  rsencode.o rsdecode.o randFile.o  upload.o rstest.o  sqlsimdtct.o
