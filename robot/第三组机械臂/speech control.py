#coding=gbk
import wave
import requests
headers = requests.utils.default_headers()
headers['User-Agent'] = 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36'
import time
import base64
from pyaudio import PyAudio, paInt16
import webbrowser
import sys
from aip import AipSpeech
import os
import pygame
#from pocketsphinx import LiveSpeech, get_model_path
from requests.packages import urllib3
import json
import RPi.GPIO as GPIO
import time
import pyaudio
pa = pyaudio.PyAudio()
pa.get_default_input_device_info()
{'defaultLowOutputLatency': 0.008707482993197279, 'maxOutputChannels': 32, 'hostApi': 0, 'defaultSampleRate': 44100.0, 'defaultHighOutputLatency': 0.034829931972789115, 'name': 'default', 'index': 15, 'maxInputChannels': 32, 'defaultHighInputLatency': 0.034829931972789115, 'defaultLowInputLatency': 0.008707482993197279, 'structVersion': 2}
pyaudio.pa.__file__
'/root/.virtualenvs/py3k/lib/python3.4/site-packages/_portaudio.cpython-34m.so'




framerate = 16000  # 采样率
num_samples = 2000  # 采样点
channels = 1  # 声道
sampwidth = 2  # 采样宽度2bytes
FILEPATH = 'speech.wav'

base_url = "https://openapi.baidu.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s"
APP_ID = '16615243'
API_KEY = 'XrxxoBTeeOC1YNFHUxW9Ao6P'
SECRET_KEY = 'lva2knWAIlRgCZMXWwwRLj1QLaBa26KG'

HOST = base_url % (API_KEY, SECRET_KEY)


def getToken(host):
    res = requests.post(host,timeout=10,verify=False)
    return res.json()['access_token']


def save_wave_file(filepath, data):
    wf = wave.open(filepath, 'wb')
    wf.setnchannels(channels)
    wf.setsampwidth(sampwidth)
    wf.setframerate(framerate)
    wf.writeframes(b''.join(data))
    wf.close()


def my_record():#录音并保存为wav文件
    pa = PyAudio()
    stream = pa.open(format=paInt16, channels=channels,
                     rate=framerate, input=True, frames_per_buffer=num_samples)#一个buffer存NUM_SAMPLES个字节,作为一帧

    my_buf = []
    # count = 0
    t = time.time()
    print('正在录音...')
    GPIO.output(18, GPIO.HIGH)
    while time.time() < t + 2:  # 秒
        string_audio_data = stream.read(num_samples)
        my_buf.append(string_audio_data)
    GPIO.output(18, GPIO.LOW)
    print('录音结束.')

    save_wave_file(FILEPATH, my_buf)
    stream.close()


def get_audio(file): #读入wav音频文件中的内容
    with open(file, 'rb') as f:
        data = f.read()
    return data


def speech2text(speech_data, token, dev_pid=1537):#把音频文件中的内容发给服务器
    FORMAT = 'wav'
    RATE = '16000'
    CHANNEL = 1
    CUID = '*******'
    SPEECH = base64.b64encode(speech_data).decode('utf-8')

    data = {
        'format': FORMAT,
        'rate': RATE,
        'channel': CHANNEL,
        'cuid': CUID,
        'len': len(speech_data),
        'speech': SPEECH,
        'token': token,
        'dev_pid':dev_pid
    }
    url = 'https://vop.baidu.com/server_api'
    headers = {'Content-Type': 'application/json'}#服务端根据请求头中的Content-Type字段来获知请求中的消息主体是用何种方式进行编码，再对消息主体进行解析，此处以json串提交数据
    r=requests.post(url,timeout=10,data=json.dumps(data),headers=headers,verify=False)
    print('正在识别...')
    urllib3.disable_warnings()###########
    r = requests.post(url,timeout=10, json=data, headers=headers,verify=False)#requests提供了一个json参数，自动使用json方式发送
    Result = r.json()#Requests中有一个内置的JSON解码器，来处理JSON数据
    if 'result' in Result:
        return Result['result'][0]
    else:
        return Result


def openbrowser(text):
    maps = {
        '百度': ['百度', 'baidu'],
        '腾讯': ['腾讯', 'tengxun'],
        '网易': ['网易', 'wangyi']

    }
    if text in maps['百度']:
        webbrowser.open_new_tab('https://www.baidu.com')
    elif text in maps['腾讯']:
        webbrowser.open_new_tab('https://www.qq.com')
    elif text in maps['网易']:
        webbrowser.open_new_tab('https://www.163.com/')
    else:
        webbrowser.open_new_tab('https://www.baidu.com/s?wd=%s' % text)

if __name__ == '__main__':
   
 GPIO.setmode(GPIO.BCM)
 GPIO.setup(18, GPIO.OUT)  
    
#    
 while 1:
        
     devpid = 1536
     my_record()
     TOKEN = getToken(HOST)
     speech = get_audio(FILEPATH)
     result = speech2text(speech, TOKEN, int(devpid))
     flag='no'
     if '你好' in result:
         flag='yes'
         print('唤醒成功')  
         client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
         pygame.mixer.init()
         track = pygame.mixer.music.load('huanxing.mp3')
         pygame.mixer.music.play()
         time.sleep(3)
         pygame.mixer.music.stop()

     while flag.lower()=='yes':
      
        print('准备录音')
        time.sleep(0.5)
        devpid = 1536
        my_record()
        TOKEN = getToken(HOST)
        speech = get_audio(FILEPATH)
        result = speech2text(speech, TOKEN, int(devpid))
        if '起小' in result:
           print('1')
           url='http://localhost:5000/do?act=control&para=002P2100T2000!'#1500竖直,1900抬起来
           data1 = {}
           urllib3.disable_warnings()######################
           r1 = requests.get(url,timeout=10,params=data1) #发get请求
                      
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('taiqishoubi.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
                     
         
        if '抓' in result:
           print('2')
           url='http://localhost:5000/do?act=control&para=004P1430T2000!'#1430握紧手
           data1 = {}
           urllib3.disable_warnings()####################
           r = requests.get(url,timeout=10,params=data1) #发get请求
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('zhua.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
        if '握手' in result:
           print('7') 
           data1 = {}
           url='http://localhost:5000/do?act=control&para=001P1300T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           time.sleep(1)
           url='http://localhost:5000/do?act=control&para=002P2100T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           time.sleep(1)
           url='http://localhost:5000/do?act=control&para=004P1190T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           time.sleep(1)
           url='http://localhost:5000/do?act=control&para=004P1430T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('woshou.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
           url='http://localhost:5000/do?act=midall'
           urllib3.disable_warnings()####################

           r = requests.get(url,timeout=10,params=data1) #发get请求 
        if '扔' in result:
           print('20') 
           data1 = {}
           url='http://localhost:5000/do?act=control&para=000P800T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           time.sleep(1)
           url='http://localhost:5000/do?act=control&para=004P1190T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           time.sleep(1)
          
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('reng.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
           url='http://localhost:5000/do?act=midall'
           urllib3.disable_warnings()####################

           r = requests.get(url,timeout=10,params=data1) #发get请求            
        if '逆' in result:
           print('3') 
           url='http://localhost:5000/do?act=control&para=003P900T2000!'#1500竖直，900俯视逆时针90度，2200俯视顺时针90度
           data1 = {}
           urllib3.disable_warnings()####################

           r = requests.get(url,timeout=10,params=data1) #发get请求
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('nishizhen.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
          
        if '顺' in result:
           print('4') 
           url='http://localhost:5000/do?act=control&para=003P2200T2000!'#1500竖直，900俯视逆时针90度，2200俯视顺时针90度
           data1 = {}
           urllib3.disable_warnings()####################

           r = requests.get(url,timeout=10,params=data1) #发get请求
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('shunshizhen.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
          
        if '松' in result:
           print('5')
           url='http://localhost:5000/do?act=control&para=004P1190T2000!'#1190松开手
           data1 = {}
           urllib3.disable_warnings()####################

           r = requests.get(url,timeout=10,params=data1) #发get请求
                      
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('songkaishou.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
          
        if '下小' in result:
           print('6') 
           url='http://localhost:5000/do?act=control&para=002P1500T2000!'
           data1 = {}
           urllib3.disable_warnings()####################

           r = requests.get(url,timeout=10,params=data1) #发get请求
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('fangxiashoubi.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
          
        if '再见' in result:
           flag='no' 
           
           data1 = {}
           url='http://localhost:5000/do?act=control&para=002P2200T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           time.sleep(1)
           url='http://localhost:5000/do?act=control&para=001P1100T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
        
           url='http://localhost:5000/do?act=control&para=000P1200T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           time.sleep(1)
           url='http://localhost:5000/do?act=control&para=000P1700T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           time.sleep(1)
           url='http://localhost:5000/do?act=control&para=000P1200T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求
           time.sleep(1)
           url='http://localhost:5000/do?act=control&para=000P1700T1000!'
           r = requests.get(url,timeout=10,params=data1) #发get请求         
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('zaijian.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
           url='http://localhost:5000/do?act=midall'
           urllib3.disable_warnings()####################
           r = requests.get(url,timeout=10,params=data1) #发get请求        
        if '起大' in result:
           print('9') 
           url='http://localhost:5000/do?act=control&para=001P1000T2000!'
           data1 = {}
           urllib3.disable_warnings()####################

           r = requests.get(url,timeout=10,params=data1) #发get请求
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('大臂.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
        if '下大' in result:
           print('10') 
           url='http://localhost:5000/do?act=control&para=001P1500T2000!'
           data1 = {}
           urllib3.disable_warnings()####################

           r = requests.get(url,timeout=10,params=data1) #发get请求
           client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
           pygame.mixer.init()
           track = pygame.mixer.music.load('fangxiadabi.mp3')
           pygame.mixer.music.play()
           time.sleep(3)
           pygame.mixer.music.stop()
            
          
          
          
        print(result)
        continue
        