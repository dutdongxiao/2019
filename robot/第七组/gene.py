
#!/usr/bin/env python
#coding=utf-8
import cv2
import os  
import sys

face_cascade=cv2.CascadeClassifier('./lbpcascade_frontalface.xml')

def generate(filepath):
    count=0
    for filename in os.listdir(filepath): 
        if filename == ".directory":
            continue
        imgPath = os.path.join(filepath, filename)
        try:
            print 'read:'+imgPath
            img = cv2.imread(imgPath)
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            faces=face_cascade.detectMultiScale(gray,1.3,5)
            for x,y,w,h in faces:
                f=cv2.resize(gray[y:y+h,x:x+w],(200,200))
                out = filepath.replace('./data/','')
                outPath=os.path.join('data','generate',out,'%s.pgm' % str(count))
                print 'write:'+ outPath
                cv2.imwrite(outPath, f)
                count+=1

        except:
            print "error"
            pass


if __name__ == '__main__':
    if len(sys.argv)==1:
        exit(0)
    filepath = sys.argv[1]
    generate(filepath)
