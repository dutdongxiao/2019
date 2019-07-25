import cv2 
import numpy as np
import time

def resize_photo(imgArr,MAX_WIDTH = 1000):
    img = imgArr
    rows, cols= img.shape[:2]    
    if cols >  MAX_WIDTH:
        change_rate = MAX_WIDTH / cols
        img = cv2.resize(img ,( MAX_WIDTH ,int(rows * change_rate) ), interpolation = cv2.INTER_AREA)
    return img


def chose_licence_plate(contours,Min_Area = 10000,Max_Area = 20000):
    temp_contours = []
    for contour in contours:
        if cv2.contourArea( contour ) > Min_Area and cv2.contourArea( contour ) < Max_Area:
            temp_contours.append(contour)
    L=len(temp_contours)
    x=y=w=h=0
    for cnt in temp_contours:
        (x, y, w, h) = cv2.boundingRect(cnt) 
        cv2.rectangle(img, pt1=(x, y), pt2=(x+w, y+h),color=(0, 255, 0), thickness=2)
    
    return img,L,x,y,w,h
def T_HSV(hsv,x,y,w,h):
    H=0
    S=0
    V=0
    X=0
    for i in range(x,x+w+1):
        for j in range(y,y+h+1):
            if hsv[j][i][2]>=255 and hsv[j][i][1]==0:
                continue
            elif hsv[j][i][2]==0:
                continue
            else:
                H+=hsv[j][i][0]
                S+=hsv[j][i][1]
                V+=hsv[j][i][2]
                X=X+1
    
    H=H/X
    S=S/X
    V=V/X
    print(H,S,V)
    cv2.imshow("digit1.jpg", img)
    return H,S,V

def separate_color(frame,h,s,v):                              
    hsv=frame
    lower_hsv = np.array([h-30, s-70, 0])                                      
    high_hsv = np.array([h+30, 255, 255])                                     
    mask = cv2.inRange(hsv, lowerb = lower_hsv, upperb = high_hsv)           
    return mask

def predict(imageArr):
    img_copy = imageArr.copy()
    gray_img = cv2.cvtColor(img_copy , cv2.COLOR_BGR2GRAY)
    gray_img_ = cv2.GaussianBlur(gray_img, (5,5), 0, 0, cv2.BORDER_DEFAULT)
    ret, img_thresh = cv2.threshold(gray_img_, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    img_edge = cv2.Canny(img_thresh, 100, 200)
    contours, hierarchy = cv2.findContours(img_edge, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)


    for cnt in contours:
        (x, y, w, h) = cv2.boundingRect(cnt) 
        cv2.rectangle(img_copy, pt1=(x, y), pt2=(x+w, y+h),color=(0, 255, 0), thickness=2)
        
    cv2.imshow("digit0.jpg", img_copy) 

    return contours 

L=0
cap = cv2.VideoCapture(0)
cap.set(3,320)
cap.set(4,240)
while L!=1:
    

    ret,img=cap.read()
    img = resize_photo(img)
    contours = predict(img)
    img_,L,x,y,w,h=chose_licence_plate(contours)
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    if L==1:
        H,S,V=T_HSV(hsv,x,y,w,h)
        break
    k = cv2.waitKey(1) & 0xff
    if k==27:
        break


ser=serial.Serial("COM6 ",9600)


while L==1:
    ret,img=cap.read()
    img = resize_photo(img)
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    M=separate_color(hsv,H,S,V)
    img_edge = cv2.Canny(M, 100, 200)
    contours, hierarchy = cv2.findContours(img_edge, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    temp_contours = []
    for contour in contours:
        if cv2.contourArea( contour ) > 2000:
            temp_contours.append(contour)
    print(len(temp_contours))
    if len(temp_contours)!=1:
        continue
    for cnt in temp_contours:
        (x, y, w, h) = cv2.boundingRect(cnt)
    if x<(320-x-w)/2:
        ser.write(('l').encode('ascii'))
    elif x/2>(320-x-w):
        ser.write(('r').encode('ascii'))
    for i in range(h):
        if M[y+i][x+w/4]==255:
            ylu=y+i
            break
    for i in range(h):
        if M[y+i][x+3*w/4]==255:
            yru=y+i
            break
    for i in range(h):
        if M[y+h-i][x+w/4]==255:
            yld=y+h-i
            break
    for i in range(h):
        if M[y+h-i][x+3*w/4]==255:
            yrd=y+h-i
            break
    if (yld-ylu)<4*(yrd-yru)/5:
        ser.write(('T').encode('ascii'))
    elif 4*(yld-ylu)/5>(yrd-yru):
        ser.write(('t').encode('ascii'))
    #cv2.imshow("M", M)
    k = cv2.waitKey(1) & 0xff
    if k==27:
        break
            
cap.release()
cv2.destroyAllWindows()

