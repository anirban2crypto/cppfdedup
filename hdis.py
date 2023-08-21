import os
import sys
from scipy.spatial.distance import hamming


data1=bytes()
data2=bytes()

if len(sys.argv) < 5:
    print("Enter two file name")
    print("Usage: file_name_1, file_name_2, blocksize, tollerance ")
    exit(1)
f1_name = sys.argv[1]
f2_name = sys.argv[2]
limit = int(sys.argv[4])
BLOCK_SIZE=int(sys.argv[3])

f1_size = os.stat(f1_name).st_size
f2_size = os.stat(f2_name).st_size
t1=0
t2=0
t3=0

if f1_size == f2_size :
    f1=open(f1_name, mode="rb")
    f2=open(f2_name, mode="rb")

    data_remain=f1_size
    block_cnt=0
    while(data_remain > 0):
        block_cnt+=1
        #print("block count: ",block_cnt)
        data1 = f1.read(BLOCK_SIZE)
        data2 = f2.read(BLOCK_SIZE)
        hamming_distance = hamming(list(data1),list(data2)) * len(list(data1))
        print(hamming_distance)
        if (hamming_distance == 0):
            t1+=1
        elif (hamming_distance <= limit):
            t2+=1            
        else:            
            t3+=1                                
        data_remain-=BLOCK_SIZE        
    print("Total block: ",block_cnt)
    print("Total exact block: ",t1)
    print("Total similar block: ",t2)
    print("Total disimilar block: ",t3)    