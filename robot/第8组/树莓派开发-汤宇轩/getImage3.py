import cv2
from hyperlpr import *
import time
import RPi.GPIO as GPIO
import serial

ser = serial.Serial("/dev/ttyAMA0", 9600)

list = ['鲁R00309']
tempList = []

def getImage():
    cv2.namedWindow("Image")
    cap = cv2.VideoCapture(0)
    i = 0

    cap.set(3, 400)
    cap.set(4, 300)

    while (1):
        ret, frame = cap.read()
        cv2.imshow("Image", frame)
        k = cv2.waitKey(1)
        if k == 27:
            break
        path = './jpgs/' + str(i) + '.png'
        cv2.imwrite(path, frame)
        i += 1
        image = cv2.imread(path)
        res = HyperLPR_PlateRecogntion(image)
        if i == 20:
            for j in range(0, 20):
                os.remove('./jpgs/' + str(j) + '.png')
                i = 0
        if res:
            said = False
            print(res)
            tempList.append(res[0][0])
            if res[0][0] in list:
                GPIO.output(pin_up, True)
                print('say yes :' + res[0][0])
                said = True
                time.sleep(0.05)
                tempList.clear()
                GPIO.output(pin_up, False)
            if len(tempList) == 20:
                map = {}
                if not said:
                    for s in tempList:
                        if not map.get(s):
                            map[s] = 1
                        else:
                            map[s] = map.get(s) + 1
                    for s in tempList:
                        if s in list:
                            if not said:
                                print('say yes :' + s)
								ser.write('@TextToSpeech#车辆' + s + '准入' + '$'.encode('gbk'))
								said = True
								GPIO.output(pin_up, True)
								time.sleep(0.05)
								tempList.clear()
								GPIO.output(pin_up, False)
                if said:
                    tempList.clear()
                    continue
                s = max(map, key = map.get)
                if len(s) is 7:
                    print('say no ' + s)
					ser.write('@TextToSpeech#车辆' + s + '请绕行' + '$'.encode('gbk'))
                tempList.clear()
        time.sleep(0.05)

    cv2.imshow("capture", frame)
    cap.release()
    cv2.destroyAllWindows()
	
pin_up = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_up, GPIO.OUT)

getImage()
