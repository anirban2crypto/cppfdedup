import os
import sys
from scipy.spatial.distance import hamming

 

f1_name="log11.txt"
f2_name="log12.txt"

 

data1=list()
data1=list()

 

# read log1 data
with open(f1_name) as f1:
    data1 = [int(line.rstrip()) for line in f1]

 

# read log1 data    
with open(f2_name) as f2:
    data2 = [int(line.rstrip()) for line in f2]

 

#calculate hamming distance
hamming_distance = hamming(data1,data2) * len(data1)
print(hamming_distance)    