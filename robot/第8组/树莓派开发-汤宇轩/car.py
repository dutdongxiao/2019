import cv2
import socket
import sys
import time, threading
import os
import RPi.GPIO as GPIO
import signal

def getImage(sock, addr):
    cv2.namedWindow("Image")
    cap = cv2.VideoCapture(1)
    i = 0
    test_dir = "jpgs"

    cap.set(3,320)
    cap.set(4,240)

    flag = False

    while True:
        ret, frame = cap.read()
        path = '/home/pi/Desktop/jpgs/ + str(i) + '.jpg'
        cv2.imwrite(path, frame)
        i = i + 1
        string = str(os.path.getsize(path)).encode('utf-8')
        sock.send(string)
        rec = sock.recv(1)
        if rec.decode() == '1':
            print('tran image')
            f = open(path, "rb")
            l = f.read(1024)
            while l:
                sock.send(l)
                l = f.read(1024)
        time.sleep(0.01)
        if i == 21:
            for j in range(0, 20):
                os.remove('/home/pi/Desktop/jpgs/' + str(j) + '.jpg')
        elif i == 1 and flag:
            for j in range(21, 40):
                os.remove('/home/pi/Desktop/jpgs/' + str(j) + '.jpg')
        elif i == 40:
            i = 0
            flag = True

    cv2.imshow("capture", frame)
    cap.release()
    cv2.destroyAllWindows()

def tcplink(sock, addr):
    print('Accept new connection from %s:%s...' % addr)
    pre = ''
    preDirect = '0'
    while True:
        data = sock.recv(1024)
        if len(data) == 0:
            dispose(pre)
        elif data == 'close':
            break
        elif data == 'up' or data == 'down' or data == 'normal':
            pre = dispose(data, preDirection)
        else:
            pre = dispose(data)
            if pre == '0' or pre == '1' or pre == '2' or pre == '3' or pre == '4':
                preDirection = pre
        print(data)
    sock.close()
    print('Connection from %s:%s closed.' % addr)


def dispose(data, direct = '0'):
    if data == '0':
        GPIO.output(pin_forward, False)
        GPIO.output(pin_back, False)
        GPIO.output(pin_left, False)
        GPIO.output(pin_right, False)
        GPIO.output(pin_speedUp, False)
        GPIO.output(pin_speedDown, False)
        print 'standing'
        return '0'
    elif data == '1':
        GPIO.output(pin_forward, True)
        GPIO.output(pin_back, False)
        GPIO.output(pin_left, False)
        GPIO.output(pin_right, False)
        GPIO.output(pin_speedUp, False)
        GPIO.output(pin_speedDown, False)
        print 'forwarding'
        return '1'
    elif data == '2':
        GPIO.output(pin_forward, False)
        GPIO.output(pin_back, True)
        GPIO.output(pin_left, False)
        GPIO.output(pin_right, False)
        GPIO.output(pin_speedUp, False)
        GPIO.output(pin_speedDown, False)
        print 'backing'
        return '2'
    elif data == '3':
        GPIO.output(pin_forward, False)
        GPIO.output(pin_back, False)
        GPIO.output(pin_left, True)
        GPIO.output(pin_right, False)
        GPIO.output(pin_speedUp, False)
        GPIO.output(pin_speedDown, False)
        print 'turn left'
        return '3'
    elif data == '4':
        GPIO.output(pin_forward, False)
        GPIO.output(pin_back, False)
        GPIO.output(pin_left, False)
        GPIO.output(pin_right, True)
        GPIO.output(pin_speedUp, False)
        GPIO.output(pin_speedDown, False)
        print 'trun right'
        return '4'
    elif data == 'up':
        GPIO.output(pin_forward, direct  == '1')
        GPIO.output(pin_back, direct  == '2')
        GPIO.output(pin_left, direct  == '3')
        GPIO.output(pin_right, direct  == '4')
        GPIO.output(pin_speedUp, False)
        GPIO.output(pin_speedDown, True)
        return 'up'
    elif data == 'down':
        GPIO.output(pin_forward, direct == '1')
        GPIO.output(pin_back, direct  == '2')
        GPIO.output(pin_left, direct  == '3')
        GPIO.output(pin_right, direct  == '4')
        GPIO.output(pin_speedUp, True)
        GPIO.output(pin_speedDown, False)
        return 'down'
    elif data == 'normal':
        GPIO.output(pin_forward, direct == '1')
        GPIO.output(pin_back, direct == '2')
        GPIO.output(pin_left, direct == '3')
        GPIO.output(pin_right, direct == '4')
        GPIO.output(pin_speedUp, False)
        GPIO.output(pin_speedDown, False)
    elif data == 't':
        GPIO.output(camera_forward, True)
        GPIO.output(camera_back, False)
        GPIO.output(camera_left, False)
        GPIO.output(camera_right, False)
        print 'camera forwarding'
        return 't'
    elif data == 'b':
        GPIO.output(camera_forward, False)
        GPIO.output(camera_back, True)
        GPIO.output(camera_left, False)
        GPIO.output(camera_right, False)
        print 'camera backing'
        return 'b'
    elif data == 'l':
        GPIO.output(camera_forward, False)
        GPIO.output(camera_back, False)
        GPIO.output(camera_left, True)
        GPIO.output(camera_right, False)
        print 'camera turn left'
        return 'l'
    elif data == 'r':
        GPIO.output(camera_forward, False)
        GPIO.output(camera_back, False)
        GPIO.output(camera_left, False)
        GPIO.output(camera_right, True)
        print 'camera trun right'
        return 'r'

pin_forward = 18
GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_forward, GPIO.OUT)

pin_back = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_back, GPIO.OUT)

pin_left = 22
GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_left, GPIO.OUT)

pin_right = 27
GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_right, GPIO.OUT)

#speed up
pin_speedUp = 21
GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_speedUp, GPIO.OUT)

#speed down
pin_speedDown = 16
GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_speedDown, GPIO.OUT)

camera_forward = 5
GPIO.setmode(GPIO.BCM)
GPIO.setup(camera_forward, GPIO.OUT)

camera_back = 6
GPIO.setmode(GPIO.BCM)
GPIO.setup(camera_back, GPIO.OUT)

camera_left = 13
GPIO.setmode(GPIO.BCM)
GPIO.setup(camera_left, GPIO.OUT)

camera_right = 19
GPIO.setmode(GPIO.BCM)
GPIO.setup(camera_right, GPIO.OUT)

pre = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pre.bind(('192.168.137.1', 8806))
pre.listen(5)
print("8806 waiting for connection!")

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('192.168.137.1', 8807))
s.listen(5)
print("8807 waiting for connection!")

pres, prea = pre.accept()
t2 = threading.Thread(target=tcplink, args=(pres, prea))
t2.start()

sock, addr = s.accept()
t = threading.Thread(target=getImage, args=(sock, addr))
t.start()

