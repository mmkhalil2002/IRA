/*************************************************************************
 * servo.c
 *
 * PCA9685 servo example
 * Connect a servo to any pin. It will rotate to random angles.
 *
 *
 * This software is a devLib extension to wiringPi <http://wiringpi.com/>
 * and enables it to control the Adafruit PCA9685 16-Channel 12-bit
 * PWM/Servo Driver <http://www.adafruit.com/products/815> via I2C interface.
 *
 * Copyright (c) 2014 Reinhard Sprung
 *
 * If you have questions or improvements email me at
 * reinhard.sprung[at]gmail.com
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The given code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You can view the contents of the licence at <http://www.gnu.org/licenses/>.
 **************************************************************************
 */

#include "pca9685.h"
#include <wiringPi.h>
#include <softPwm.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50
const int in0 = 0; // pin 11
const int in1 = 1; // pin 12

const int in2 = 2; // Enable speed B  pin 13
const int in3 = 3; // pin 15
const int in4 = 4; // pin 16
const int in6 = 5; // Enable B speed control pin 5
int speed =  100;

float lmillis [4] = {1.4,1.4,1.4,1.4};




/**
 * Calculate the number of ticks the signal should be high for the required amount of time
 */
int calcTicks(float impulseMs, int hertz)
{
	float cycleMs = 1000.0f / hertz;
	return (int)(MAX_PWM * impulseMs / cycleMs + 0.5f);
}

/**
 * input is [0..1]
 * output is [min..max]
 */
float map(float input, float min, float max)
{
	return (input * max) + (1 - input) * min;
}


float rotate_camera_lt (int channel, int msindex)
{
	int tick;
	if (lmillis[msindex] <= 2.4)
	{
   		tick = calcTicks(lmillis[msindex], HERTZ);
		printf ("left millis %f \n",lmillis[msindex]);
       		pwmWrite(PIN_BASE + channel, tick);
   		delay(100);
	        return lmillis[msindex];
	}
	else
        	return (-1.0);
}

float rotate_camera_gt (int channel,int msindex)
{
	int tick;
	if (lmillis[msindex] >= .1)
	{
   		tick = calcTicks(lmillis[msindex], HERTZ);
		printf ("right millis %f \n",lmillis[msindex]);
       		pwmWrite(PIN_BASE + channel, tick);
   		delay(100);
		return lmillis[msindex];
	}
	else
	{
		printf ("right  exit %f \n",lmillis[msindex]);
        	return (-1.0);
	}
}


void stop (){
        printf ("stop called\n");
        digitalWrite(in0, LOW);
        digitalWrite(in1, LOW);
}

void turn_left (void){
        printf ("turn left \n");
        digitalWrite(in0, LOW);
        digitalWrite(in1, HIGH);
	delay (250);
	stop ();

}

void turn_right (void){
        printf ("turn right \n");
        digitalWrite(in0, HIGH);
        digitalWrite(in1, LOW);
	delay (250);
	stop ();

}


void ctr_speed (int speed){
        softPwmWrite (in2, speed);
}



void speed_up (){
	speed += 10;
 if (speed >   255)
	 speed = 255;
 ctr_speed (speed);
}

void speed_down (){
	speed -= 10;
 if (speed <   1)
	 speed = 1;
 ctr_speed (speed);
}


void stop_motion ()
{
        printf ("stop motion\n");
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
}

void go_forward (int speed){
        printf ("SPEED in FORWD %d \n",speed);
        ctr_speed (speed);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
	delay (250);
	//stop_motion ();


}


void go_back (int speed){
        ctr_speed (speed);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
	delay (250);
	//stop_motion ();
 }






int main(void)
{
	printf("PCA9685 servo example\n");
	printf("Connect a servo to any pin. It will rotate to random angles\n");

	// Calling wiringPi setup first.
	wiringPiSetup();

	  pinMode (in0, OUTPUT);
          pinMode (in1, OUTPUT);
          pinMode (in2, OUTPUT);
          pinMode (in3, OUTPUT);
          pinMode (in4, OUTPUT);
          softPwmCreate(in2,1,255);
          //softPwmCreate(in5,1,255);



	// Setup with pinbase 300 and i2c location 0x40
	int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
	if (fd < 0)
	{
		printf("Error in setup\n");
		return fd;
	}

	// Reset all output
	pca9685PWMReset(fd);


	// Set servo to neutral position at 1.5 milliseconds
	// (View http://en.wikipedia.org/wiki/Servo_control#Pulse_duration)
	int tick = calcTicks(lmillis[3], HERTZ);
	pwmWrite(PIN_BASE + 15, tick);
	tick = calcTicks(lmillis[2], HERTZ);
	pwmWrite(PIN_BASE + 14, tick);
	tick = calcTicks(lmillis[0], HERTZ);
	pwmWrite(PIN_BASE + 0, tick);
	tick = calcTicks(lmillis[1], HERTZ);
	pwmWrite(PIN_BASE + 1, tick);
	delay(9000);


	int active = 1;
	//lmillis = 2.0;
	 system("stty raw");
	while (active)
	{
		// That's a hack. We need a random number < 1
	        	char c = getchar ();
			if  (c == '.') {
           			 system("stty cooked");
            			exit(0);
        		}
                        printf ("  char  %c is pressed \n",c ); 
			if (c == 'u' )
			{
				lmillis [0] += .1;
				rotate_camera_lt (0,0);
         			printf ("up  millis %f \n",lmillis[0]);
		 	}
			if (c == 'd' )
			{
				lmillis[0] -= .1;
				rotate_camera_gt (0,0);
         			printf ("down  millis %f \n",lmillis[0]);
		 	}
			if (c == 'l' )
			{
				lmillis [1] += .1;
				rotate_camera_lt (1,1);
         			printf ("left millis %f \n",lmillis[1]);
		 	}
			if (c == 'r' )
			{
				lmillis[1] -= .1;
				rotate_camera_gt (1,1);
         			printf ("right  millis %f \n",lmillis[1]);
		 	}


			if (c == 'R' )
			{
				printf ("turn right \n");
				turn_right ();
		 	}

			if (c == 'L' )
			{
				printf ("turn left \n");
				turn_left ();
			}
			if (c == 'b' )
			{
				printf ("go back \n");
				go_back (speed);
			}
			if (c == 'f' )
			{
				printf ("go forward \n");
				go_forward (speed);
			}
			if (c == 'K' )
			{
				printf ("stop rotate \n");
				stop ();
			}
			if (c == 'k' )
			{
				printf ("stop motion\n");
				stop_motion ();
			}
			if (c == 's' )
			{
				printf ("speed down\n");
				speed_down ();
			}
			if (c == 'S' )
			{
				printf ("speed up\n");
				speed_up ();
			}
			 else 
			{
				printf ("undefined \n");
		 	}
	}

	return 0;
}
