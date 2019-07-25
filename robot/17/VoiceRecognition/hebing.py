# -*- coding: utf-8 -*-
"""
Created on Sat Jul  6 16:00:15 2019

@author: 10762
"""

# 用Pyaudio库录制音频
    
from aip import AipSpeech
from imp import reload
import pygame
import os
import wave
import pyaudio
import serial
import time
import demo
import weather

APP_ID = '16698599'
API_KEY = 'gKDUdHWV5c78WZUAj5HvDKr5'
SECRET_KEY = 'pT5vRwNjTXMZGAXG5pUNwcVGCeehIGrt'

# 新建一个AipSpeech
client1 = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
client2 = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
client3 = AipSpeech(APP_ID, API_KEY, SECRET_KEY)
client4 = AipSpeech(APP_ID, API_KEY, SECRET_KEY)

def begin():
    '''
    voice1 = client1.synthesis('请在蜂鸣器响后开始说话', 'zh', 1, {
    'vol': 5,
    'spd': 4,
    'pit': 5,
    'per': 4,
    })

    # 识别正确返回语音二进制 错误则返回dict 参照下面错误码
    if not isinstance(voice1,dict):
        with open('/home/pi/Desktop/VoiceRecognition/begin.mp3', 'wb') as f:
            f.write(voice1)
    '''
    pygame.init()
    track1 = pygame.mixer.music.load('/home/pi/Desktop/VoiceRecognition/begin.mp3')
    pygame.mixer.music.play()
    time.sleep(4)#根据mp3时长进行休眠
    pygame.mixer.music.stop()

def finish():
    '''
    voice2 = client2.synthesis('录音结束', 'zh', 1, {
    'vol': 5,
    'spd': 4,
    'pit': 5,
    'per': 4,
    })
    
    # 识别正确返回语音二进制 错误则返回dict 参照下面错误码
    if not isinstance(voice2,dict):
        with open('/home/pi/Desktop/VoiceRecognition/finish.mp3', 'wb') as f:
            f.write(voice2)
    '''
    pygame.init()
    track2 = pygame.mixer.music.load('/home/pi/Desktop/VoiceRecognition/finish.mp3')
    pygame.mixer.music.play()
    time.sleep(4)#根据mp3时长进行休眠
    pygame.mixer.music.stop()

def again():
    '''
    voice3 = client3.synthesis('我没有听清，请再说一遍', 'zh', 1, {
    'vol': 5,
    'spd': 4,
    'pit': 5,
    'per': 4,
    })
    if not isinstance(voice3,dict):
        with open('/home/pi/Desktop/VoiceRecognition/again.mp3', 'wb') as f:
            f.write(voice3)
    '''
    pygame.init()
    track3 = pygame.mixer.music.load('/home/pi/Desktop/VoiceRecognition/again.mp3')
    pygame.mixer.music.play()
    time.sleep(4)#根据mp3时长进行休眠
    pygame.mixer.music.stop()

def bug():
    '''
    voice3 = client3.synthesis('再见，我会想你的', 'zh', 1, {
    'vol': 5,
    'spd': 4,
    'pit': 5,
    'per': 4,
    })
    if not isinstance(voice3,dict):
        with open('/home/pi/Desktop/VoiceRecognition/beybey.mp3', 'wb') as f:
            f.write(voice3)
    '''
    pygame.init()
    track3 = pygame.mixer.music.load('/home/pi/Desktop/VoiceRecognition/beybey.mp3')
    pygame.mixer.music.play()
    time.sleep(4)#根据mp3时长进行休眠
    pygame.mixer.music.stop()    

def audio_record(out_file, rec_time):#   out_file:输出音频文件名,rec_time:音频录制时间(秒)
    ser = serial.Serial("/dev/ttyAMA0", 115200)
    CHUNK = 1024
    FORMAT = pyaudio.paInt16 #16bit编码格式
    CHANNELS = 1 #单声道
    RATE = 16000 #16000采样频率

    print("录音开始...")
    begin()
    '''
    pygame.init()
    track = pygame.mixer.music.load('/home/pi/Desktop/VoiceRecognition/ding.mp3')
    pygame.mixer.music.play()
    time.sleep(1)
    pygame.mixer.music.stop()
    '''
    ser.write("z$".encode("utf-8"))
    ser.flushInput()
    time.sleep(0.1)
    p = pyaudio.PyAudio()#pyaudio包
    # 创建音频流 
    stream = p.open(format=FORMAT, # 音频流wav格式
                    channels=CHANNELS, # 单声道
                    rate=RATE, # 采样率16000
                    input=True,
                    frames_per_buffer=CHUNK)

    #time.sleep(2)

    frames = [] # 录制的音频流
    # 录制音频数据
    for i in range(0, int(RATE / CHUNK * rec_time)):
        data = stream.read(CHUNK,exception_on_overflow=False)
        frames.append(data)

    print("录音结束！")
    finish()    
    # 录制完成
    stream.stop_stream()
    stream.close()
    p.terminate()#用于出现异常时的终止行为

    # 保存音频文件
    wf = wave.open(out_file, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(p.get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(b''.join(frames))
    wf.close()   
    
# 读取文件
def get_file_content(filePath):   #filePath  待读取文件名
    with open(filePath, 'rb') as fp:
        return fp.read()

def stt(filename):         # 语音识别
    # 识别本地文件
    ser = serial.Serial("/dev/ttyAMA0", 115200)
    result = client4.asr(get_file_content(filename),
                        'wav',
                        16000,
                        {'dev_pid': 1536,}      # dev_pid参数表示识别的语言类型 1536表示普通话
                        )
    print (result)

	# 解析返回值，打印语音识别的结果
    if result['err_msg']=='success.':#表示识别成功
        word = result['result'][0].encode('utf-8')       # utf-8编码
        if word!='':
            if word[len(word)-3:len(word)]=='，':
                print (word[0:len(word)-3] + "\n")
                with open('/home/pi/Desktop/VoiceRecognition/wenzi.txt','w') as f:#以二进制流读写
                    word=word.decode('utf-8')
                    f.write(word[0:len(word)-3]) 
                f.close()
            else:
                print (word[0:len(word)].decode('utf-8'))#(word.decode('utf-8').encode('gbk'))#gbk
                check(word,ser)
        else:
            print ("音频文件不存在或格式错误")
            bug()      
    else:
        print ("语音识别错误")
        again()
        audio_record('/home/pi/Desktop/VoiceRecognition/yuyin.wav', 3)
        get_file_content('/home/pi/Desktop/VoiceRecognition/yuyin.wav')
        stt('/home/pi/Desktop/VoiceRecognition/yuyin.wav')
        
   # check(word,ser)
        
def check(word,ser):
    if word.find("小车前进") != -1:
        ser.write("F$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("聊天") != -1:
        weather.main()
    elif word.find("退出") != -1:
        bug()
    elif word.find("小车后退") != -1:
        ser.write("B$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("小车左转") != -1:
        ser.write("L$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("小车右转") != -1:
        ser.write("R$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("小车停止") != -1:
        ser.write("S$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("摄像头抬头") != -1:
        ser.write("u$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("摄像头低头") != -1:
        ser.write("d$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("摄像头左转") != -1:
        print("666")
        ser.write("l$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("摄像头右转") != -1:
        ser.write("r$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("摄像头复位") != -1:
        ser.write("o$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif word.find("摄像头停止") != -1:
        ser.write("s$".encode('utf-8'))
        ser.flushInput()
        time.sleep(0.1)
    elif True:
        print("666")
        again()
        audio_record('/home/pi/Desktop/VoiceRecognition/yuyin.wav', 4)
        get_file_content('/home/pi/Desktop/VoiceRecognition/yuyin.wav')
        stt('/home/pi/Desktop/VoiceRecognition/yuyin.wav')
def main():
    #begin()
    audio_record('/home/pi/Desktop/VoiceRecognition/yuyin.wav', 3)
    #finish()
    get_file_content('/home/pi/Desktop/VoiceRecognition/yuyin.wav')
    stt('/home/pi/Desktop/VoiceRecognition/yuyin.wav')
# main函数 调用以上函数
if __name__ == "__main__":
    main()
            
        
    
