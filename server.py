import socket
from threading import Thread
import time

import pyServer
import json
import time

data = []

def establishHost(port):
	HOST = 'localhost'
	
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

	s.bind((HOST, port))
	
	s.listen(1)
	conn, addr = s.accept()
	
	return s, conn

def getPositionData(conn):
	while True:
		data = conn.recv(1024)
		if not data: break
		#print(data.decode('utf-8'))
		x,y = data.decode('utf-8').split(",")
		x = int(float(x))
		y = int(float(y))
		#print(x,y)
		#temp = pyServer.JSON_convert(int(x),int(y))
		#print(temp)
		test = {
					"y": y,
					"x": x
				}
		temp =json.dumps(test)
		pyServer.update(temp)
		print(temp)
		time.sleep(0.1)
		
        
    		
def sendMsg(s, messages):
	for message in messages:
		s.sendall(message.encode('utf-8'))
		time.sleep(0.5)

sock, conn = establishHost(8000)
pyServer.call()
thread = Thread(target = getPositionData, args = [conn])
thread.start()

#msg = ["a","b","c","d","close"]
#sendMsg(conn,msg)
thread.join()
sock.close()
