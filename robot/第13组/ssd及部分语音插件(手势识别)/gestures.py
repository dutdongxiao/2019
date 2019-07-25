import base64
import urllib
import urllib.request
import urllib.parse
#import serial
def get_target_value(key, dic, tmp_list):
    """
    :param key: 目标key值
    :param dic: JSON数据
    :param tmp_list: 用于存储获取的数据
    :return: list
    """
    if not isinstance(dic, dict) or not isinstance(tmp_list, list):  # 对传入数据进行格式校验
        return 'argv[1] not an dict or argv[-1] not an list '

    if key in dic.keys():
        tmp_list.append(dic[key])  # 传入数据存在则存入tmp_list
    else:
        for value in dic.values():  # 传入数据不符合则对其value值进行遍历
            if isinstance(value, dict):
                get_target_value(key, value, tmp_list)  # 传入数据的value值是字典，则直接调用自身
            elif isinstance(value, (list, tuple)):
                _get_value(key, value, tmp_list)  # 传入数据的value值是列表或者元组，则调用_get_value
    return tmp_list

def _get_value(key, val, tmp_list):
    for val_ in val:
        if isinstance(val_, dict):  
            get_target_value(key, val_, tmp_list)  # 传入数据的value值是字典，则调用get_target_value
        elif isinstance(val_, (list, tuple)):
            _get_value(key, val_, tmp_list)  

request_url = "https://aip.baidubce.com/rest/2.0/image-classify/v1/gesture"

f = open('cxk.jpg', 'rb')
img = base64.b64encode(f.read())


params = {"image":img}
params = urllib.parse.urlencode(params).encode()

access_token = '24.24193c98acc5ba665056944a51ea795d.2592000.1563628922.282335-16587859'
request_url = request_url + "?access_token=" + access_token
request = urllib.request.Request(url=request_url, data=params)
print(request)
request.add_header('Content-Type', 'application/x-www-form-urlencoded')
response = urllib.request.urlopen(request)
content = response.read()
content = content.decode('utf-8')
content = eval(content)

if content:
    print (content)
    
print(type(content))

return_value = get_target_value('classname', content, [])
print(return_value)

#ser = serial.Serial("COM3", baudrate=1000000)
for k in return_value:
    if k=='Heart_single' or k=='Heart_1' or k=='Heart_2' or k=='Heart_3':
        print('heart')
        #ch = 'heart'   #input()   #ch为需要传输的字符
        #ser.write(ch)
    elif k=='Thumb_down':
        print('diss')
        #ch = 'diss'   #input()   #ch为需要传输的字符
        #ser.write(ch)
    elif k=='Thumb_up':
        print('zan')
        #ch = 'diss'   #input()   #ch为需要传输的字符
        #ser.write(ch)
    else :
        print('others')
        

#ch = 'heart'   #input()   #ch为需要传输的字符
#ser.write(ch)
ch ='heart'
print(ch)

    