import socket
from threading import Thread
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
    		print(data.decode('utf-8'))
    		
def sendMsg(s, messages):
	for message in messages:
		s.sendall(message.encode('utf-8'))
		time.sleep(0.5)

sock, conn = establishHost(8000)
thread = Thread(target = getPositionData, args = [conn])
thread.start()

msg = ["a","b","c","d","close"]
sendMsg(conn,msg)
thread.join()
sock.close()
