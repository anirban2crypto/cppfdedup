sudo apt-get install m4
cd gmp-6.2.1
./configure
make
make check
sudo make install
cd ..
sudo apt-get install libsqlite3-dev
sudo apt-get install libssl-dev
make -B
