# encoding: utf-8
import socket
import sys
address = ('192.168.137.88', 6029)  # 服务端地址和端口
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(address)  # 绑定服务端地址和端口
s.listen(5)
conn, addr = s.accept()  # 返回客户端地址和一个新的 socket 连接
print('[+] Connected with', addr)
while True:
    
    send = input('Input: ')
    conn.sendall(send.encode())
conn.close()
s.close()
