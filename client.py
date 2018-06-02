import socket
import sys
import random
import time

def connectToHost(port):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	server_address = ('localhost', port)
	sock.connect(server_address)
	return sock

def sendMsg(sock, messages):
	for message in messages:
		sock.sendall(message.encode('utf-8'))
		time.sleep(0.01)
	


msg = ["a","b","c","d","close"]

s = connectToHost(6000)
sendMsg(s,msg)
