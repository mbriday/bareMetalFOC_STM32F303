#! /usr/bin/env python3
# -*- coding: UTF-8 -*-
from math import *

nbVal = 256       #nb values for 90°
nbDataPerLine = 8 #number of values per row
curVal = 0        #internal var for pretty print

res = 1./(1<<15)
print('/* sin 0 to 90 degree. 256 values')
print(' * format fixed point: 1.15')
print(' * resolution 1/32768 ~> '+str(res))
print(' * interval [-1,'+str(1-res)+']')
print(' **/')
print("const int16_t sinTab["+str(nbVal)+"] = {\n\t",end='')

for idx in range (nbVal):
    print('0x{s:04x}'.format(s=(int)(sin(idx*pi/(2*nbVal))*32768)),end='')
    if idx != (nbVal-1): #except last value
        print(', ',end="")
        if idx % nbDataPerLine == (nbDataPerLine-1):
            print('\n\t',end='')
    curVal += 1
print('};')

