import os
import sys
from scipy.spatial.distance import hamming


data1=bytes()
data2=bytes()
symbol1=list()
symbol2=list()

# if len(sys.argv) < 5:
#     print("Enter two file name")
#     print("Usage: file_name_1, file_name_2, blocksize, tollerance ")
#     exit(1)
f1_name = "img2.bmp"
f2_name = "img3.bmp"
#f1_name = "alpha1.txt"
#f2_name = "alpha2.txt"
limit = 18000
BLOCK_SIZE=59070

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

    #block wise loop
    while(data_remain > 0):
        block_cnt+=1
        print("block count: ",block_cnt)

        data1 = list(f1.read(BLOCK_SIZE))
        data2 = list(f2.read(BLOCK_SIZE))
        #print(data1)
        #print(data2)
        #print("length ",len(data1))
        symbol1=list()
        symbol2=list()
        for i in range(int(len(data1)/2)):
            #print("Index",(2*i+1),(2*i))
            symbol1.append(int(data1[2*i+1])+256*int(data1[2*i]))
            symbol2.append(int(data2[2*i+1])+256*int(data2[2*i]))
        #print(symbol1)
        #print(symbol2)            
        hamming_distance = hamming(symbol1,symbol2) * len(symbol1)        
        print("hamming distance",hamming_distance)
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