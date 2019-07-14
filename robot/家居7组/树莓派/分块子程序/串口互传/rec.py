# -*- coding: utf-8 -*
import serial
import time
import os
import sys
# 打开串口
ser = serial.Serial("/dev/ttyAMA0", 115200)
sys.path.append('./face/')
def rec():
    #ser.write(b'4$')
    while True:
        # 获得接收缓冲区字符
        count = ser.inWaiting()
        if count == 1:
            # 读取内容并回显,转移至人脸识别
            recv = ser.read(count)
            print(recv)
            #ser.write(recv)
            os.system("python /home/pi/Desktop/chuank/face/facess.py")
        elif count != 0:
            recv = ser.read(count)
            print(recv)
            #ser.write(recv)
        #else:
            #print("waiting")
        # 清空接收缓冲区
        ser.flushInput()
        # 必要的软件延时
        time.sleep(0.2)
    
if __name__ == '__main__':
    try:
        rec()
    except KeyboardInterrupt:
        if ser != None:
            ser.close()
