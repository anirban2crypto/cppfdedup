randFile:
Generates random file. Takes input integer number and generates (2^number)KB random file.
Usage :
./randFile number

rsdecode:
Generates sketch from input file and output . Reed solomon code parameter hardcoded in the program (rs_n=255,rs_k=243,dis=(rs_n-rs_k)).
Usage :
./rsencode messagefile parityfile

rsdecode:
Takes close message file, parity file and reconstruct message file. 

./rsdecode closeMessagefile parityfile reconstuctMessagefile