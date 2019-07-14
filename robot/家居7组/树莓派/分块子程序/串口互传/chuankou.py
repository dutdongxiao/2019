# -*- coding: utf-8 -*
import serial
import time
import os
import sys
# 打开串口
ser = serial.Serial("/dev/ttyAMA0", 115200)
sys.path.append('./face/')
def main():
    #ser.write(b'5')
    while True:
        # 获得接收缓冲区字符
        count = ser.inWaiting()
        if count == 1:
            # 读取内容并回显,转移至人脸识别
            recv = ser.read(count)
            print(recv)
            ser.write(recv)
            os.system("python /home/pi/Desktop/chuank/face/facess.py")
            sys.exit()
        # 清空接收缓冲区
        ser.flushInput()
        # 必要的软件延时
        time.sleep(1)
    
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        if ser != None:
            ser.close()
