#! /usr/bin/python

import serial
import time

ser = serial.Serial('/dev/ttyUSB0', timeout = 0)  # open serial port
print(ser.name)         # check which port was really used

while (True):
    a  = ser.read()
    if not a:
        continue

    i = ord(a)
    print 'Received counter ', i
    if i == 4:
        break 
        

ser.write(chr(1))     # write a string
time.sleep(1)
ser.close()     # close port