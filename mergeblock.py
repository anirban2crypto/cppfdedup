import os
import sys

f1_name="img1.bmp7"
f1=open(f1_name, mode="rb")
data1=f1.read()


f2_name="img1.bmp15"
f2=open(f2_name, mode="rb")
data2=f2.read()
 

#f3_name="img1.bmp6"
#f3=open(f3_name, mode="rb")
#data3=f3.read()




f4_name="img1.bmp715"
f4=open(f4_name, mode="wb")
f4.write(data1)
f4.write(data2)
#f4.write(data3)

f4.close()
f1.close()        
f2.close()       
#f3.close()