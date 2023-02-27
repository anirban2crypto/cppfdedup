randFile:
Generates random file and error file . Takes input integer number XXXX and generates (XXX)KB random file randfile_XXXX and 
randsim_XXXX, same as  randfile_XXXX with four error four error. 
Usage :
./randFile number

rsencode: Generates sketch from input file and output . Reed solomon code parameter hardcoded in the program (rs_n=255,rs_k=243,dis=(rs_n-rs_k)).
Usage :
./rsencode messagefile parityfile

rsdecode: Reed solomon decoder. Takes close message file, parity file and reconstruct message file.
Usage :
./rsdecode closeMessagefile parityfile reconstuctMessagefile

dbsetup: Initialize database and create table
Usage :
./dbsetup 

cryptoalgo: Message lock encryption algorithm. Take input file gen key SHA256. Encrypt it with EVP_aes_128_cbc . Decrypt ciphertext. 
Usage :
./cryptoalgo test.txt

dedup: take input file and output three ciphertext file (base - mle cihertext file, offset - cpa ciphertext file, key - cpa ciphertext file )
Usage :
/dedup randsim_2 
