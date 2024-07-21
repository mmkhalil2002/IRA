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

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::                                                                         :*/
/*::  This routine calculates the distance between two points (given the     :*/
/*::  latitude/longitude of those points). It is being used to calculate     :*/
/*::  the distance between two locations using GeoDataSource(TM) products.   :*/
/*::                                                                         :*/
/*::  Definitions:                                                           :*/
/*::    South latitudes are negative, east longitudes are positive           :*/
/*::                                                                         :*/
/*::  Passed to function:                                                    :*/
/*::    lat1, lon1 = Latitude and Longitude of point 1 (in decimal degrees)  :*/
/*::    lat2, lon2 = Latitude and Longitude of point 2 (in decimal degrees)  :*/
/*::    unit = the unit you desire for results                               :*/
/*::           where: 'M' is statute miles (default)                         :*/
/*::                  'K' is kilometers                                      :*/
/*::                  'N' is nautical miles                                  :*/
/*::  Worldwide cities and other features databases with latitude longitude  :*/
/*::  are available at https://www.geodatasource.com                         :*/
/*::                                                                         :*/
/*::  For enquiries, please contact sales@geodatasource.com                  :*/
/*::                                                                         :*/
/*::  Official Web site: https://www.geodatasource.com                       :*/
/*::                                                                         :*/
/*::           GeoDataSource.com (C) All Rights Reserved 2018                :*/
/*::                                                                         :*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#include <stdio.h>
#include <math.h> //for harversine sin, cos, acos, etc. 
#include <stdlib.h>
#include <string.h>
#include "gps.h"

#define pi 3.14159265358979323846

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  Function prototypes                                           :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
long double deg2rad(long double);
long double rad2deg(long double);

///  first method not accurate

long double distance(long double lat1, long double lon1, long double lat2, long double lon2, char unit) {
  long double theta, dist;
  if ((lat1 == lat2) && (lon1 == lon2)) {
    return 0;
  }
  else {
    theta = lon1 - lon2;
    dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
    dist = acos(dist);
    dist = rad2deg(dist);
    dist = dist * 60 * 1.1515;
    switch(unit) {
      case 'M':
        break;
      case 'K':
        dist = dist * 1.609344;
        break;
      case 'N':
        dist = dist * 0.8684;
        break;
      case 'F':
        dist = dist * 5280.0;
        break;
    }
    return (dist);
  }
}

long double distance_p(struct Position *point1, struct Position *point2, char unit) {
 return (distance(point1->lat, point1->lon, point2->lat, point2->lon, unit));
}

long double dist_miles(struct Position *point1, struct Position *point2) {
 return (distance(point1->lat, point1->lon, point2->lat, point2->lon, 'M'));
}


long double dist_km(struct Position *point1, struct Position *point2) {
 return (distance(point1->lat, point1->lon, point2->lat, point2->lon, 'K'));
}

long double dist_feet(struct Position *point1, struct Position *point2) {
 return (distance(point1->lat, point1->lon, point2->lat, point2->lon, 'F'));
}
//
/// second method
//
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
long double deg2rad(long double deg) {
  return (deg * pi / 180);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts radians to decimal degrees             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
long double rad2deg(long double rad) {
  return (rad * 180 / pi);
}



// Utility function for
// converting degrees to radians
long double toRadians(const long double degree)
{
    // cmath library in C++
    // defines the constant
    // M_PI as the value of
    // pi accurate to 1e-30
    long double one_deg = (M_PI) / 180;
    return (one_deg * degree);
}

// miles
long double distance_miles(struct Position *point1, struct Position *point2) {
    // Convert the latitudes
    // and longitudes
    // from degree to radians.
    long double lat1 = toRadians(point1->lat);
    long double long1 = toRadians(point1->lon);
    long double lat2 = toRadians(point2->lat);
    long double long2 = toRadians(point2->lon);

    // Haversine Formula
    long double dlong = long2 - long1;
    long double dlat = lat2 - lat1;

    long double ans = pow(sin(dlat / 2), 2) +
                          cos(lat1) * cos(lat2) *
                          pow(sin(dlong / 2), 2);

    ans = 2 * asin(sqrt(ans));

    // Radius of Earth in
    // Kilometers, R = 6371
    // Use R = 3956 for miles
    long double R = 3956;

    // Calculate the result
    ans = ans * R;

    return ans;
}

long double distance_km(struct Position *point1, struct Position *point2) {
   long double d = distance_miles(point1,point2);
   d = 1.609344 * d ;//miles
   return d;

}

long double distance_feet(struct Position *point1, struct Position *point2) {
   long double d = distance_miles(point1,point2);
   d = 5280.0 * d ;//feet
   return d;

}


///
//third method
//
long double getDistance(struct Position * point1, struct Position * point2)
{
	
	//sanity check
	if(point1->lat<0)
		point1->lat*=-1;
	if(point1->lon<0)
		point1->lon*=-1;
	if(point2->lat<0)
		point2->lat*=-1;
	if(point2->lon<0)
		point2->lon*=-1;

	
	
	//convert decimal degrees to radians
	point1->latRad = point1->lat * 0.017453293;
	point1->lonRad = point1->lon * 0.017453293;
	point2->latRad = point2->lat * 0.017453293;
	point2->lonRad = point2->lon * 0.017453293;
	
	//find the difference values between lat and lon radians
	long double diffLat, diffLon = 0;
	
	if(point1->latRad > point2->latRad)
	{
		diffLat = point1->latRad - point2->latRad;
	}
	else
	{
		diffLat = point2->latRad - point1->latRad;
	}
	if(point1->lonRad > point2->lonRad)
	{
		diffLon = point1->lonRad - point2 ->lonRad;
	}
	else
	{
		diffLon = point2 ->lonRad - point1->lonRad;
	}
	
	

	
	long double  latSin = sin(diffLat/2);
	long double lonSin = sin(diffLon/2);
	
	long double distance = 2* asin(sqrt((latSin*latSin)+cos(point1->latRad) *cos(point2->latRad) * (lonSin *lonSin)));
	
		
	return distance;
	
}


long double getDistanceKM(struct Position * point1, struct Position * point2)
{
	
	long double distance = getDistance (point1,point2);
	distance *= distance*6371;
	
	return distance;
	
}


long double getDistanceMiles(struct Position * point1, struct Position * point2)
{
	
	long double distance = getDistanceKM (point1,point2);
	distance*= 0.621371192;
	return distance;
	
}

long double getDistanceFeet(struct Position * point1, struct Position * point2)
{
	
	long double distance = getDistanceMiles (point1,point2);
	distance *= 5282;
		
	return distance;
	
}

void clearPacket(struct Packet * loc)
{
	loc->latDegree = 0;
	loc->lonDegree =0;
	loc->latMinSec =0;
	loc->lonMinSec =0;
	loc->timeHours =0;
	loc->timeMinutes =0;
	loc->timeSeconds =0;
	loc->day =0;
	loc->month=0;
	loc->year=0;
	loc->latHem='\0';
	loc->lonHem='\0';
	loc->alt=0;
	loc->numSats=0;
	loc->heading=0;
	loc->speedFPS =0;
	loc->speedKnots =0;
	loc->speedMPH =0;
	loc->isValid=0;
	loc->isLatNeg = 0;
	loc->isLonNeg =0;
	bzero(loc->headingDirection, sizeof(loc->headingDirection));

	return;
}

//used to set the positions within the position structure. this must be done when using the distance measuring routines
void setPosition(struct Position *p, long double lat, long double lon)
{
	p->lat = lat;
	p->lon = lon;
	
	return;
}

long double convertToDD(int degree, long double minSec)
{
	long double a = 0.00000;
	
	long double temp = 0.00000;
	
	temp = minSec/60;
	
	a = degree + temp;
	
	
	
	return a;
	
}

//gps sends us data in knots, convert it
long double knotsToMPH(long double knots)
{
	
	return knots * 1.15077945;
	
	
	
}

//more percision if your module provides it
long double mphToFPS(long double mph)
{
	
	return mph * 1.46667;
	
}


//used for the heading when the compass goes from 0 to 360
int checkbounds(int num)
{
	if(num>360)
	{
		return num -=360;
	}
	
	if(num < 0)
	{
		return num +=360;
	}
	
	
	return num;
}

//adds threshold checking and turns the heading degrees into N, E, S , W, NE, NW, SE, SW headings
void setHeading(struct Packet * loc)
{
	int th= 15; //threshold in degrees
	int in = (int)loc->heading;	

	
	int north = 0;
	int east = 90;
	int south = 180;
	int west = 270;
	
	int nl = checkbounds(north-th);
	int nh = checkbounds(north+th);
	
	int el = checkbounds(east -th);
	int eh = checkbounds(east +th);
	
	int sl = checkbounds(south-th);
	int sh = checkbounds(south +th);
	
	int wl = checkbounds(west-th);
	int wh = checkbounds(west+th);
	
	if((in >=nl && in <=360)||(in>=0 && in<=nh))
	{
		loc->headingDirection[0] ='N';
		return;
	}
	if(in >=el && in <=eh)
	{
		loc->headingDirection[0] ='E';
		return;
	}
	if(in >=wl && in <=wh)
	{
		loc->headingDirection[0] ='W';
		return;
	}
	if(in >=sl && in <=sh)
	{
		loc->headingDirection[0] ='S';
		return;
	}
	
	if(in<el && in>nh)
	{
		loc->headingDirection[0] ='N';
		loc->headingDirection[1] ='E';
	}
	if(in>eh && in<sl)
	{
		loc->headingDirection[0] ='S';
		loc->headingDirection[1] ='E';
	}
	if(in>sh && in<wl)
	{
		loc->headingDirection[0] ='S';
		loc->headingDirection[1] ='W';
		return;
	}
	if(in>wh && in<nl)
	{
		loc->headingDirection[0] ='N';
		loc->headingDirection[1] ='W';
		return;
	}
	else
	loc->headingDirection[0] = 'u';
	
	return;
		
}

void printData(struct Packet * loc)
{

	
	printf("\n-------------------------------------------------------\n");
	printf("time:%d:%d:%d\t", loc->timeHours, loc->timeMinutes, loc->timeSeconds);
	printf("date:%d/%d/%d\n", loc->month, loc->day, loc->year);
	printf("latitude: %f %c\t",loc->lat, loc->latHem);
	printf("longitude: %f %c\n", loc->lon, loc->lonHem);
	
	
	
	loc->speedMPH= knotsToMPH(loc->speedKnots);
	loc->speedFPS = mphToFPS(loc->speedMPH);
	
	setHeading(loc);
	
	printf("knots:%f\tMPH:%f\tFPS:%f\n", loc->speedKnots, loc->speedMPH, loc->speedFPS);
	printf("heading:%f\t\tDirection:%s",loc->heading,loc->headingDirection);
	printf("\n-------------------------------------------------------\n");
	
	return;
}

//
//$GPRMC,225032.00,A,3300.095424,N,09636.698196,W,0.0,295.1,040820,3.5,E,A*29index 422
//


int parseGPS(char * input, struct Packet * packet)
{
	
	char* currentLocation = input;
	
	char temp[128];
	bzero(temp, sizeof(temp));
	
        //printf ("\nBUFFER (%s) \n",input);	
	//check the string to see what kind of GPS packet it is. 
	//This module can only receive GPGSA, GPGGA, GPMRC 
	//All the data needed to fill the struct can be captured from the GPMRC packets
	//so we will process these, and drop all others. 
	//$GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
	
	
	char * needle = "$GPRMC";
	int len = strlen(needle);
	
	
	//if we have a valid GPMRC start code
	//printf ("before  gps in parseGPS input [%s]   type [%s]\n",input,needle );
	if(strstr(input, needle)!=NULL)
	{
		//put us on the first hour char
		currentLocation += (len+1);
		
		//next two are hours, place it in the struct
		temp[0] = *currentLocation++;
		temp[1] = *currentLocation++;
		
		packet->timeHours = atoi(temp);
		
		//get the minutes
		temp[0] = *currentLocation++;
		temp[1] = *currentLocation++;
		
		packet->timeMinutes = atoi(temp);
		
		//get seconds
		temp[0] = *currentLocation++;
		temp[1] = *currentLocation++;
				
		
		packet->timeSeconds = atoi(temp);
		bzero(temp, sizeof(temp));
		
		//current location on decimal point increment +5
		currentLocation+=4;

		printf ("\nCURRENT LOCATION %c \n",*currentLocation);
		
		//current location on packet status
		//if this isnt a valid packet clear the structure and return non-valid
		if(*currentLocation != 'A')
		{
			printf ("\nwrong packet \n");
			clearPacket(packet);
			return -1;
		}
		
		//increment +2 
		currentLocation +=2;
		
		//current location, first char of degree latitude
		temp[0] = *currentLocation++;
		temp[1] = *currentLocation++;
		
		packet->latDegree = atoi(temp);
		printf ("LATDEGREE %d \n",packet->latDegree);
		
		//current location is lat min first char
		temp[0] = *currentLocation++;
		temp[1] = *currentLocation++;
		temp[2] = *currentLocation++;
		temp[3] = *currentLocation++;
		temp[4] = *currentLocation++;
		temp[5] = *currentLocation++;
		temp[6] = *currentLocation++;
		temp[7] = *currentLocation++;
		
		packet->latMinSec = atof(temp);
		printf ("LATMINSEC %f \n",packet->latMinSec);
		bzero(temp, sizeof(temp));
		currentLocation++;
		
		//current location is N/S indicator
		currentLocation++;
		packet->latHem = *currentLocation++;
		currentLocation++;
		printf (" LAT DIRECTION %c \n",packet->latHem);
		
		//current location is first char degree lon
		printf ("\nLONGTUDE DETECTION %c \n",*currentLocation);
		if(*currentLocation == '0')
		{
			packet->isLonNeg = 1;
			currentLocation++;
			temp[0]=*currentLocation++;
			temp[1]=*currentLocation++;
			packet->lonDegree= atoi(temp);
		        printf ("LONDEGREE %d \n",packet->lonDegree);
			
		}
		else
		{
			temp[0] = *currentLocation++;	
			temp[1] = *currentLocation++;
			temp[2] = *currentLocation++;
			packet->lonDegree= atoi(temp);
		        printf ("LONDEGREE %d \n",packet->lonDegree);
		}
		

		
		//current location is first char of lon minutes
		
		temp[0] = *currentLocation++;
		temp[1] = *currentLocation++;
		temp[2] = *currentLocation++;
		temp[3] = *currentLocation++;
		temp[4] = *currentLocation++;
		temp[5] = *currentLocation++;
		temp[6] = *currentLocation++;
		
		
		packet->lonMinSec= atof(temp);
		currentLocation++;
	        printf ("LONGMINSEC  %f \n",packet->lonMinSec);
		
		//need to do this here i guess
		//convert coordinates into cartesian
		packet->lat = convertToDD(packet->latDegree, packet->latMinSec);
	
		packet->lon = convertToDD(packet->lonDegree, packet->lonMinSec);
		
		if(packet->isLonNeg)
		{
			packet->lon *= -1;
		}
	        printf ("LAT %f LON %f \n",packet->lat,packet->lon);	
		//current location is EW indicator
		currentLocation++;
		currentLocation++;
		packet->lonHem = *currentLocation++;
		printf (" LON DIRECTION %c\n",packet->lonHem);

	         	
		currentLocation++;
		for (int i=0;;i++){
			if (*currentLocation == ','){
		          temp[i] = '\0';
	                  break;
			}
		          temp[i] = *currentLocation++;
		          printf (" TEMP %c \n",temp[i]);
		}
		currentLocation ++;
		//current location is speed in knots
		packet->speedKnots= atof(temp);
		printf ("SPEED in NOTS %f \n",packet->speedKnots);
		bzero(temp, sizeof(temp));
		//current location is heading
		for (int i=0;;i++){
			if (*currentLocation == ','){
		          temp[i] = '\0';
	                  break;
			}
		          temp[i] = *currentLocation++;
		          printf (" TEMP %c \n",temp[i]);
		}
		currentLocation ++;
		packet->heading = atof(temp);
		//currentLocation++;
		bzero(temp, sizeof(temp));
		printf ("HEADING  %f \n",packet->heading);
		
		//current location is day of date
		temp[0] = *currentLocation++;
		temp[1] = *currentLocation++;
		
		packet->day = atoi(temp);
		
		//current location is month
		temp[0] = *currentLocation++;
		temp[1] = *currentLocation++;
		
		packet->month = atoi(temp);
		
		//current location is year		
		temp[0] = *currentLocation++;
		temp[1] = *currentLocation++;
		
		packet->year = atoi(temp);		
		printf ("date %d %d %d \n",packet->day,packet->month,packet->year);
		
		
	}	
	
	return 0;
	
}
