#include <iostream>
#include <unistd.h>
#include <wiringPi.h>
#include "Car/Car.h"
#include <softPwm.h>
#include <csignal>
#include <cstring>
#include <string>
#include <vector>
#include <ctime>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
Car* car;

void testPWM();
void testMotor();


vector<Coordinate> destination;

string GetStdoutFromCommand(string cmd) {

	string data;
	FILE * stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	cmd.append(" 2>&1");

	stream = popen(cmd.c_str(), "r");
	if (stream) {
		while (!feof(stream))
		if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
		pclose(stream);
		}
	return data;
}


void signalHandler( int signum ) {
   cout << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here  
   // terminate program  
   car->drive(STOP);
   exit(signum);  
}

PI_THREAD (stateUpdate){
	while(true){
		piLock(0);
		car->updateState();
		piUnlock(0);
		usleep(10000);
	}
}

PI_THREAD (obstacleUpdate){
	while(true){
		piLock(1);
		update_ultra();
		isUpdateUltra = true;
		piUnlock(1);
		usleep(50000);
	}
}


PI_THREAD (updateDestination){
	while(true){
		if(newSignal){
			printf("In updateDestination\r\n");
			newSignal = false;
			float x = atof(strtok (buffer,","));
			float y = atof(strtok (NULL, ","));
			Coordinate temp = Coordinate(x,y);
			destination.push_back(temp);
		}
		usleep(500000);
	}
}

PI_THREAD (listenToServer){
	int valread=0;
	while(true){
		if( (valread = read( conn, buffer, 1024)) != 0){
			printf("In listenToServer\r\n");
			newSignal = true;
		}	
		else exit(1);
	}
}

void setup(){
	setupSocket(8000);
	signal(SIGINT, signalHandler); 
	wiringPiSetup();
	car = new Car(0.6);
	
	if(piThreadCreate(stateUpdate)!=0){
		printf("Fail to create stateUpdate thread\n");
		exit(1);
	}
	if(piThreadCreate(listenToServer)!=0){
		printf("Fail to create stateUpdate thread\n");
		exit(1);
	}	
	if(piThreadCreate(obstacleUpdate)!=0){
		printf("Fail to create stateUpdate thread\n");
		exit(1);
	}	
	
}

void takePicture(){
	delay(1000);
	system("raspistill -o cam4.jpg -t 10 -w 640 -h 480 -q 75 --nopreview");
	string test = GetStdoutFromCommand("./aruco_simple cam4.jpg -c charuco_rasb.yml -s 0.162 -d ARUCO_MIP_36h12");
	cout<<test<<endl;
	vector <string> fields;
	split(fields, test, boost::is_any_of(", "));
	cout<<"Covert: "<<stof(fields[0])<<" "<<stof(fields[1])<<endl;
	float tempX = stof(fields[0]);
	float tempY = stof(fields[1]);
	if(tempX ==0 && tempY ==0) return;
	tempX = (tempX - 0.1*cos(toRad(car->carAngle)))*100;
	tempY = (tempY - 0.1*sin(toRad(car->carAngle)))*100;
	cout<<"Real: "<<tempX<<" "<<tempY<<endl;
	//if(distance2D(Coordinate(car->x,car->y),Coordinate(tempX,tempY)) > 8) return;
	car->x = tempX;
	car->y = tempY;
	
}

int main(){
	cout << "test" << endl;
	setup();
	delay(5000);
	//testMotor();
	//return 0;
	//car->goToCoordinate3(50.0,0,0);
	//car->goToCoordinate3(50.0,50.0,0);
	//car->goToCoordinate3(0.0,50.0,0);
	//car->goToCoordinate3(0.0,0.0,0);
	//car->planToCoordinate2(250.0,0);
	//car->planToCoordinate2(250.0,-300.0);
	car->planToCoordinate2(150,25,-90);
	takePicture();
	car->planToCoordinate2(150,0);
	car->planToCoordinate2(450,0,0);
	takePicture();
	car->planToCoordinate2(450,200,90);
	takePicture();
	car->planToCoordinate2(450,250);
	car->planToCoordinate2(450,700,90);
	takePicture();
	car->planToCoordinate2(450,700,0);

	/*
	while(true){
		if(destination.size() != 0){
			car->planToCoordinate2(float(destination[0].x),float(destination[0].y));
			destination.erase(destination.begin());
			
		}
		
		delay(500);
	} */
	
	free(car);
	return 0;
	
}


void testPWM(){
	car->goToCoordinate2(100.0,100.0,-1.0);
	
	//car->goForwardDistance(100);
	//car->drive(STOP);
	//delay(100);
	//car->turnToAngle(90);
	//car->drive(FORWARD);
	//usleep(5000000);
	//car->drive(STOP);
	
}

void testMotor(){
	printf("Go forward \r\n");
	car->drive(FORWARD);
	delay(2000);
	car->drive(STOP);
	delay(250);
	
	printf("Go backward \r\n");
	car->drive(BACKWARD);
	delay(2000);
	car->drive(STOP);
	delay(250);
	
	printf("Go left \r\n");
	car->drive(LEFT);
	delay(2000);
	car->drive(STOP);
	delay(250);
	
	printf("Go right \r\n");
	car->drive(RIGHT);
	delay(2000);
	car->drive(STOP);
	delay(250);
	
}
