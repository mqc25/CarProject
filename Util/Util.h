#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <set>
#include <sstream>
#include <vector>

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>

using namespace std;
extern char buffer[1024];
extern volatile bool newSignal;
extern int conn;

typedef struct Coordinate{
	int x;
	int y;
	
	Coordinate(int a, int b){
		x = a;
		y = b;
	} 
	
	Coordinate(float a, float b){
		//x = int((a + 1)/2) * 2;
		//y = int((b + 1)/2) * 2;
		x = int(a);
		y = int(b);
	}
	
	bool operator<( const Coordinate& other ) const
    {
        if(x == other.x) return y < other.y;
        else return x < other.x;
    }
} Coordinate;


float absolute(float);
float angleDif(float, float);
std::vector<std::string> split(const std::string &s, char delim);
float toRad(float);
float distance2D(Coordinate, Coordinate);
float realDistance2D(Coordinate,Coordinate);
float constrainAngle(float x);
std::set<Coordinate> generateCoordinate(Coordinate currentPos, float currentHeading, float step, float distance);

void setupSocket(int);
void sendMsg(char* msg);
float* generateAngle(float curAngle);

#endif
