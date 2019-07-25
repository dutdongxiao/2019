# -*- coding: utf-8 -*
#!/usr/bin/env python
from aip import AipFace
from SimpleCV import *  
import urllib
import urllib3
import RPi.GPIO as GPIO
import base64
import time
import os
import sys
import pygame
import serial
import socket
import pyaudio
import wave
import snowboydecoder
import signal
from gpio import GPIO
from aip import AipSpeech

pygame.init()
reload(sys) 
sys.setdefaultencoding('utf-8')
sys.path.append('/home/pi/Desktop/huanxing/rpi/')
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import urllib3
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
interrupted = False

#百度人脸识别API账号信息
APP_ID = '16672078'
API_KEY = 'fHmr2YpCVT4m1qO56CmKQXrN'
SECRET_KEY ='amdZCAEot5ldIQV69hKx0kBU7laIDpDq'
client = AipFace(APP_ID, API_KEY, SECRET_KEY)#创建一个客户端用以访问百度云
APP_ID1 = '16669355'
API_KEY1 = 'TVzrHV72FLDEaK1oNiIpRjd8'
SECRET_KEY1 = '5kgjlguHy2f4fIBS8wTHDtpY59UgbPtp'
APIClient = AipSpeech(APP_ID1, API_KEY1, SECRET_KEY1)
#图像编码方式
IMAGE_TYPE='BASE64'

#用户组
GROUP = 'name1'

#socket与unity的链接
address = ('192.168.137.88', 7006)  # 服务端地址和端口
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(address)  # 绑定服务端地址和端口
s.listen(5)
conn, addr = s.accept()  # 返回客户端地址和一个新的 socket 连接
print('[+] Connected with', addr)

#定义采集声音文件参数
CHUNK = 1024
FORMAT = pyaudio.paInt16 #16位采集
CHANNELS = 1             #单声道
RATE = 16000             #采样率
RECORD_SECONDS =5       #采样时长 定义为9秒的录音
WAVE_OUTPUT_FILENAME = "./myvoice.pcm"  #采集声音文件存储路径

class Fan():

    def __init__(self):
        self.pin=18
        self.mode=1 #open is 1 close is 0
        self.mgpio=GPIO()
        self.mgpio.setPinMode(pin=self.pin,mode=1) #OUTPUT 1 INPUT 0

    def on(self):
        ''
        self.mgpio.setV(self.pin,self.mode)

    def off(self):
        ''
        self.mgpio.setV(self.pin,self.mode&0)

    def status(self):
        #0 is off 1 is on
        return self.mgpio.getV(self.pin)



def get_file_content(filePath):
    with open(filePath, 'rb') as fp:
        return fp.read()


def word_to_voice(text):
    result = APIClient.synthesis(text, 'zh', 1, {
        'vol': 5, 'spd': 3, 'per': 0})
    if not isinstance(result, dict):
        with open('./audio.mp3', 'wb') as f:
            f.write(result)
            f.close()
    time.sleep(.2)
    pygame.mixer.music.load('./audio.mp3')#text文字转化的语音文件
    pygame.mixer.music.play()
    print('waiting')
    #while pygame.mixer.music.get_busy() == True:
        #print('waiting')


def  get_mic_voice_file(p):
    word_to_voice('您好，请输入命令.')
 
    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)
    print("* recording")
 
    frames = []
    for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
        data = stream.read(CHUNK)
        frames.append(data)
    print("* done recording")
    stream.stop_stream()
    stream.close()
    #p.terminate()#这里先不使用p.terminate(),否则 p = pyaudio.PyAudio()将失效，还得重新初始化。
    wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(p.get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(b''.join(frames))
    wf.close()
    print('recording finished')



def  baidu_get_words(client):
    results = client.asr(get_file_content(WAVE_OUTPUT_FILENAME), 'pcm', 16000, { 'dev_pid': 1536, })
    # print(results['result'])
    words=results['result'][0]
    return words


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
            face.show()
            #time.sleep(0.5)
            #disp.close()
            break
            
        
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
            #ser = serial.Serial("/dev/ttyAMA0", 115200)
            #ser.write(b'5$')
            #ser.close()
            #time.sleep(3)
            #ser = serial.Serial("/dev/ttyAMA0", 115200)
            #ser.write(b'6$')
            #ser.close()
            send = '1'
            conn.sendall(send.encode())
                       
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
# 导入包

def signal_handler(signal, frame):
    global interrupted
    interrupted = True


def interrupt_callback():
    global interrupted
    return interrupted

#  回调函数，语音识别在这里实现
def callbacks():
    global detector

    # 语音唤醒后，提示ding两声
    # snowboydecoder.play_audio_file()
    pygame.mixer.music.load('./resources/ding.wav')#text文字转化的语音文件
    pygame.mixer.music.play()
    print('waiting')
    #while pygame.mixer.music.get_busy() == True:
        #print('waiting')
    #snowboydecoder.play_audio_file()

    #  关闭snowboy功能
    detector.terminate()
    #  开启语音识别
    get_mic_voice_file(p)
    rText=baidu_get_words(client=APIClient)
    if rText.find("开启排风扇")!=-1:
        fan.on()
        #输出结果语音
        rTe= rText + "命令已完成"
        word_to_voice(rTe)
        send = '2'
        conn.sendall(send.encode())
        print "**********排风扇已开启**********"
    elif rText.find("关闭排风扇")!=-1:
        fan.off()
        #输出结果语音
        rTe= rText + "命令已完成"
        word_to_voice(rTe)
        send = '3'
        conn.sendall(send.encode())
        print "**********排电扇已关闭**********"
    elif rText.find("开启白天模式")!=-1:
        # 打开串口
        ser = serial.Serial("/dev/ttyAMA0", 115200)
        ser.write(b'1$')
        ser.close()
        #输出结果语音
        rTe= rText + "命令已完成"
        word_to_voice(rTe)
        send = '4'
        conn.sendall(send.encode())
        print "**********白天模式开启**********"
    elif rText.find("开启睡眠模式")!=-1:
        # 打开串口
        ser = serial.Serial("/dev/ttyAMA0", 115200)
        ser.write(b'3$')
        ser.close()
        #输出结果语音
        rTe= rText + "命令已完成"
        word_to_voice(rTe)
        send = '5'
        conn.sendall(send.encode())
        print "**********睡眠模式开启**********"
    elif rText.find("开启傍晚模式")!=-1:
        # 打开串口
        ser = serial.Serial("/dev/ttyAMA0", 115200)
        ser.write(b'4$')
        ser.close()
        #输出结果语音
        rTe= rText + "命令已完成"
        word_to_voice(rTe)
        send = '7'
        conn.sendall(send.encode())
        print "**********傍晚模式开启**********"
    elif rText.find("开启工作模式")!=-1:
        # 打开串口
        ser = serial.Serial("/dev/ttyAMA0", 115200)
        ser.write(b'2$')
        ser.close()
        #输出结果语音
        rTe= rText + "命令已完成"
        word_to_voice(rTe)
        send = '6'
        conn.sendall(send.encode())
        print "**********工作模式开启**********"
    elif rText.find("更新温湿度")!=-1:
        ser = serial.Serial("/dev/ttyAMA0", 115200)
        ser.write(b'7$')
        time.sleep(0.2)
        count = ser.inWaiting()
        recv = ser.read(count)
        print(recv)
        #ser.write(recv[1:-1])
        ser.close()
        #输出结果语音
        rTe= rText + "命令已完成"
        word_to_voice(rTe)
        send = recv
        conn.sendall(send.encode())
        print "**********温湿度已更新**********"
    elif rText.find("退出")!=-1:
        word_to_voice('成功退出语音识别')
        jieshou()
        sys.exit()
    else:
        print "**********unknown command*********"
        word_to_voice('未知指令')
    
    # 打开snowboy功能
    wake_up()    # wake_up —> monitor —> wake_up  递归调用

# 热词唤醒
def wake_up():

    global detector
    model = './resources/snowboy.pmdl'  #  唤醒词为 魔镜魔镜
    # capture SIGINT signal, e.g., Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)

    # 唤醒词检测函数，调整sensitivity参数可修改唤醒词检测的准确性
    detector = snowboydecoder.HotwordDetector(model, sensitivity=0.5)
    print('Listening... please say wake-up word:魔镜魔镜')
    # main loop
    # 回调函数 detected_callback=snowboydecoder.play_audio_file
    # 修改回调函数可实现我们想要的功能
    detector.start(detected_callback=callbacks,      # 自定义回调函数
                   interrupt_check=interrupt_callback,
                   sleep_time=0.03)
    # 释放资源
    detector.terminate()
    
def chuankou():
    ser = serial.Serial("/dev/ttyAMA0", 115200)
    #ser.write(b'1')
    while True:
        # 获得接收缓冲区字符
        count = ser.inWaiting()
        if count == 1:
            # 读取内容并回显,转移至人脸识别
            recv = ser.read(count)
            print(recv)
            print('准备')
            word_to_voice ('开始检测，请在摄像头黄灯亮起后正视镜头')
            getimage()#拍照
            img = transimage()#转换照片格式
            res = go_api(img)#将转换了格式的图片上传到百度云
            time.sleep(3)
            if(res == 1):#是人脸库中的人
                print("开门")
                print('*******开启语音识别*******')
                ser = serial.Serial("/dev/ttyAMA0", 115200)
                ser.write(b'5$')
                time.sleep(3)
                ser.write(b'6$')
                ser.close() 
                pygame.init()
                pygame.mixer.init()
                wake_up()
            else:
                print('未成功识别')
            
        # 清空接收缓冲区
        ser.flushInput()
        # 必要的软件延时
        time.sleep(1)
def jieshou():
    print('进入接收模式')
    while True:
        data = conn.recv(1024)  # buffersize 等于 1024
        data = data.decode()
        if not data:
            break
        print('[Received]', data)
        if data== '1':#打开蜂鸣器
            # 打开串口
            ser = serial.Serial("/dev/ttyAMA0", 115200)
            ser.write(b'8$')
            ser.close()
        elif data=='2':#关闭蜂鸣器
             # 打开串口
            ser = serial.Serial("/dev/ttyAMA0", 115200)
            ser.write(b'9$')
            ser.close()
        elif data=='4':
             # 打开串口
            ser = serial.Serial("/dev/ttyAMA0", 115200)
            ser.write(b'1$')
            ser.close()    
        elif data=='5':
             # 打开串口
            ser = serial.Serial("/dev/ttyAMA0", 115200)
            ser.write(b'3$')
            ser.close()
        elif data=='6':
             # 打开串口
            ser = serial.Serial("/dev/ttyAMA0", 115200)
            ser.write(b'2$')
            ser.close()
        elif data=='7':
             # 打开串口
            ser = serial.Serial("/dev/ttyAMA0", 115200)
            ser.write(b'4$')
            ser.close()
    
#主函数
if __name__ == '__main__':
     #初始化pygame,让之后播放语音合成的音频文件
    pygame.mixer.init()
    p = pyaudio.PyAudio()
    fan=Fan()
    chuankou()
    
    #关闭互传
    conn.close()
    s.close()
