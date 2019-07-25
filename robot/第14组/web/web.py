# -*- coding:utf-8 -*-
from bottle import get,post,run,request,template
import serial

@get("/")
def index():
    return template("demo") 
@post("/cmd")
def cmd():
    count=0
    ser=serial.Serial("/dev/ttyAMA0",9600)
    adss=request.body.read().decode()
    if(adss == "front"):
	ser.write(('A').encode("ascii"))
    elif(adss == "leftFront"):
	ser.write(('L').encode("ascii"))
    elif(adss == "rightFront"):
	ser.write(('R').encode("ascii"))
    elif(adss == "pause"):
	ser.write(('S').encode("ascii"))
    elif(adss == "rear"):
	ser.write(('B').encode("ascii"))
    elif(adss == "add"):
	ser.write(('P').encode("ascii"))
    elif(adss == "minus"):
	ser.write(('M').encode("ascii"))
	
run(host="0.0.0.0")  
