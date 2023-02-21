
CXXFLAGS = -I  ezpwd-reed-solomon/c++



all:	rstest randFile rsencode rsdecode

rstest: rstest.cpp
	g++ $(CXXFLAGS)  rstest.cpp -o rstest
randFile: randFile.cpp
	g++   randFile.cpp -o randFile
rsencode: rsencode.cpp
	g++ $(CXXFLAGS)  rsencode.cpp -o rsencode	
rsdecode: rsdecode.cpp
	g++ $(CXXFLAGS)  rsdecode.cpp -o rsdecode	

clean:
	rm rstest.o randFile.o
