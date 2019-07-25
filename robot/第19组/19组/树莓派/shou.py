# -*- coding: utf-8 -*
import serial
import time

ser = serial.Serial("/dev/ttyAMA0",115200)

def main():
    ser.write(b"Raspberry pi is ready")
    print("ok!")
    if ser.isOpen == False:
        ser.open()               
    while True:
        count = ser.inWaiting()
        if count != 0:
            recv = ser.read(count)
            print(recv)
            ser.write(recv)
        ser.flushInput()
        time.sleep(0.1)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        if ser != None:
            ser.close()