# coding=utf-8

import re
import sys
reload(sys)
sys.setdefaultencoding('utf8')
sys.path.append('/home/pi/.local/lib/python2.7/site-packages')
from aip import AipSpeech
import pyaudio
import wave
import os
import requests
import json
import time
import cv2
import shutil
import pygame
import control as co
import subprocess

APP_ID = '11331887'
API_KEY = 'GPUn5PqodL0wjhu1RA2l9fF5'
SECRET_KEY = 'QqTbFp612taFirROMcx1Cd0oi9acz1M0'
'''
APP_ID = '16663271'
API_KEY = 'LbkqhGSGQHrwtevv937UAotd'
SECRET_KEY = 'c9oribIUZZSzNzQz8W5qpobwzflhUKpI'
'''
client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000
WAVE_OUTPUT_FILENAME = "test.wav"
QA_OUTPUT_FILENAME = './QA/temp.wav' 

TOP_DIR = os.path.dirname(os.path.abspath(__file__))
DETECT_DING = os.path.join(TOP_DIR, "./resources/Oldboy.wav")

HELLOW_OUTPUT_FILENAME = os.path.join(TOP_DIR, "./resources/hellow.wav")
SIGNUP_OUTPUT_FILENAME = os.path.join(TOP_DIR, "./resources/singup.wav")
QUESTION_OUTPUT_FILENAME = os.path.join(TOP_DIR, "./resources/question.wav")
ANSWER_OUTPUT_FILENAME = os.path.join(TOP_DIR, "./resources/answer.wav")

SEARCH_PATH = os.path.join(TOP_DIR, './data')
# play hi
def play_sound(filepath):
	'''
	ding_wav = wave.open(filepath, 'rb')
	ding_data = ding_wav.readframes(ding_wav.getnframes())
	audio = pyaudio.PyAudio()
	stream_out = audio.open(
		format=audio.get_format_from_width(ding_wav.getsampwidth()),
		channels=ding_wav.getnchannels(),
		rate=ding_wav.getframerate(), input=False, output=True)
	stream_out.start_stream()
	stream_out.write(ding_data)
	time.sleep(0.2)
	stream_out.stop_stream()
	stream_out.close()
	audio.terminate()
	'''
	pygame.init()
	pygame.mixer.music.load(filepath)
	pygame.mixer.music.play()
	time.sleep(10)
	pygame.mixer.music.stop()
	
# get audio input
def get_input(frames, p, RECORD_SECONDS):
	p = pyaudio.PyAudio()
	stream = p.open(format = FORMAT,
			channels = CHANNELS,
			rate = RATE,
			input = True,
			frames_per_buffer = CHUNK)
	print ('speak')
	for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
		data = stream.read(CHUNK)
		frames.append(data)
	print ('end')
	stream.stop_stream()
	stream.close()
	p.terminate()

# write wav
def write_wav(file_path, frams):
	wf = wave.open(file_path, 'w')
	wf.setnchannels(CHANNELS)
	wf.setsampwidth(p.get_sample_size(FORMAT))
	wf.setframerate(RATE)
	wf.writeframes(b''.join(frames))
	wf.close()

# read file
def get_file_content(file_path):
	with open(file_path, 'rb') as fp:
		return fp.read();

# recognize file
def get_chinese(file_path):
	result = (client.asr(get_file_content(file_path),'wav', 16000, {
		'dev_pid' : 1536,}
	))
	if result['err_no'] == 0:
		return result['result']
	return '0'

def create_training_frame_file(filepath):
	folder = os.path.exists(filepath)
	if not folder:
		os.makedirs(filepath)

def get_and_save_frame(filepath):
	vc = cv2.VideoCapture(0)
	cnt = 0
	time = 1
	timeF = 10
	while 1:
		rval, frame = vc.read()
		#gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
		#cv2.imshow('frame',gray)
		#cv2.waitKey(1)
		if(time % timeF == 0):
			# if py2
			# cv2.imwrite(filepath+'/'+str(cnt)+'.jpg', frame)
			# if py3
			cv2.imencode('.jpg', frame)[1].tofile(filepath+'/'+str(cnt)+'.jpg')
			cnt += 1
		time += 1
		if(time > 500):
			break
	vc.release()
	
def copy_frame(framepath, filepath):
	alllist = os.listdir(framepath)
	for i in alllist:
		oldname = framepath + i
		newname = filepath + i
		shutil.copyfile(oldname, newname)
		
def tuling(text='I said nothing'):
	tuling_url = 'http://www.tuling123.com/openapi/api'
	tuling_date = {
		'key': '2fd4afcd632048af80e5634f08c1a492',
		'info': text
	}
	r = requests.post(tuling_url, data=tuling_date)
	return json.loads(r.text)['text']

def play_tuling(reply):
	# 写入mp3
	result  = client.synthesis(reply, 'zh', 1, {'vol': 5,'per':4})
	if not isinstance(result, dict):
		with open('auido.mp3', 'wb') as f:
			f.write(result)
	# pygame播放
	file = r'auido.mp3'
	pygame.mixer.init()
	track = pygame.mixer.music.load(file)
	pygame.mixer.music.play()
	time.sleep(10)
	pygame.mixer.music.stop()

def pygame_play(file):
	pygame.mixer.init()
	track = pygame.mixer.music.load(file)
	pygame.mixer.music.play()
	time.sleep(10)
	pygame.mixer.music.stop()

if __name__ == '__main__':
	pygame_play(r'hello.mp3')
	pro = subprocess.Popen('python test.py', shell = True)
	while(1):
		p = pyaudio.PyAudio()
		frames = []
		get_input(frames, p, 4)
		p.terminate()
		write_wav(WAVE_OUTPUT_FILENAME, frames)
		order = get_chinese(WAVE_OUTPUT_FILENAME)[0]
		print(order)
		if order == '0':
			continue
		if order == u'再见':
			pygame_play(r'goodbye.mp3')
			exit(1)
		'''
		# python3
		analysis = re.search('(我是)(/D+)', order)
		print(analysis.group())
		print(analysis.group(1))
		print(analysis.group(2))
		if analysis:
			if analysis.group(1) == '我是':
				print(1)
				if analysis.group(2):
					filepath = './data/'+analysis.group(2)
					print(filepath)
					create_training_frame_file(filepath)
					get_and_save_frame(filepath)
					print(analysis.group(2))
				else:
					pass
			else:
				reply = tuling(order)
				print(reply)
		'''

		if order:
			if '前' in order:
				co.send_signal('1')
				continue
			if '后' in order:
				co.send_signal('2')
				continue
			if '左' in order:
				co.send_signal('3')
				continue
			if '右' in order:
				co.send_signal('4')
				continue
			if '停' in order:
				co.send_signal('0')
				continue
			if '跟' in order:
				co.send_signal('f')
				continue
			'''
			if '开' in order:
				# pro = os.system('python facerec.py ./data/generate ./data/datamape.csv')
				pro = subprocess.Popen('python facerec.py ./data/generate ./data/datamape.csv')
				continue
			if '关' in order:
				# pro.kill()
				os.killpg(os.getpgid(pro.pid), 9)
				pygame_play(r'goodbye.mp3')
				continue
			'''
			# 添加人脸识别用户
			if not -1 == order.find('我是'):
				order = order.replace('我是','')
				if order:
					# 生成一般用户
					if -1 == order.find('老师'):
						filepath = './data/'+order
						generatepath = './data/generate/'+order
						# print(filepath)
						create_training_frame_file(filepath)
						create_training_frame_file(generatepath)
						framepath = './frame/'
						copy_frame(framepath, filepath+'/')
						os.system('python gene.py '+filepath)
						os.system('python create_csv.py ./data/generate/ > ./data/datamap.csv')
						txtpath = './cnt/' + order + '.txt'
						f = open(txtpath, 'w')
						f.close()
						# get_and_save_frame(filepath)
					# 特殊用户‘老师’
					else:
						all_names = os.listdir(SEARCH_PATH)
						sign = 0
						for i in all_names:
							if '老师' == i:
								sign = 1
								break
						# 老师尚未生成
						if not sign:
							filepath = './data/'+order
							generatepath = './data/generate/'+order
							# print(filepath)
							create_training_frame_file(filepath)
							create_training_frame_file(generatepath)
							framepath = './frame/'
							copy_frame(framepath, filepath+'/')
							os.system('python gene.py '+filepath)
							os.system('python create_csv.py ./data/generate/ > ./data/datamap.csv')
							txtpath = './cnt/' + order + '.txt'
							f = open(txtpath, 'w')
							f.close()
						# 进行QA生成
						else:
							# 写入Q
							fw = open('./QA/list.txt','a')
							p = pyaudio.PyAudio()
							frames = []
							pygame_play(r'question.mp3')
							get_input(frames, p, 4)
							p.terminate()
							write_wav(QA_OUTPUT_FILENAME, frames)
							question = get_chinese(QA_OUTPUT_FILENAME)[0]
							fw.write(question + '\n')
							fw.close()
							# 生成A
							p = pyaudio.PyAudio()
							frames = []
							pygame_play(r'answer.mp3')
							get_input(frames, p, 10)
							ans_path = './QA/' + question + '.wav'
							write_wav(ans_path, frames)
							p.terminate()
							pygame_play(r'accept.mp3')
 
				else:
					pass
			#对话或者Q&A
			else:
				fr = open('./QA/list.txt','r')
				lines = fr.readlines()
				sign = 0
				for line in lines:
					line = line.strip()
					if not -1 == order.find(line):
						sign = 1
						break
				fr.close()
				# Q&A
				if sign:
					answer_path = './QA/' + line + '.wav'
					print (answer_path)
					play_sound(answer_path)
				# 对话
				else:	
					reply = tuling(order)
					play_tuling(reply)
					print(reply)
