//gpsDecoder.h
//decode GPS NEMA data
//This decoder is very lightweight and only parses data from GPRMC packets
//this offers the minimum ammount of data to be parsed and gets you everything but altitude. 
//if you need altitude feel free to modify the parseGPS routine and add an alternate IF statment for GPGGA Packets

//written by: Bill Heaster
//1/20/16
//This shit is free, do what you want with it, just give me some credit. 
//theCreator a.t ApexLogic d0t Net
/////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <math.h> //for harversine sin, cos, acos, etc. 
#include <stdlib.h>
#include <string.h>



struct Packet
{
	
	//decimal degree format for use in mapping software. ie. google maps 
	float lat;
	float lon;
	
	//latitude and longitude
	int latDegree;
	double latMinSec;
	
	int lonDegree;
	double lonMinSec;
	
	//time
	int timeHours;
	int timeMinutes;
	int timeSeconds;
	
	//date
	int day;
	int month;
	int year;
	
	//hemisphere
	char latHem;
	char lonHem;
	
	//altitude
	int alt;
	
	//number of fixed satalites
	int numSats;
	
	//movement

	float heading;
	char headingDirection[3];
	
	float speedKnots;
	float speedFPS;
	float speedMPH;
	
	int isValid;
	int isLatNeg;
	int isLonNeg;
	
};

struct Position
{
	long double lat;
	long double lon;	
	long double  latRad;
	long double lonRad;
};

int parseGPS(char * input, struct Packet * packet);
void setHeading(struct Packet * loc);
long double getDistanceKM(struct Position * point1, struct Position * point2);
long double getDistanceMiles(struct Position * point1, struct Position * point2);
long double getDistanceFeet(struct Position * point1, struct Position * point2);
long double dist_km(struct Position * point1, struct Position * point2);
long double dist_miles(struct Position * point1, struct Position * point2);
long double dist_feet(struct Position * point1, struct Position * point2);
long double distance_km(struct Position * point1, struct Position * point2);
long double distance_miles(struct Position * point1, struct Position * point2);
long double distance_feet(struct Position * point1, struct Position * point2);
void setPosition(struct Position *p, long double lat, long double lon);
void clearPacket(struct Packet * loc);
