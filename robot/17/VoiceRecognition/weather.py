# -*- coding: utf-8 -*-
"""
Created on Mon Jul  8 14:16:50 2019

@author: 10762
"""
import time
import pyaudio
import pygame
import json
import requests
import wave
import snowboydecoder
from aip import AipSpeech
from imp import reload
from pydub import AudioSegment


APP_ID = '16698599'
API_KEY = 'gKDUdHWV5c78WZUAj5HvDKr5'
SECRET_KEY = 'pT5vRwNjTXMZGAXG5pUNwcVGCeehIGrt'
URL = "http://openapi.tuling123.com/openapi/api/v2"
HEADERS = {'Content-Type': 'application/json;charset=UTF-8'}
client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)

voice_text = ""   # 定义一个全局变量，保存语音转换文字的结果

def begin():
    '''
    voice1 = client.synthesis('您想聊点什么', 'zh', 1, {
    'vol': 5,
    'spd': 4,
    'pit': 5,
    'per': 4,
    })

    # 识别正确返回语音二进制 错误则返回dict 参照下面错误码
    if not isinstance(voice1,dict):
        with open('/home/pi/Desktop/VoiceRecognition//robot/begin.mp3', 'wb') as f:
            f.write(voice1)
    '''
    pygame.init()
    track1 = pygame.mixer.music.load('/home/pi/Desktop/VoiceRecognition/SnowBoy/begin.mp3')
    pygame.mixer.music.play()
    time.sleep(3)#根据mp3时长进行休眠
    pygame.mixer.music.stop()

def rec(out_file, rec_time):#   out_file:输出音频文件名,rec_time:音频录制时间(秒)
    CHUNK = 1024
    FORMAT = pyaudio.paInt16 #16bit编码格式
    CHANNELS = 1 #单声道
    RATE = 16000 #16000采样频率
    p = pyaudio.PyAudio()#pyaudio包
    # 创建音频流 
    stream = p.open(format=FORMAT, # 音频流wav格式
                    channels=CHANNELS, # 单声道
                    rate=RATE, # 采样率16000
                    input=True,
                    frames_per_buffer=CHUNK)
    print("666")
    snowboydecoder.play_audio_file()
    frames = [] # 录制的音频流
    for i in range(0, int(RATE / CHUNK * rec_time)):
        data = stream.read(CHUNK,exception_on_overflow=False)
        frames.append(data)
        
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

#语音转换文字函数
def voice_to_text(filename):
    global flag
    with open('/home/pi/Desktop/VoiceRecognition/SnowBoy/question.wav', 'rb') as f:
        audio_data = f.read()
    result = client.asr(audio_data, 
                        'wav',
                        16000, {
                        'dev_pid': 1536,
                        })
    if result["err_msg"] == "success.":#表示识别成功
        global voice_text
        voice_text = result['result'][0].encode('utf-8')
        if voice_text.find("再见") != -1:
            flag=-1
        else:
            #print(voice_text)
            text_to_voice('/home/pi/Desktop/VoiceRecognition/SnowBoy/answer.mp3',voice_text)
            #play('/home/pi/Desktop/VoiceRecognition/answer.mp3')
            flag=1
    else:
        print("我没有听清您想说什么")
        text_to_voice('/home/pi/Desktop/VoiceRecognition/SnowBoy/answer.mp3',"我没有听清您想说什么")
        play('/home/pi/Desktop/VoiceRecognition/SnowBoy/answer.mp3',5)
        flag=0

#将机器人回复的文字变为语音输出(文字转语音)
def text_to_voice(filename, text=""):
    result = client.synthesis(text, 'zh', 1, {
    'vol': 5,
    'spd': 3,
    'pit': 5,
    'per': 0,
    })
    if not isinstance(result, dict):
        with open(filename, 'wb') as f:
            f.write(result)
            f.close()
    
def tuning_robot(text=""):
    data = {
        "reqType": 0,
        "perception": {
            "inputText": {
                "text": ""
            },
            "selfInfo": {
                "location": {
                    "city": "大连市",
                    "street": "甘井子区"
                }
            }
        },
        "userInfo": {
            "apiKey":"4f94c52200fe42c3ab7295e4cfcfb8f0",
            
            "userId":"1076297425"  # 这个值可以自己定义，不要超过32位
        }
    }

    data["perception"]["inputText"]["text"] = text
    response = requests.request("post", URL, json=data, headers=HEADERS)
    response_dict = json.loads(response.text)

    result = response_dict["results"][0]["values"]["text"]
    return result
#上传一个json格式的请求，获取回复后再从对象中提出文本

def play(filename):
    pygame.init()
    track3 = pygame.mixer.music.load(filename)
    pygame.mixer.music.play()
    sound=AudioSegment.from_file(filename)
    audio_time = sound.duration_seconds
    time.sleep(audio_time)#根据mp3时长进行休眠
    pygame.mixer.music.stop()    
    # 识别正确返回语音二进制 错误则返回dict 参照下面错误码    

def main():
    print("您想聊点什么")
    begin()
    while True:
        rec('/home/pi/Desktop/VoiceRecognition/SnowBoy/question.wav',5)
        voice_to_text('/home/pi/Desktop/VoiceRecognition/SnowBoy/question.wav')
        if  flag == 1:
            robot_said = tuning_robot(voice_text)
            text_to_voice('/home/pi/Desktop/VoiceRecognition/SnowBoy/answer.mp3', robot_said)
            play('/home/pi/Desktop/VoiceRecognition/SnowBoy/answer.mp3')
        elif flag == 0:
            print("请再说一遍")
            text_to_voice('/home/pi/Desktop/VoiceRecognition/SnowBoy/answer.mp3', "请再说一遍")
            play('/home/pi/Desktop/VoiceRecognition/SnowBoy/answer.mp3')
        else:
            text_to_voice('/home/pi/Desktop/VoiceRecognition/SnowBoy/answer.mp3', "再见，我会想你的")
            play('/home/pi/Desktop/VoiceRecognition/SnowBoy/answer.mp3')
            break
if __name__ == "__main__":
    main()
