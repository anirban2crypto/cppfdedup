from PIL import Image
import os
import sys

f1_name = sys.argv[1]  

input_image = Image.open(f1_name)
pixel_map = input_image.load()
  
# Extracting the width and height 
width, height = input_image.size
print("Mode",input_image.mode)
print("Widhth ",width,"height ",height)
# getting the RGB pixel value.
for i in range(height):
        print(i)    
        row=[ input_image.getpixel((i, j)) for j in range(width)]
        if(sum(row) !=0 ):
            a=5
# Saving the final output
input_image.putpixel((38, 100), 250)
file_out = f1_name[:-4]+"_modify.bmp"
print(file_out)
input_image.save(file_out)

