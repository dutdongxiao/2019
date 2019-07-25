#!/usr/bin/env python
# coding=utf-8
### Imports ###################################################################

import multiprocessing as mp 
import cv2
import os
import sys
import time
import numpy as np
from PIL import Image, ImageDraw, ImageFont
from pinyin import PinYin

reload(sys)
sys.setdefaultencoding('utf-8')   
### Setup #####################################################################
resX = 320
resY = 240

import serial
ser = serial.Serial('/dev/ttyAMA0', 9600)

def send_signal(s):
	s.encode()
	ser.write(s + b'\n')
	
def steering_control(faces, img):
	if 1 == len(faces):
		for (x, y, w, h) in faces:
			if(x + w / 2) > resX / 2 + w / 2:
				send_signal('r')
			elif(x + w / 2) < resX / 2 - w / 2:
				send_signal('l')
			else:
				send_signal('0')
				time.sleep(0.1)
				if(y + h / 2) < resY / 2 - h / 2:
					send_signal('6')
				elif(y + h / 2) > resY / 2 + h / 2:
					send_signal('5')
			
# The face cascade file to be used
face_cascade = cv2.CascadeClassifier('./haarcascade_frontalface_alt2.xml')
 
#三种识别算法
#model = cv2.createEigenFaceRecognizer()
#model = cv2.createFisherFaceRecognizer()
model = cv2.createLBPHFaceRecognizer()

t_start = time.time()
fps = 0

### Helper Functions ##########################################################


def file_count(filepath):
    cnt = 0
    ls = os.listdir(filepath)
    for i in ls:
        if os.path.isfile(os.path.join(filepath, i)):
            cnt += 1
    return cnt

def normalize(X, low, high, dtype=None):
    """Normalizes a given array in X to a value between low and high."""
    X = np.asarray(X)
    minX, maxX = np.min(X), np.max(X)
    # normalize to [0...1].
    X = X - float(minX)
    X = X / float((maxX - minX))
    # scale to [low...high].
    X = X * (high-low)
    X = X + low
    if dtype is None:
        return np.asarray(X)
    return np.asarray(X, dtype=dtype)


def load_images(path, sz=None):
    c = 0
    X,y = [], []
    for dirname, dirnames, filenames in os.walk(path):
        for subdirname in dirnames:
            subject_path = os.path.join(dirname, subdirname)
            for filename in os.listdir(subject_path):
                try:
                    filepath = os.path.join(subject_path, filename)
                    if os.path.isdir(filepath):
                        continue
                    img = cv2.imread(os.path.join(subject_path, filename), cv2.IMREAD_GRAYSCALE)
                    if (img is None):
                        print ("image " + filepath + " is none")
                    else:
                        print (filepath)
                    # resize to given size (if given)
                    if (sz is not None):
                        img = cv2.resize(img, (200, 200))

                    X.append(np.asarray(img, dtype=np.uint8))
                    y.append(c)
                # except IOError, (errno, strerror):
                #     print ("I/O error({0}): {1}".format(errno, strerror))
                except:
                    print ("Unexpected error:", sys.exc_info()[0])
                    raise
            print (c)
            c = c+1


    print (y)
    return [X,y]

def get_faces( img ):

    gray = cv2.cvtColor( img, cv2.COLOR_BGR2GRAY )
    faces = face_cascade.detectMultiScale(gray, 1.3, 5)

    return faces, img, gray
    
# 尝试解决中文乱码
'''
def change_cv2_draw(image,strs,local,sizes,colour):
    cv2img = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    pilimg = Image.fromarray(cv2img)
    draw = ImageDraw.Draw(pilimg)  # 图片上打印
    font = ImageFont.truetype("./font/simsun.ttc",sizes, encoding="utf-8")
    draw.text(local, strs, colour, font=font)
    image = cv2.cvtColor(np.array(pilimg), cv2.COLOR_RGB2BGR)
    return image

def cv2ImgAddText(img, text, left, top, textColor=(0, 255, 0), textSize=20):
    if (isinstance(img, np.ndarray)):  #判断是否OpenCV图片类型
        img = Image.fromarray(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
    draw = ImageDraw.Draw(img)
    fontText = ImageFont.truetype(
        "./font/simsun.ttc", textSize, encoding="utf-8")
    draw.text((left, top), text, textColor, font=fontText)
    return cv2.cvtColor(np.asarray(img), cv2.COLOR_RGB2BGR)
'''
def change_identity(name):
    f = open('./identity/identity.txt', 'w')
    f.write(name)
    f.close()

def draw_frame( faces, img, gray, move ):

    global xdeg
    global ydeg
    global fps
    global time_t
    
    if move == 2:
        steering_control(faces, img)
    # Draw a rectangle around every face
    for ( x, y, w, h ) in faces:

        cv2.rectangle( img, ( x, y ),( x + w, y + h ), ( 200, 255, 0 ), 2 )
        #-----rec-face
        roi = gray[x:x+w, y:y+h]
        try:
            roi = cv2.resize(roi, (200, 200), interpolation=cv2.INTER_LINEAR)
            params = model.predict(roi)
            if params[1] < 500.0:
                #print (names[params[0]])
                #pec = (' %.2f' % (params[1]))
                #sign = names[params[0]] + pec
                pyin = PinYin()
                pyin.load_word()
                pname = names[params[0]]
                change_identity(pname)
                #pyin.hanzi2pinyin(string = pname)
                pname = pyin.hanzi2pinyin_split(string = pname, split = '')
                s = ''
                for p in pname:
                    s = s + p
                sign=("%s %.2f" % (s, params[1]))
                # print(sign)
                cv2.putText(img, sign, (x, y-2), cv2.FONT_HERSHEY_SIMPLEX, 0.5, ( 0, 0, 255 ), 2 )
                #img = cv2ImgAddText(img, sign , x , y - 2, (0, 0, 255), 20)
                #img = change_cv2_draw(img,sign,(x, y + 2), 20 , 'firebrick' )
        
        except:
            continue
        

    # Calculate and show the FPS
    fps = fps + 1
    sfps = fps / (time.time() - t_start)
    cv2.putText(img, "FPS : " + str( int( sfps ) ), ( 10, 15 ), cv2.FONT_HERSHEY_SIMPLEX, 0.5, ( 0, 0, 255 ), 2 )

    cv2.imshow( "recognize-face", img )

def retraining(names, model):
    f = open('./data/datamap.csv')
    lines = f.readlines()
    for line in lines:
        temp = line.split('/')
        print(temp[3])
        if temp[3] not in names:
            names.append(temp[3])

    [X,y] = load_images(sys.argv[1])
    y = np.asarray(y, dtype=np.int32)

    if len(sys.argv) == 3:
        out_dir = sys.argv[2]

    model.train(np.asarray(X), np.asarray(y))
    f.close()
    


### Main ######################################################################

if __name__ == '__main__':

    camera = cv2.VideoCapture(0)
    camera.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,resX)  
    camera.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,resY) 

    pool = mp.Pool( processes=4 )

    # -----------init-rec----------
    # 人名要与datamap.csv里面的对应，不要弄错了顺序
    names = []
    retraining(names, model)
    # print(names)
    # ------init finish---------

    read, img = camera.read()
    pr1 = pool.apply_async( get_faces, [ img ] )   
    read, img = camera.read()
    pr2 = pool.apply_async( get_faces, [ img ] )  
    read, img = camera.read() 
    pr3 = pool.apply_async( get_faces, [ img ] )   
    read, img = camera.read()
    pr4 = pool.apply_async( get_faces, [ img ] )    

    fcount = 1
    cnt = 0
    move = 0
    update = file_count('./cnt/')
    print(update)
    while (True):
        if not update == file_count('./cnt/'):
            names = []
            retraining(names, model)
            update = file_count('./cnt/')
            print(update)
        read, img = camera.read()
        cv2.imencode('.jpg', img)[1].tofile('./frame/'+str(cnt)+'.jpg')
        cnt = (cnt+1) % 30
        if   fcount == 1:
            pr1 = pool.apply_async( get_faces, [ img ] )
            faces, img, gray=pr2.get()
            draw_frame( faces, img, gray, move )

        elif fcount == 2:
            pr2 = pool.apply_async( get_faces, [ img ] )
            faces, img, gray=pr3.get()
            draw_frame( faces, img, gray, move )

        elif fcount == 3:
            pr3 = pool.apply_async( get_faces, [ img ] )
            faces, img, gray=pr4.get()
            draw_frame( faces, img, gray, move )

        elif fcount == 4:
            pr4 = pool.apply_async( get_faces, [ img ] )
            faces, img, gray=pr1.get()
            draw_frame( faces, img, gray, move )
            fcount = 0

        fcount += 1
        move = (move + 1) % 3
        if cv2.waitKey(1000 // 12) & 0xff == ord("q"):
            break

    camera.release()
    cv2.destroyAllWindows()
