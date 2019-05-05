# -*- coding: utf-8 -*-

# конвертация растровых шрифтов из горизонтального формата в вертикальный
# шрифты взяты с сайта http://electronics-and-mechanics.azm.su/page59.html
# и представляют собой массив данных без какой либо разметки

import sys
import os
import math

file_input = '8X16WIN1251.FNT'
file_output = 'fnt8x16.c'
glyph_width = 8
glyph_height = 16

glyphs_src = []
glyphs_dest = []

with open(file_input,'rb') as f:
    arr = [i for i in f.read()]
    f.close

for i in range(0, len(arr), glyph_height * math.ceil(glyph_width/8)):
    glyphs_src.append(arr[i:i+glyph_height])

for glyph in glyphs_src:
    arr = [0] * (glyph_width * math.ceil(glyph_height/8))
    for w in range(0, glyph_width):
        for h in range(0, glyph_height):
            index_dest = w * math.ceil(glyph_height/8) + math.floor(h/8)
            index_src = h * math.ceil(glyph_width/8) + math.floor(w/8)
            arr[index_dest] = (arr[index_dest] >> 1) | (glyph[index_src] & (1 << 7))
            glyph[index_src] = glyph[index_src] << 1
    glyphs_dest.append(arr)

with open(file_output,'w', encoding='windows-1251') as f:
    f.write('const uint8_t data[] =\r{\r')
    for glyph in glyphs_dest:
        str = "    "
        for dig in glyph:
            str = str + '0x{0:0{1}X}'.format(dig, 2) + ', '
        f.write(str + '\r')
    f.write('};')
    f.close

os.system('pause')
