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

#include <stdio.h>
#include <stdlib.h>

#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50
#define front_v_camera    15
#define front_h_camera   15 
#define back_v_camera    15
#define back_h_camera    15



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


float rotate_camera_left (float millis)
{
	int tick;
	if (millis <= 2.0)
	{
   		tick = calcTicks(millis, HERTZ);
		printf ("left millis %f \n",millis);
       		pwmWrite(PIN_BASE + front_h_camera, tick);
   		delay(100);
	        return millis;
	}
	else
        	return (-1.0);
}

float rotate_camera_right (float millis)
{
	int tick;
	if (millis >= .1)
	{
   		tick = calcTicks(millis, HERTZ);
		printf ("right millis %f \n",millis);
       		pwmWrite(PIN_BASE + front_h_camera, tick);
   		delay(100);
		return millis;
	}
	else
	{
		printf ("right  exit %f \n",millis);
        	return (-1.0);
	}
}


int main(void)
{
	int flag  =0;
	printf("PCA9685 servo example\n");
	printf("Connect a servo to any pin. It will rotate to random angles\n");

	// Calling wiringPi setup first.
	wiringPiSetup();

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
	float hmillis = 1.1;
	int tick = calcTicks(hmillis, HERTZ);
	pwmWrite(PIN_BASE + 15, tick);
	pwmWrite(PIN_BASE + 14, tick);
	pwmWrite(PIN_BASE + 0, tick);
	delay(9000);


	int active = 1;
	//millis = 2.0;
	while (active)
	{
		// That's a hack. We need a random number < 1
         	printf ("initial  millis %f \n",hmillis);
                 if (flag == 0)
		 {
			for (;;hmillis += .1)
			{
				int c = getchar ();
				printf ("left  get char \n");
				int r  = rotate_camera_left (hmillis);
				if (r < 0)
				{
					flag = 1;
					break;
				}
		 	}
		 }
		 else 
		 {
			for (;;hmillis -= .1)
			{
		                int c = getchar ();
				printf ("right get char \n");
				int r = rotate_camera_right (hmillis);
				if (r < 0)
				{
					flag = 0;
					break;
				}
		 	}
		 }
	}

	return 0;
}
