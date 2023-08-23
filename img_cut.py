from PIL import Image
img = Image.open("img1.bmp")
area = (500, 500, 508, 504)
cropped_img = img.crop(area)
cropped_img.save("img1_crop.bmp")
