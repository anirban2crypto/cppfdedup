import os
import sys
from scipy.spatial.distance import hamming



data1=bytes()
data2=bytes()
symbol1=list()
symbol2=list()

if len(sys.argv) < 3:
     print("Empty parameters")
     print("Usage: Symbol size in bytes, tolerate no of error symbol")
     exit(1)

SYM_SIZE=int(sys.argv[1])    
RS_BLK_SYM=(2**(8*SYM_SIZE))-1
RS_LMT_SYM=int(sys.argv[2])    

BLK_BYT_SIZE = (RS_BLK_SYM-(2*RS_LMT_SYM))*SYM_SIZE
PARITY_SIZE = (2*RS_LMT_SYM)*SYM_SIZE

print("Symbol size",SYM_SIZE)
print("RS BLK SYMBOL",RS_BLK_SYM)
print("RS ERROR LIMIT SYMBOL",RS_LMT_SYM)
print("RS BLOCK IN BYT",BLK_BYT_SIZE)
print("PARITY IN BYT",PARITY_SIZE)


f_begin = "/home/anirban/crypto/img_dedup/frames_bmp/img"
f_end = ".bmp"
file_list=list()
for fnum in range(1,1000):
    f_name=f_begin+str(fnum)+f_end
    file_list.append(f_name)

max_cl_cnt=0
save_snum=0
for snum in range(919,920):
    cl_cnt=0    
    for vnum in range(1,7):
        print("File number",snum,(vnum+snum))
        if ((vnum+snum) <= 999):    
            t1=0
            t2=0
            t3=0
            f1_name=file_list[snum]
            f2_name=file_list[snum+vnum]
    
            f1=open(f1_name, mode="rb")
            f2=open(f2_name, mode="rb")
            
            data_remain=os.path.getsize(f1_name)
            block_cnt=0
            
            #block wise loop
            while(data_remain > 0):
                block_cnt+=1
                #print("block count: ",block_cnt)
            
                data1 = list(f1.read(BLK_BYT_SIZE))
                data2 = list(f2.read(BLK_BYT_SIZE))
            
                symbol1=list()
                symbol2=list()
            
                if SYM_SIZE ==2:
                    for i in range(int(len(data1)/2)):
                        symbol1.append(int(data1[2*i+1])+256*int(data1[2*i]))
                        symbol2.append(int(data2[2*i+1])+256*int(data2[2*i]))
                if SYM_SIZE ==1:
                    for i in range(int(len(data1))):
                        symbol1.append(data1[i])
                        symbol2.append(data2[i])
            
                hamming_distance = hamming(symbol1,symbol2) * len(symbol1)        
                #print("hamming distance",hamming_distance)
                if (hamming_distance == 0):
                    t1+=1
                elif (hamming_distance <= RS_LMT_SYM):
                    t2+=1            
                else:            
                    t3+=1                                
                data_remain-=BLK_BYT_SIZE        
            print("Total block: ",block_cnt)
            print("Total exact block: ",t1)
            print("Total similar block: ",t2)
            print("Total disimilar block: ",t3)
            f2.close()
            f1.close()                
            if(t2>t3):
                cl_cnt+=1
                #print("More similar block, similar, disimilar ",t2,t3)
            else:
                #print("File number",snum,(vnum+snum))
                #print("Count ",cl_cnt)
                if (cl_cnt > max_cl_cnt ):
                    max_cl_cnt=cl_cnt
                    save_snum=snum    
                break      
    
print(max_cl_cnt,save_snum)