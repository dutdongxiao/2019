# -*- coding: utf-8 -*
import serial
import time
import os
import sys
# 打开串口
ser = serial.Serial("/dev/ttyAMA0", 115200)
def send(mode):
    ser.write(mode)
    
    
if __name__ == '__main__':
    try:
        send()
    except KeyboardInterrupt:
        if ser != None:
            ser.close()
