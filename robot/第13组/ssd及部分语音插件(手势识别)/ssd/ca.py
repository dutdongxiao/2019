# -*- coding: utf-8 -*-
"""
Created on Mon Jul  8 20:23:10 2019

@author: liutianyi
"""
import cv2
cap = cv2.VideoCapture(0)
i = 0
ret, img = cap.read()
while (ret and i<30) :
    cv2.imshow('1',img)
    key = cv2.waitKey(100)
    ret, img = cap.read()
    i = i+1
cap.release()
cv2.destroyAllWindows()
