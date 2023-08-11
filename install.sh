sudo apt-get install m4
cd gmp-6.2.1
./configure
make
make check
sudo make install
cd ..
sudo apt-get install libsqlite3-dev
sudo apt-get install libssl-dev
pip install pip
make -B
WORK_DIR=~/Crypto/dedup/cppfdedup
cd $WORK_DIR
cd ..
mkdir input_bitmap
mkdir blkfuzzy
cd blkfuzzy
mkdir mapping
mkdir keystore
mkdir encstore
mkdir offstore

