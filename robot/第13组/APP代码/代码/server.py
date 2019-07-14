
from socket import *
from time import ctime

HOST = ''
PORT = 55500
BUFSIZ = 1024
ADDR = (HOST,PORT)

tcpSerSock = socket(AF_INET,SOCK_STREAM)
tcpSerSock.bind(ADDR)
tcpSerSock.listen(5)
while True:
    print('waiting for connection...')
    tcpCliSock, addr = tcpSerSock.accept()
    data = tcpCliSock.recv(BUFSIZ)
    data = data.decode()
    print (data)
  
tcpSerSock.close()