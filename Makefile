
CXXFLAGS = -I  ezpwd-reed-solomon/c++



all:	rstest randFile rsencode rsdecode dedup dbsetup cryptoalgo decrypt fblkdedup blkdedup

rstest: rstest.cpp
	g++ $(CXXFLAGS)  rstest.cpp -o rstest
randFile: randFile.cpp
	g++ randFile.cpp -o randFile
rsencode: rsencode.cpp rsgenparity.cpp
	g++ $(CXXFLAGS)   rsgenparity.cpp rsencode.cpp -o rsencode	
rsdecode: rsdecode.cpp reconstruct.cpp
	g++ $(CXXFLAGS)   reconstruct.cpp rsdecode.cpp -o rsdecode
dbsetup: dbsetup.cpp sqlsimdtct.cpp 
	g++ $(CXXFLAGS)  dbsetup.cpp sqlsimdtct.cpp  -lsqlite3 -o dbsetup	
cryptoalgo: cryptoalgo.cpp encsub.cpp
	g++ encsub.cpp cryptoalgo.cpp  -o cryptoalgo -lssl  -lcrypto
dedup: dedup.cpp sqlsimdtct.cpp rsgenparity.cpp reconstruct.cpp
	g++ $(CXXFLAGS) encsub.cpp sqlsimdtct.cpp rsgenparity.cpp reconstruct.cpp dedup.cpp -lsqlite3 -lssl -lcrypto -o dedup
decrypt: encsub.cpp  decrypt.cpp
	g++ $(CXXFLAGS) encsub.cpp  decrypt.cpp  -lssl -lcrypto -o decrypt	
fblkdedup: sqlcxttable.cpp encsub.cpp sqlsimdtct.cpp rsgenparity.cpp reconstruct.cpp fblkdedup.cpp 	
	g++ $(CXXFLAGS) sqlcxttable.cpp encsub.cpp sqlsimdtct.cpp rsgenparity.cpp reconstruct.cpp fblkdedup.cpp -lsqlite3 -lssl -lcrypto -o fblkdedup	
blkdedup: blkdedup.cpp encsub.cpp
	g++  blkdedup.cpp encsub.cpp -o blkdedup -lssl  -lcrypto 
clean:
	rm  rsencode.o rsdecode.o randFile.o  upload.o rstest.o  sqlsimdtct.o cryptoalgo.o dedup.o decrypt.o fblkdedup.o blkdedup.o
	
