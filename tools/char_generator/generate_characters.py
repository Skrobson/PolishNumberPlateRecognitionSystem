'''
Krzysztof Skopyła script modified by Bartłomiej Skrobol
https://github.com/ksopyla/numbers_recognition
'''
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

import matplotlib.pyplot as plt

import numpy.random as rnd
import datetime as dt
import time
import os
from itertools import chain
import string


#font_names = ["OpenSans-Regular.ttf", "Mothproof_Script.ttf", "Calligraffiti.ttf"]
font_names = ["arklatrs.ttf"]
font_path = "fonts/{}"
font_size = 26 


#how many digits to generate
#random_digits=1
img_size=(32,32)#width, height (good for one digit)

#how many digits to generate
random_digits=1
#img_size=(56,32)#width, height, good for two digits

#random_digits=4
#img_size=(104,32)#width, height, good for two digits



#how many digits to generate
#random_digits=6
#img_size=(160,32)#width, height, good for four digits


folder='shared/Digits_1f1'
#how many images with one type of font, final dataset has size number_of_images*number_of_fonts
number_of_images=32
dispaly_count=1

colorBackground = "white"   
colorText = "black"

for font_name in font_names:

         
    font = ImageFont.truetype(font_path.format(font_name), font_size)
    font_folder = os.path.splitext(font_name)[0]
    
    img_save_folder = '{}/{}/'.format(folder,font_folder)
    
    if not os.path.exists(img_save_folder):
        os.makedirs(img_save_folder)

    concatenated = chain(range(ord('A'), ord('Z') + 1), range(ord('0'),ord('9')+1))
    for ch in concatenated:
        
        for i in range(0,number_of_images):
            
            #numbers_str = chr(ch)
            digit_offset=5
            dh=-3 #height offset
            angle_var=25

            img = Image.new('RGBA', img_size, colorBackground)

            char_str=chr(ch)
            fw, fh=font.getsize(char_str)
            im1 = Image.new('RGBA',(fw,fh+15),colorBackground)
       
            d1  = ImageDraw.Draw(im1)
            d1.text( (0,dh),char_str,font=font, fill=colorText)
            #d1.rectangle((0, 0, fw-1, fh-1), outline='red')
            
            im1sz = im1.size
            #d1.rectangle((0, 0, im1sz[0]-1, im1sz[1]-1), outline='green')
            
            angle = rnd.randint(-angle_var,angle_var)    
            #im1_rot=im1.rotate(angle,  expand=1)
            im1_rot=im1.rotate(angle, resample=Image.BILINEAR,  expand=1)
            #im1_rot=im1.rotate(angle, resample=Image.BICUBIC,  expand=1)
            
            pad_w = rnd.randint(-3,5)
            pad_h = rnd.randint(-1,6)
            
            pos_w = digit_offset+pad_w
            
            #img.paste(im1_rot,(pos_w,pad_h))
            img.paste(im1_rot,(pos_w,pad_h),im1_rot)
            
            
            digit_offset=pos_w+im1_rot.size[0]
            
            char_file = '{}{}_{}.png'.format(img_save_folder,char_str,int(time.time()*1000))
            
            #convert to grayscale
            img = img.convert('L')
            img.save(char_file)	
        
      
            
