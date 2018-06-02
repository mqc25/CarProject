#include "Car.h"
#include <wiringPi.h>
#include <iostream>

#include <math.h>
#include <softPwm.h>
#include <time.h>

#define SAFE_DISTANCE 30

using namespace std;

int L1 = 25;
int L2 = 29;
int R1 = 27;
int R2 = 28;

int Lspeed = 24;
int Rspeed = 26;

float toX = 0;
float toY = 0;

char buffer[1024];

float previousDistance = 0;
long long counter = 0;
ofstream file("record.csv");
Car::Car():Car(0.0){}

Car::Car(float speed){
	driveSpeed = int(speed*RESOLUTION);
	diff = 0;
	
	encoders[0] = *setupencoder(3,2);
	encoders[1] = *setupencoder(12,0);
	carEncoder = encoders;
	
	pwmSetMode(PWM_MODE_MS);
	pinMode(Lspeed,PWM_OUTPUT);
	pinMode(Rspeed,PWM_OUTPUT);
	pinMode(L1,OUTPUT);
	pinMode(R1,OUTPUT);
	pinMode(L2,OUTPUT);
	pinMode(R2,OUTPUT);
	
	pwmSetClock(1);
	pwmSetRange(RESOLUTION);
	
	pwmWrite(Lspeed,00);
	pwmWrite(Rspeed,00);
	
	digitalWrite(R1,LOW);
	digitalWrite(L1,LOW);
	digitalWrite(L2,LOW);
	digitalWrite(R2,LOW);
	
	setupSonarArray();
	
	setupMPU();
	calibrateMPU();
	
	std::cout<< "Finish Calibrate MPU." << std::endl;
	x = 150;
	y = 160;
	carAngle = 0;
	targetAngle = 0;
	
	Kpt = 0.01*RESOLUTION;
    Kdt = 00;

    Kps = 0.01*RESOLUTION;
    Kds = 00;

    lastErrorT = 0;
    lastErrorS = 0;

    timerState = micros();
    timerPID = micros();
	
	std::cout<< "Finish Initialization. Speed: " << (int)driveSpeed << std::endl;
	
}

void Car::drive(Direction dir){
	float rightSpeed = driveSpeed + diff;
	float leftSpeed = driveSpeed - diff;
	
	if( leftSpeed < 0) leftSpeed =0;
	if( rightSpeed < 0) rightSpeed = 0;
	
	switch(dir){
		case FORWARD:
			pwmWrite(Lspeed,leftSpeed);
			pwmWrite(Rspeed,rightSpeed);
			digitalWrite(L1,HIGH);
			digitalWrite(L2,LOW);
			digitalWrite(R1,HIGH);
			digitalWrite(R2,LOW);
			break;
			
		case BACKWARD:
			pwmWrite(Lspeed,leftSpeed);
			pwmWrite(Rspeed,rightSpeed);
			digitalWrite(L1,LOW);
			digitalWrite(L2,HIGH);
			digitalWrite(R1,LOW);
			digitalWrite(R2,HIGH);
			break;
			
		case LEFT:
			pwmWrite(Lspeed,leftSpeed);
			pwmWrite(Rspeed,rightSpeed);
			digitalWrite(L1,LOW);
			digitalWrite(L2,HIGH);
			digitalWrite(R1,HIGH);
			digitalWrite(R2,LOW);
			break;
			
		case RIGHT:
			pwmWrite(Lspeed,leftSpeed);
			pwmWrite(Rspeed,rightSpeed);
			digitalWrite(L1,HIGH);
			digitalWrite(L2,LOW);
			digitalWrite(R1,LOW);
			digitalWrite(R2,HIGH);
			break;
		
		default:
			pwmWrite(Lspeed,0);
			pwmWrite(Rspeed,0);
			digitalWrite(L1,HIGH);
			digitalWrite(L2,LOW);
			digitalWrite(R1,HIGH);
			digitalWrite(R2,LOW);
			
	}
}

void Car::updateState()
{
    updateMPU();
    angle += (angle > 180) ? -360 : (angle < -180) ? 360 : 0;
    carAngle = angle;
    
    long long leftEncoder = carEncoder[0].value;
    long long rightEncoder = carEncoder[1].value;

    float currentDistance = (leftEncoder + rightEncoder) * 0.5 * 6 * M_PI / CYCLE;
    float diffDistance = currentDistance - previousDistance;
    previousDistance = currentDistance;

    x += diffDistance*cos(toRad(carAngle));
    y += diffDistance*sin(toRad(carAngle));
    
	
	
	//sprintf(buffer,"CSV_FORMAT_IMU,%lld,%.2f,%.2f,%.2f\r\n",++counter,x,y,carAngle);
	//file << buffer; 

	
    if(micros() - timerState> 1000000){
		//checkForObstacle();
		//for(Coordinate obs: obstacleList) printf("Obs Position: x: %d y: %d\r\n",obs.x,obs.y);

        printf("left: %lld right: %lld diff: %lld speed: %d \r\n",
                  leftEncoder, rightEncoder, leftEncoder - rightEncoder,driveSpeed);
        printf("x: %.2f y: %.2f carAngle: %.2f diff: %.2f\r\n",
                  x, y, carAngle, angleDif(targetAngle, carAngle));   
        printf("Heading to %.2f, %.2f\r\n",toX,toY); 
      
        timerState = micros();
    }
}

void Car::PID_Straight_Gyro()
{
    float error = angleDif(targetAngle, carAngle);

    long temp = micros();
    float dt = 0.000001 * (temp - timerPID);
    timerPID = temp;
    float dError = (error - lastErrorS) * dt;
    lastErrorS = error;

    diff = Kpt * error + Kdt * dError;
    if (diff > 0.35 * RESOLUTION)
        diff = 0.35 * RESOLUTION;
    else if (diff < -.35 * RESOLUTION)
        diff = -0.35 * RESOLUTION;
    drive(FORWARD);
    if(micros() - timerState> 1000000){
        printf("L: %lld R: %lld E:%.2f \r\n", carEncoder[0].value, carEncoder[1].value, error);
        printf("P: %.2f D: %.2f diff: %d\r\n\r\n", Kpt * error, Kdt * dError, diff);
    }
}

void Car::turnToAngle(float toAngle)
{
	drive(STOP);
    delay(100);
    targetAngle = toAngle;
    int original = driveSpeed;
    int slow = 0.75*driveSpeed;
    while (absolute(angleDif(carAngle,targetAngle)) > 1.0)
    {
		if(absolute(angleDif(carAngle,targetAngle)) <30.) {
			driveSpeed = slow;
        }
        PID_Straight_Gyro();
        if (newCommand){
			driveSpeed = original;
            return;
        }
        delay(25);
    }
    drive(STOP);
    delay(100);
    driveSpeed = original;
    printf("Turn done\r\n");
}

void Car::staticTurn(float toAngle){
	//drive(STOP);
	//delay(100);
	diff = 0;
	targetAngle = toAngle;
	float diffAngle = angleDif(targetAngle,carAngle);
	int original = driveSpeed;
	int slow = 1.1*driveSpeed;
	driveSpeed = slow;
	while( absolute(diffAngle) > 1){
		if(diffAngle > 0 ) {
			drive(LEFT);
		}
		else {
			drive(RIGHT);
		}
		
		if(absolute(diffAngle) < 30) driveSpeed = 0.85*slow;
		delay(20);
		diffAngle = angleDif(targetAngle,carAngle);
	}
	//drive(STOP);
	//delay(100);
	driveSpeed = original;
}

void Car::goForwardDistance(float distance)
{
    drive(STOP);
    delay(100);
    float currentDistance = (carEncoder[0].value + carEncoder[1].value) * 0.5 * 6 * M_PI / CYCLE;
    while (currentDistance < distance)
    {
        PID_Straight_Gyro();
        if (newCommand)
            return;
        delay(50);
        currentDistance = (carEncoder[0].value + carEncoder[1].value) * 0.5 * 6 * M_PI / CYCLE;
    }
    drive(STOP);
    delay(100);
    printf("Drive done\r\n");
}

void Car::checkForObstacle(){
	update_ultra();
	
	float tempx, tempy;
	float tempx_s, tempy_s, angleToS, angleCal;
	
	
	float angleToSensor[] = {-45,-45,0,45,45};
	float angleOffSet[] = {-90,-55,0,55,90};
	
	for (int i=0; i<5; i++){
		if(Ultra_reading[i] > 80) continue;
		
		angleToS = toRad(carAngle + angleToSensor[i]);
		angleCal = toRad(carAngle + angleOffSet[i]);
		tempx_s = x + 10*cos(toRad(angleToS));
		tempy_s = y + 10*sin(toRad(angleToS));
		tempx = tempx_s + Ultra_reading[i]*cos(angleCal);
		tempy = tempy_s + Ultra_reading[i]*sin(angleCal);
		obstacleList.insert(Coordinate(tempx,tempy));
		printf("Obstacle Cal: %.2f %.2f\r\n",tempx,tempy);
	}
	
	printf("Ultra reading: %.2f %.2f %.2f %.2f %.2f\r\n", Ultra_reading[0],Ultra_reading[1],Ultra_reading[2],Ultra_reading[3],Ultra_reading[4]);
	
}


void Car::goToCoordinate1(float xTarget, float yTarget, float angleTarget ){
	drive(STOP);
	delay(100);
	float dx = 0;
	float dy = 0;
	float distanceLeft = 100000;
	int original = driveSpeed;
	int slow = int(0.8*driveSpeed);
	while( distanceLeft > 10){
		dx = xTarget - x;
		dy = yTarget - y;
		distanceLeft = sqrtf(dx*dx + dy*dy);
		targetAngle = atan2f(dy,dx)*180.0/M_PI;
		
		if(distanceLeft < 50){
			driveSpeed = slow;
		}
		
		PID_Straight_Gyro();
		delay(25);
		if(newCommand) {
			driveSpeed = original;
			return;
		}
	}
	driveSpeed = original;
	drive(STOP);
	delay(100);
	printf("Arrived\r\n");
}

void Car::goToCoordinate2(float xTarget, float yTarget, float angleTarget ){
	drive(STOP);
	delay(100);
	float dx = 0;
	float dy = 0;
	float distanceLeft = 100000;
	int original = driveSpeed;
	int slow = int(0.8*driveSpeed);
	
	dx = xTarget - x;
	dy = yTarget - y;
	distanceLeft = sqrtf(dx*dx + dy*dy);
	targetAngle = atan2f(dy,dx)*180.0/M_PI;
	staticTurn(targetAngle);
	
	while( distanceLeft > 10){
		dx = xTarget - x;
		dy = yTarget - y;
		distanceLeft = sqrtf(dx*dx + dy*dy);
		targetAngle = atan2f(dy,dx)*180.0/M_PI;
		
		if(distanceLeft < 50){
			driveSpeed = slow;
		}
		
		PID_Straight_Gyro();
		delay(25);
		if(newCommand) {
			driveSpeed = original;
			return;
		}
	}
	driveSpeed = original;
	drive(STOP);
	delay(100);
	printf("Arrived\r\n");
}

void Car::goToCoordinate3(float xTarget, float yTarget, float angleTarget){
	toX = xTarget;
	toY = yTarget;
	drive(STOP);
	delay(100);
	float dx = 0;
	float dy = 0;
	float distanceLeft = 0;
	int original = driveSpeed;
	int slow = int(0.8*driveSpeed);
	
	dx = xTarget - x;
	dy = yTarget - y;
	distanceLeft = sqrtf(dx*dx + dy*dy);
	targetAngle = atan2f(dy,dx)*180.0/M_PI;
	if(absolute(angleDif(targetAngle,carAngle)>30)){
		staticTurn(targetAngle);
	}
	while( distanceLeft > 7.0){
		
		if(distanceLeft < 50){
			driveSpeed = slow;
		}
		
		PID_Straight_Gyro();
		delay(20);
		//if(newCommand) {
		//	driveSpeed = original;
		//	return;
		//}
		dx = xTarget - x;
		dy = yTarget - y;
		distanceLeft = sqrtf(dx*dx + dy*dy);
		targetAngle = atan2f(dy,dx)*180.0/M_PI;
	}
	driveSpeed = original;
	//drive(STOP);
	//delay(100);
	//printf("Arrived\r\n");
}

void Car::planToCoordinate(float xTarget, float yTarget, float angleTarget){
	Coordinate current = Coordinate(x,y);
	Coordinate goal = Coordinate(xTarget,yTarget);
	while( distance2D(current,goal) > 5){

		checkForObstacle();
		set<Coordinate> plan = generateCoordinate(current,carAngle,30,10);		
		float tempDistance= 1000000000;

		Coordinate next = current;
		
		for(set<Coordinate>::iterator it=plan.begin(); it!=plan.end();){
			bool isCollision = false;
			for(Coordinate obs : obstacleList){
				if(distance2D(*it,obs) < SAFE_DISTANCE){
					printf("Collision detect. Positon: %d %d .Distance: %.2f\r\n",it->x, it->y, distance2D(*it,obs));
					isCollision = true;
					break;
				}
				
			}
			if(!isCollision){
				if(tempDistance > distance2D(*it,goal)){
					tempDistance = distance2D(*it,goal);
					next = Coordinate(it->x,it->y);
				}
				++it;
			}
			else it=plan.erase(it);
		}
		for(Coordinate obs: obstacleList) printf("Obs Position: x: %d y: %d\r\n",obs.x,obs.y);
		for(Coordinate coord : plan){
			printf("Plan Position: x: %d y: %d\r\n",coord.x,coord.y);
		}
		
		
		if(plan.size() > 0) {
			printf("Chosen Position: x: %d y: %d\r\n",next.x,next.y);
			goToCoordinate3(next.x,next.y);
		}
		else {
			drive(STOP);
			printf("Fail to choose position \r\n");
		}
		current = Coordinate(x,y);
		//obstacleList.clear();
	}
	drive(STOP);
	delay(100);
	if(angleTarget != -999){
		staticTurn(angleTarget);
	}
	drive(STOP);
	delay(2000);
}

void Car::planToCoordinate2(float xTarget, float yTarget, float angleTarget){
	toX = xTarget;
	toY = yTarget;
	drive(STOP);
	delay(100);
	float dx = 0;
	float dy = 0;
	float distanceLeft = 0;
	float biasAngle = 0;
	int original = driveSpeed;
	int slow = int(0.8*driveSpeed);
	
	dx = xTarget - x;
	dy = yTarget - y;
	distanceLeft = sqrtf(dx*dx + dy*dy);
	targetAngle = atan2f(dy,dx)*180.0/M_PI;
	if(absolute(angleDif(targetAngle,carAngle)>30)){
		staticTurn(targetAngle);
	}
	driveSpeed = slow;
	
	while( distanceLeft > 2.5){
		if(absolute(angleDif(targetAngle,carAngle)> 29)){
		staticTurn(targetAngle);
		}
		PID_Straight_Gyro();
		delay(100);
		
		update_ultra();
		
		if(Ultra_reading[2] < 25 || Ultra_reading[1] < 25){
			biasAngle += 30;
		} 
		else if (biasAngle > 0){
			if(Ultra_reading[2] > 25 && Ultra_reading[0] > 25 && Ultra_reading[1] > 25 && biasAngle > 0){
				biasAngle -= 15;
			}
		}
		
		
		dx = xTarget - x;
		dy = yTarget - y;
		distanceLeft = sqrtf(dx*dx + dy*dy);
		targetAngle = constrainAngle(atan2f(dy,dx)*180.0/M_PI + biasAngle);
		
		//printf("Target angle: %.2f, Bias Angle: %.2f\r\n", targetAngle, biasAngle);
		//printf("Ultra reading: %.2f %.2f %.2f %.2f %.2f\r\n", Ultra_reading[0],Ultra_reading[1],Ultra_reading[2],Ultra_reading[3],Ultra_reading[4]);

	}
	drive(STOP);
	delay(100);
	if(angleTarget != -999){
		staticTurn(angleTarget);
	}
	drive(STOP);
	delay(100);
	printf("Arrived\r\n");
	driveSpeed = original;
}


