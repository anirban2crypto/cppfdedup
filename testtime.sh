for var in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
do
   echo $var
   ./randFile $var
   #ls -l randifile
   #ls -l randsfile
   ./dbsetup
   ./dedup randifile cxt_i_b cxt_i_o cxt_i_k
   ./dedup randsfile cxt_s_b cxt_s_o cxt_s_k
   diff  cxt_i_b cxt_s_b
   ./decrypt cxt_i_b cxt_i_o cxt_i_k msg_i
   diff randifile msg_i
   ./decrypt cxt_s_b cxt_s_o cxt_s_k msg_s
   diff randsfile msg_s
done
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

