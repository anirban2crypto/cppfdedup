
Install prerequisite software for this package
Command:
. install.sh

Run fuzzy deduplication on various file size and record the time
Command:
. testtime.sh

randFile:
Generates random file and similar file . Takes input integer number XXXX and generates 2^(XXX)KB random file randifile and 
randsfile, same as  randifile with four error in every block. 
Usage :
./randFile number
Command:
./randFile 1

rsencode:
Generates sketch from input file. Reed solomon code parameter hardcoded in the program (rs_n=255,rs_k=243,dis=(rs_n-rs_k)).
Usage :
./rsencode messagefile parityfile
Command:
./rsencode randifile parityfile

rsdecode:
Reed solomon decoder. Takes close message file, parity file and output reconstruct message file.
Usage :
./rsdecode closeMessagefile parityfile reconstuctedMessagefile
Command:
./rsdecode randsfile parityfile reconfile

dbsetup: Initialize database and create table
Usage :
./dbsetup
Command:
./dbsetup  

cryptoalgo:
CPA encryption and message lock encryption algorithm. Take input file gen key SHA256. Encrypt it with EVP_aes_128_cbc . Decrypt ciphertext. 
Usage :
./cryptoalgo test.txt

dedup:
Fuzzy deduplication encryption/upload algorithm. Takes input file and output three ciphertext file (base - mle cihertext file, offset - cpa ciphertext file, key - cpa ciphertext file )
Usage :
./dedup inputfile out_base_ciph_file out_offset_ciph_file out_key_ciph_file
command:
./dedup randifile cxt_i_b cxt_i_o cxt_i_k
./dedup randsfile cxt_s_b cxt_s_o cxt_s_k


decrypt:
Fuzzy deduplication decryption/download algorithm. Decrypt and reconstruct file from base ciphertext , offset ciphertext , key ciphertect
Usage :
./decrypt in_base_ciph_file in_offset_ciph_file in_key_ciph_file out_reconstructed_file
command:
./decrypt cxt_i_b cxt_i_o cxt_i_k msg_i
./decrypt cxt_s_b cxt_s_o cxt_s_k msg_s


./dbsetup
./initcxtdb
./fblkdedup randifile 155
./fblkdedup randsfile 155

sqlite3 similarity.db
select count(*) from HSIMILARITY;

sqlite3 DBCXT.db
select count(*) from CXTTABLE;
select sum(size) from CXTTABLE;


#rsgenparity.cpp
#reconstruct.cpp
#rscode.h

#20 4129392
#25 4005072
#30 4181792
#35 4050656
#40 3906880
#total 5529708
#key    463294
#key     94880