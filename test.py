

import threading
import time
from websocket_server import WebsocketServer
input = "this is for testing purpose"
output =""
flag =1
# Called for every client connecting (after handshake)
def new_client(client, server):
	print("New client connected and was given id %d" % client['id'])
	server.send_message_to_all("Hey all, a new client has joined us")


# Called for every client disconnecting
def client_left(client, server):
	print("Client(%d) disconnected" % client['id'])

#if len(message) > 200:
	#	message = message[:200]+'..'
# Called when a client sends a message
def message_received(client, server, message):
    global flag
    global input
    if (message =='{"x":"24","y":"42"}'):
        time.time()
    if len(message) > 200:
        message = message[:200]+'..'
    print("Client(%d) said: %s" % (client['id'], message))
    #server.send_message_to_all(input)
    #print ("Send intput: %d", flag)
    #flag = flag +1
    #print(flag)


PORT=8181
server = WebsocketServer(PORT)

#client = WebsocketServer



def call():


    thread1 = threading.Thread(target=internal_update, args=())
    thread1.daemon = True
    thread1.start()

    thread2 = threading.Thread(target=server_run, args=())
    thread2.daemon = True
    thread2.start()
def internal_update():
    global output
    while 1:
        global flag

        temp = str(flag)
        server.send_message_to_all(output)
        time.sleep (0.5)
        #print (flag )
#while 1:
def server_run():
    global flag
    server.set_fn_new_client(new_client)
    server.set_fn_client_left(client_left)
    server.set_fn_message_received(message_received)
	#server.send_message_to_all(message_sent(client,server,"hardcode"));
    #server.send_message_to_all(flag)
    server.run_forever()

def update(message):
    global output
    output = message





def main():
        call()
        while 1:
            update("takata")


       # server_run()
            update ("toyota")


