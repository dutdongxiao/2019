import serial
import time

ser = serial.Serial("/dev/ttyAMA0", 115200)

def main():
    ser = serial.Serial("/dev/ttyAMA0", 115200)
    ser.write(b"Raspberry pi is ready")
    print("connected")
    while True:
            msg = raw_input()
            ser.write(msg)
            ser.flushInput()
            time.sleep(0.1)
            flag=1

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        if ser != None:
            ser.close()
