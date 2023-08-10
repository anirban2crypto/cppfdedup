from PIL import Image
import os
import sys

f1_name = sys.argv[1]  

input_image = Image.open(f1_name)
pixel_map = input_image.load()
  
# Extracting the width and height 
width, height = input_image.size

# getting the RGB pixel value.
for i in range(int(width)):
    r, g, b, p = input_image.getpixel((i, int(height-1)))
       
    # Apply formula of grayscale:
    #grayscale = (0.299*r + 0.587*g + 0.114*b)
    grayscale = (1*r + 0*g + 0*b)

    # setting the pixel value.
    pixel_map[i, int(height-1)] = (int(grayscale), int(grayscale), int(grayscale))
  
# Saving the final output
input_image.save("grayscale", format="bmp")
