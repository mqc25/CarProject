#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>

using namespace std;
char buffer[1024] = {0};


int getSocket( int port ){
	struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
	printf("\nConnection Success \n");
	return sock;
}  

void listenToServer(int socket){
	int valread=0;
	while(true){
		if( (valread = read( socket , buffer, 1024)) != 0){
			cout<<buffer<<endl;
		}	
		else break;
	}
}

void sendMsg(int socket, string msg){
	char* temp = (char*)msg.c_str();
	send(socket , temp , strlen(temp) , 0 );
}
  
int main(int argc, char const *argv[])
{
    int sock = getSocket(8000);
    
    string msg[5] = {"a1","b2","c3","d4","close"};
    
    thread listening(listenToServer,sock);
  
    
    for (int i =0; i < 5; i++){
		char* temp = (char*)msg[i].c_str();
		send(sock , temp , strlen(temp) , 0 );
		usleep(500000);
		
	}
    printf("Hello message sent\n");
    
    printf("%s\n",buffer );
    listening.join();
    return 0;
}
