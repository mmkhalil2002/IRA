#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
 
#define F_TRIG 10 // Green front
#define F_ECHO 11 //Yellow front

#define SL_TRIG 28 // Green  side
#define SL_ECHO 29 //Yellow side

#define B_TRIG 26 // Green  back
#define B_ECHO 27 //Yellow  back

#define SR_TRIG 24 // Green  R side
#define SR_ECHO 25 //Yellow  R side
 
void setup(int trig,int echo) {
        wiringPiSetup();
        pinMode(trig, OUTPUT);
        pinMode(echo, INPUT);
 
        //TRIG pin must start LOW
        digitalWrite(trig, LOW);
        delay(30);
}
 
int getCM(int trig,int echo) {
        //Send trig pulse
	printf ("start mes\n");
        digitalWrite(trig, HIGH);
        delayMicroseconds(20);
        digitalWrite(trig, LOW);
 
        //Wait for echo start
        while(digitalRead(echo) == LOW);
 
        //Wait for echo end
	printf (" mit for echoes\n");
        long startTime = micros();
        while(digitalRead(echo) == HIGH);
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        int distance = travelTime / 58;
 
        return distance;
}
 
int main(void) {
    setup(F_TRIG,F_ECHO);
    setup(B_TRIG,B_ECHO);
    setup(SL_TRIG,SL_ECHO);
    setup(SR_TRIG,SR_ECHO);
    while (1)
    {
        int dist=getCM(F_TRIG,F_ECHO);
        printf("Front Distance: %dcm\n", dist);
        delay(10);
        dist=getCM(B_TRIG,B_ECHO);
        printf("Back Distance: %dcm\n", dist);
        delay(10);
        dist=getCM(SL_TRIG,SL_ECHO);
        printf("Side Left Distance: %dcm\n", dist);
        delay(10);
        dist=getCM(SR_TRIG,SR_ECHO);
        printf("Side Left Distance: %dcm\n", dist);
        delay(10);
    }
}
