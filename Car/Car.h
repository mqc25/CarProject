#ifndef CAR_H
#define CAR_H

#include <string>
#include "../Encoder/Encoder.h"
#include "../Sensor/MPU9250.h"
#include "../Sensor/ultra.h"
#include "../Util/Util.h"
#include <fstream>
#include <set>

#define RESOLUTION 4000
#define CYCLE 300

extern std::ofstream file;

enum Direction{
	FORWARD = 0,
	RIGHT,
	BACKWARD,
	LEFT,
	STOP
};

class Car{
	public:
		int driveSpeed;
		int diff;
		Encoder *carEncoder;
		
		std::set<Coordinate> obstacleList;
		
		float x;
		float y;
		float carAngle;
		float targetAngle;
		
		float Kps;
		float Kds;

		float Kpt;
		float Kdt;

		float lastErrorS;
		float lastErrorT;

		long timerState;
		long timerPID;

		volatile bool newCommand;

		Car();
		Car(float);
		void drive(Direction);
		void updateState();
		void checkForObstacle();
		void PID_Straight_Gyro();
		void turnToAngle(float);
		void staticTurn(float);
		void goForwardDistance(float distance);
		void executeCommand(std::string command);
		
		void goToCoordinate1(float, float, float angleTarget = -999);
		void goToCoordinate2(float, float, float angleTarget = -999);
		void goToCoordinate3(float, float, float angleTarget = -999);
		void planToCoordinate(float,float, float angleTarget = -999);
		void planToCoordinate2(float,float, float angleTarget = -999);

	
};

#endif
