

   rm -f en_i 
   rm -f en_s 
   rm -f de_i 
   rm -f de_s 

# various file size
for var in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
do
   rm -f temp_en_i
   rm -f temp_en_s
   rm -f temp_de_i
   rm -f temp_de_s

   # repeat 30 times
   for idx in {1..30}
   do
      
      ./randFile $var

      #ls -l randifile
      #ls -l randsfile

      ./dbsetup > /dev/null 2>&1
      ./dedup randifile cxt_i_b cxt_i_o cxt_i_k  >> temp_en_i 2>&1
      ./dedup randsfile cxt_s_b cxt_s_o cxt_s_k  >> temp_en_s 2>&1
      diff  cxt_i_b cxt_s_b

      ./decrypt cxt_i_b cxt_i_o cxt_i_k msg_i >> temp_de_i 2>&1
      diff randifile msg_i

      ./decrypt cxt_s_b cxt_s_o cxt_s_k msg_s >> temp_de_s 2>&1
      diff randsfile msg_s

      rm -f randifile
      rm -f randsfile
      rm -f cxt_i_b
      rm -f cxt_i_o
      rm -f cxt_i_k
      rm -f cxt_s_b
      rm -f cxt_s_o
      rm -f cxt_s_k
      rm -f msg_s
      rm -f msg_i
   done
   # repetition complete
   # find the average
   awk '{ sum += $5 } END { if (NR > 0) print sum / NR }' temp_en_i >> en_i
   awk '{ sum += $5 } END { if (NR > 0) print sum / NR }' temp_en_s >> en_s
   awk '{ sum += $5 } END { if (NR > 0) print sum / NR }' temp_de_i >> de_i
   awk '{ sum += $5 } END { if (NR > 0) print sum / NR }' temp_de_s >> de_s
   #clean up records
   rm -f temp_en_i
   rm -f temp_en_s
   rm -f temp_de_i
   rm -f temp_de_s
done


