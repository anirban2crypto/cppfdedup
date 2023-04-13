randFile:
Generates random file and error file . Takes input integer number XXXX and generates (XXX)KB random file randfile_XXXX and 
randsim_XXXX, same as  randfile_XXXX with four error four error. 
Usage :
./randFile number
Command:
./randFile 1

rsencode: Generates sketch from input file. Reed solomon code parameter hardcoded in the program (rs_n=255,rs_k=243,dis=(rs_n-rs_k)).
Usage :
./rsencode messagefile parityfile
Command:
./rsencode randfile_1 parityfile_1

rsdecode: Reed solomon decoder. Takes close message file, parity file and reconstruct message file.
Usage :
./rsdecode closeMessagefile parityfile reconstuctMessagefile
Command:
./rsdecode randsim_1 parityfile_1 reconfile_1

dbsetup: Initialize database and create table
Usage :
./dbsetup
Command:
./dbsetup  

cryptoalgo: Message lock encryption algorithm. Take input file gen key SHA256. Encrypt it with EVP_aes_128_cbc . Decrypt ciphertext. 
Usage :
./cryptoalgo test.txt

dedup: take input file and output three ciphertext file (base - mle cihertext file, offset - cpa ciphertext file, key - cpa ciphertext file )
Usage :
./dedup inputfile out_code_file out_offset_file
command:
./dedup randfile_1 code_1 off_1
./dedup randsim_1 code_s_1 off_s_1 key_s_1

