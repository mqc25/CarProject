#include "Util.h"
#include <math.h>


float absolute(float a)
{
    return a > 0 ? a : -a;
}

float constrainAngle(float x){
	x += (x > 180) ? -360 : (x < -180) ? 360 : 0;
    return x;
}

float angleDif(float target, float source){
    float diff = target - source;
    diff += (diff>180) ? -360 : (diff<-180) ? 360 : 0;
    return diff;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

float toRad(float degree){
	return degree*M_PI/180.0;
}

float distance2D(Coordinate a, Coordinate b){
	return sqrtf( float(pow(a.x - b.x,2) + pow(a.y - b.y,2)));
}

std::set<Coordinate> generateCoordinate(Coordinate currentPos, float currentHeading, float step, float distance){
	std::vector<float> listOfAngle;
	float startAngle = constrainAngle(currentHeading - 90);
	int num = 180/step;
	for(int i=0; i<num; i++){
		listOfAngle.push_back(startAngle);
		startAngle = constrainAngle(startAngle + step);
	}
	std::set<Coordinate> listOfCoordinate;
	for(float one_angle : listOfAngle){
		Coordinate temp = Coordinate(currentPos.x + distance*cos(toRad(one_angle)), currentPos.y + distance*sin(toRad(one_angle)));
		listOfCoordinate.insert(temp);
	}
	return listOfCoordinate;
}
