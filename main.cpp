#include <iostream>
#include <unistd.h>
#include <wiringPi.h>
#include "Car/Car.h"
#include <softPwm.h>
#include <csignal>
#include <cstring>

using namespace std;
Car* car;

float commandX;
float commandY;
float commandTheta;

void testPWM();
void testMotor();

void signalHandler( int signum ) {
   cout << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here  
   // terminate program  
   car->drive(STOP);
   file.close();
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

PI_THREAD (getInput){
	while(true){
		cout<< "Coordinate: x,y,theta: ";
		std::string str;
		getline(cin,str);
		cout<<str<<endl;
		std::vector<std::string> v;
		v = split(str,',');
		if(v.size()!=3) {
			cout<<"Wrong Format"<<endl;
			continue;
		}
		commandX = stof(v.at(0));
		commandY = stof(v.at(1));
		commandTheta = stof(v.at(2));
		cout<<"Execute goToCoordinate2("<<commandX<<","<<commandY<<","<<commandTheta<<")"<<endl;
		car->newCommand = true;
	}
}

void setup(){
	signal(SIGINT, signalHandler); 
	wiringPiSetup();
	car = new Car(0.5);
	
	if(piThreadCreate(stateUpdate)!=0){
		printf("Fail to create stateUpdate thread\n");
		exit(1);
	}	
	/*if(piThreadCreate(getInput)!=0){
		printf("Fail to create getInput thread\n");
		exit(1);
	}*/
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
	car->planToCoordinate2(150,0);
	car->planToCoordinate2(450,0);
	car->planToCoordinate2(450,300);
	car->planToCoordinate2(450,700,0);

	/*
	while(true){
		if(car->newCommand){
			car->newCommand = false;
			car->goToCoordinate2(commandX,commandY,commandTheta);
		}
		delay(200);
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
