
CXXFLAGS = -I  ezpwd-reed-solomon/c++



all:	rstest randFile rsencode rsdecode upload

rstest: rstest.cpp
	g++ $(CXXFLAGS)  rstest.cpp -o rstest
randFile: randFile.cpp
	g++   randFile.cpp -o randFile
rsencode: rsencode.cpp rsgenparity.cpp
	g++ $(CXXFLAGS)  rsgenparity.cpp rsencode.cpp -o rsencode	
rsdecode: rsdecode.cpp
	g++ $(CXXFLAGS)  rsdecode.cpp -o rsdecode
upload: upload.cpp sqlsimdtct.cpp rsgenparity.cpp
	g++  $(CXXFLAGS) sqlsimdtct.cpp rsgenparity.cpp upload.cpp -lsqlite3 -o upload

clean:
	rm  rsencode.o rsdecode.o randFile.o  upload.o
