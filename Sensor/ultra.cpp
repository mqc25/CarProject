
#include "piDuino.h"
#include "ultra.h"
#include "../Util/Util.h"

// defines pins numbers
/*
//Left
const int trigPinL = 25;
const int echoPinL = 14;
//Right
const int trigPinR = 9;
const int echoPinR = 24;
//Front
const int trigPinF = 7;
const int echoPinF = 18;
//Front left
const int trigPinFL = 8;
const int echoPinFL = 15;
//Front right
const int trigPinFR =11;
const int echoPinFR = 23;*/


const int trigPin[5] = {9, 11, 7, 8, 25};
const int echoPin[5] = {24, 23, 18,15, 14};


float Ultra_reading[5] = {0,0,0,0,0};

void setupSonarArray() {
	
	for (int i = 0; i< 5; i++){
		pinMode (trigPin[i], OUTPUT);
		pinMode (echoPin[i], INPUT);
	}
}


float get_distance(int trig, int echo){
	float distance, duration;
	digitalWrite(trig, LOW); 
	 delayMicroseconds(2); 

	 digitalWrite(trig, HIGH);
	 delayMicroseconds(10); 
	 
	 digitalWrite(trig, LOW);
	 duration = pulseIn(echo, HIGH);
	 
	 distance = (duration/58.2);
	 
	return distance; 
}	



void update_ultra (){
	float tempReading = 0;
	
	for (int i = 0; i < 5; i++)
	{
		tempReading = get_distance(trigPin[i],echoPin[i]);
		if(tempReading > 100) continue;
		Ultra_reading[i] += 0.25*tempReading;
		Ultra_reading[i] /= 1.25;
	}

}


