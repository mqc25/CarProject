#include "Util.h"
#include <math.h>
using namespace std;

float absolute(float a)
{
    return a > 0 ? a : -a;
}

double wrapMax(double x, double max)
{
    /* integer math: `(max + x % max) % max` */
    return fmod(max + fmod(x, max), max);
}

float constrainAngle(float x){
	
    return -180 + wrapMax(x - -180, 180 - -180);
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

char buffer[1024] = {0};
volatile bool newSignal = false;
int conn = -1;

void setupSocket( int port ){
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        exit(1);
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(1);
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        exit(1);
    }
	printf("\nConnection Success \n");
	conn = sock;
}  


void sendMsg(char* msg){
	send(conn , msg , strlen(msg) , 0 );
}

float* generateAngle(float curAngle){
	static float listAngle[5];
	listAngle[0] = curAngle -90;
	listAngle[1] = curAngle -35;
	listAngle[2] = curAngle -0;
	listAngle[3] = curAngle +35;
	listAngle[4] = curAngle +90;
	return listAngle;
	
}
