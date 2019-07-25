#!/usr/bin/env python
#_*_ coding:UTF-8 _*_
#
# 利用热词唤醒后使用百度语音识别api识别语音指令,然后匹配操作指令.如关灯,开灯操作.
###　

import serial
import os
import sys
import urllib3
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
reload(sys)
sys.setdefaultencoding('utf8')
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import time
import pyaudio
import wave
import pygame
import snowboydecoder
import signal
from gpio import GPIO
from aip import AipSpeech
import socket
#百度ip
APP_ID = '16669355'
API_KEY = 'TVzrHV72FLDEaK1oNiIpRjd8'
SECRET_KEY = '5kgjlguHy2f4fIBS8wTHDtpY59UgbPtp'
APIClient = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
#传输配置
#address = ('192.168.137.88', 6003)  # 服务端地址和端口
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.bind(address)  # 绑定服务端地址和端口
#s.listen(5)
#conn, addr = s.accept()  # 返回客户端地址和一个新的 socket 连接
#print('[+] Connected with', addr)

interrupted = False

#定义采集声音文件参数
CHUNK = 1024
FORMAT = pyaudio.paInt16 #16位采集
CHANNELS = 1             #单声道
RATE = 16000             #采样率
RECORD_SECONDS =5       #采样时长 定义为9秒的录音
WAVE_OUTPUT_FILENAME = "./myvoice.pcm"  #采集声音文件存储路径
#open-chunk
#ser = serial.Serial("/dev/ttyAMA0", 115200)

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
#_*_ coding:UTF-8 _*_
# @author: zdl
# 实现离线语音唤醒和语音识别，实现一些语音交互控制

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
        print "**********电扇已关闭**********"
    elif rText.find("关闭排风扇")!=-1:
        fan.off()
        #输出结果语音
        rTe= rText + "命令已完成"
        word_to_voice(rTe)
        send = '3'
        conn.sendall(send.encode())
        print "**********电扇已开启**********"
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
        # 打开串口
        ser = serial.Serial("/dev/ttyAMA0", 115200)
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

if __name__ == '__main__':
    #初始化pygame,让之后播放语音合成的音频文件
    pygame.mixer.init()
    p = pyaudio.PyAudio()
    fan=Fan()
    wake_up()
    #关闭互传
    conn.close()
    s.close()
