
#include "robot_sensors.h"

void setup_dist_sensor(int trig,int echo) {
        wiringPiSetup();
        pinMode(trig, OUTPUT);
        pinMode(echo, INPUT);
 
        //TRIG pin must start LOW
        digitalWrite(trig, LOW);
        delay(30);
}
 
int get_dist_CM(int trig,int echo) {
        //Send trig pulse
        digitalWrite(trig, HIGH);
        delayMicroseconds(20);
        digitalWrite(trig, LOW);
 
        //Wait for echo start
        while(digitalRead(echo) == LOW);
 
        //Wait for echo end
	//printf (" mit for echoes\n");
        long startTime = micros();
        while(digitalRead(echo) == HIGH);
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        int distance = travelTime / 58;
 
        return distance;
}
 
