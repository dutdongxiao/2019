# -*- coding: utf-8 -*
import serial
from aip import AipFace
from SimpleCV import *  
import urllib
import RPi.GPIO as GPIO
import base64
import time
import os
import sys
import pygame
import serial
import socket
pygame.init()
reload(sys) 
sys.setdefaultencoding('utf-8')
sys.path.append('/home/pi/Desktop/huanxing/rpi/')
from recall import word_to_voice

#百度人脸识别API账号信息
APP_ID = '16672078'
API_KEY = 'fHmr2YpCVT4m1qO56CmKQXrN'
SECRET_KEY ='amdZCAEot5ldIQV69hKx0kBU7laIDpDq'
client = AipFace(APP_ID, API_KEY, SECRET_KEY)#创建一个客户端用以访问百度云
#图像编码方式
IMAGE_TYPE='BASE64'

#用户组
GROUP = 'name1'

#socket与unity的链接
#address = ('192.168.137.88', 6010)  # 服务端地址和端口
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.bind(address)  # 绑定服务端地址和端口
#s.listen(5)
#conn, addr = s.accept()  # 返回客户端地址和一个新的 socket 连接
#print('[+] Connected with', addr)

#照相函数
def getimage():
    
    cam = Camera()
    disp = Display()
    while disp.isNotDone():   
        frame = cam.getImage()
        segment = HaarCascade("face.xml")    
        autoface = frame.findHaarFeatures(segment)    
        if autoface :
            for face in autoface:
                face.draw(Color.RED,1)
            face = autoface[-1].crop()
            face.save(disp)    
            face.save("faceimage.jpg")
        disp.show()
        break
    return
            
        
#对图片的格式进行转换
def transimage():
    f = open('faceimage.jpg','rb')
    img = base64.b64encode(f.read())
    return img
#上传到百度api进行人脸检测
def go_api(image):
    result = client.search(image, IMAGE_TYPE, GROUP);#在百度云人脸库中寻找有没有匹配的人脸
    if result['error_msg'] == 'SUCCESS':#如果成功了
        name = result['result']['user_list'][0]['user_id']#获取名字
        score = result['result']['user_list'][0]['score']#获取相似度
        if score > 80:#如果相似度大于80
            if name == 'yuan':
                print("欢迎您，刘缘")
            if name == 'ya':
                print("欢迎您，陈晓雅 !")
            word_to_voice ("欢迎回家" )
            # 打开串口
            ser = serial.Serial("/dev/ttyAMA0", 115200)
            ser.write(b'5$')
            time.sleep(3)
            ser.write(b'6$')
            ser.close()
            #send = '1'
            #conn.sendall(send.encode())
                        
        else:
            print("对不起，您未通过识别！")
            word_to_voice ("对不起，您未通过识别！")
            name = 'Unknow'
            return 0
        curren_time = time.asctime(time.localtime(time.time()))#获取当前时间

        #将人员出入的记录保存到Log.txt中
        f = open('Log.txt','a')
        f.write("Person: " + name + "     " + "Time:" + str(curren_time)+'\n')
        f.close()
        return 1
    if result['error_msg'] == 'pic not has face':
        print('检测不到人脸')
        word_to_voice ('检测不到人脸')
        time.sleep(3)
        return 0
    else:
        print(result['error_code']+' ' + result['error_code'])
        return 0
#主函数
if __name__ == '__main__':
    print('准备')
    word_to_voice ('开始检测，请在摄像头黄灯亮起后正视镜头')
    while True:
        getimage()#拍照
        img = transimage()#转换照片格式
        res = go_api(img)#将转换了格式的图片上传到百度云
        if(res == 1):#是人脸库中的人
            print("开门")

        else:
            print("无法开门")

   

   
