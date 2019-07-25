#_*_ coding:UTF-8 _*_
# @author: zdl 
# 实现离线语音唤醒和语音识别，实现一些语音交互控制

# 导入包
import snowboydecoder
import sys
import signal
# import record_monitor as recordMonitor    # pyaudio语音监测程序

interrupted = False

def signal_handler(signal, frame):
    global interrupted
    interrupted = True


def interrupt_callback():
    global interrupted
    return interrupted

#  回调函数，语音识别在这里实现
def callbacks():
    global detector

    #  关闭snowboy功能
    detector.terminate()
    
    # 语音识别
    snowboydecoder.play_audio_file()

    # 打开snowboy功能
    wake_up()    # wake_up —> monitor —> wake_up  递归调用

# 热词唤醒    
def wake_up():

    global detector
    model = sys.argv[1]  #  唤醒词为 SnowBoy
    # capture SIGINT signal, e.g., Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)

    # 唤醒词检测函数，调整sensitivity参数可修改唤醒词检测的准确性
    detector = snowboydecoder.HotwordDetector(model, sensitivity=0.8)
    print('Listening... please say wake-up word:SnowBoy')
    # main loop
    # 回调函数 detected_callback=snowboydecoder.play_audio_file 
    # 修改回调函数可实现我们想要的功能
    detector.start(detected_callback=callbacks,      # 自定义回调函数
                   interrupt_check=interrupt_callback,
                   sleep_time=0.03)
    # 释放资源
    detector.terminate()

if __name__ == '__main__':

    wake_up()

