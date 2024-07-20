#include "pca9685.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <wiringPi.h>
#include <stdio.h>
#include "robot_common.h"
#include "robot_comm.h"
#include "robot_motion.h"
#include "robot_memory_mgt.h"


#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50
const int in0 = 0; // pin 11 for steering
const int in1 = 1; // pin 12 for steering

const int in2 = 2; // Enable speed B  pin 13
const int in3 = 3; // pin 15 for wheels
const int in4 = 4; // pin 16 for wheels
const int in5 = 5; // Enable A speed control pin 5
const int in6 = 5; // Enable B speed control pin 5
int v_speed =  INIT_SPEED;

float lmillis [4] = {1.4,1.4,1.4,1.4};

int front_camera = 1;



unsigned int lastTime = 0;
unsigned short lastSpeed = 0;
unsigned short lastAction = 0;
unsigned short room_num = 0;
char start_record = false;

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

int motor_init (){

	wiringPiSetup();

        pinMode (in0, OUTPUT);
        pinMode (in1, OUTPUT);
        pinMode (in2, OUTPUT);
        pinMode (in3, OUTPUT);
        pinMode (in4, OUTPUT);
	softPwmCreate(in2,1,255);
        lastTime = millis();


	printf("motor_init:PCA9685 servo example\n");
        printf("motor_init:Connect a servo to any pin. It will rotate to random angles\n");

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
        return (0);
}

void start_recording (int room_no){
        open_memory_file_wr (room_no);
	start_record = true;
}

void stop_recording (){
        stop ();
        lastTime = 0;
        lastSpeed = 0;
	lastAction = 0;
	room_num = 0;
	start_record = false;
	close_memory_file ();

}

void remember_action (unsigned short action, unsigned short speed,unsigned char calculate_duration){
	MemoryCellType cell;
	printf ("remember_action: enter to function \n");
	if (!start_record)
		return;
	printf ("remember_action: do function\n");
	if (lastAction !=0){
		cell.speed = lastSpeed;
		cell.actionType =lastAction;
		cell.actionDuration = 0;   // mlisecond

		printf ("remember_action: currentaction %x  speed %d \n",action,speed);
		if (calculate_duration){
			cell.actionDuration = millis() - lastTime;   // mlisecond
			printf ("remember_action: lastaction %x current time %d lasttime %d duraton %d\n",
					                   lastAction,millis(), lastTime,cell.actionDuration);
		}
		write_cell (&cell);
	}
	lastTime = millis();
	lastSpeed = speed;
	lastAction = action;
        printf ("remember_action: current action %x current time %d \n",action,millis());

}

void go_forward (int speed){
	printf ("SPEED in FORWD %d \n",speed);
	ctr_speed (speed);
  	digitalWrite(in3, HIGH);
    	digitalWrite(in4, LOW);
	remember_action (MV_FRWD,speed,true);

}

void go_back (int speed){
	ctr_speed (speed);
  	digitalWrite(in3, LOW);
    	digitalWrite(in4, HIGH);
	remember_action (MV_BKWD,speed,true);
 }


void stop (){
	printf ("stop called\n");
  	digitalWrite(in3, LOW);
  	digitalWrite(in4, LOW);
	remember_action (STOP,0,true);
}

void go_left (int speed){
	ctr_speed (ROTATE_SPEED);
	digitalWrite(in0, LOW);
 	digitalWrite(in2, HIGH);
	delay (ROTATE_DELAY);
	stop ();
	remember_action (MV_LEFT,speed,true);
    }

void go_right (int speed){
	ctr_speed (ROTATE_SPEED);
	digitalWrite(in0, HIGH);
 	digitalWrite(in2, LOW);
	delay (ROTATE_DELAY);
	stop ();
	remember_action (MV_RGHT,speed,true);
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



void f_camera_left ()
{
      	lmillis [1] += .1;
      	rotate_camera_lt (1,1);
     	printf ("front left millis %f \n",lmillis[1]);
	remember_action (F_CAMERA_LFT,0,true);


}

void b_camera_left ()
{
	lmillis [3] += .1;
	rotate_camera_lt (15,3);
        printf ("back left millis %f \n",lmillis[3]);
	remember_action (B_CAMERA_LFT,0,true);

}

void f_camera_right ()
{
      lmillis [1] -= .1;
      rotate_camera_lt (1,1);
       printf ("front right millis %f \n",lmillis[1]);
      remember_action (F_CAMERA_RGT,0,true);

}

void b_camera_right ()
{

        lmillis[3] -= .1;
        rotate_camera_gt (15,3);
        printf ("back right millis %f \n",lmillis[3]);
        remember_action (B_CAMERA_RGT,0,true);
}

void f_camera_up ()
{
      lmillis [0] += .1;
      rotate_camera_lt (0,0);
      printf ("front up  millis %f \n",lmillis[0]);
      remember_action (F_CAMERA_UP,0,true);

}

void b_camera_up ()
{
	lmillis [2] += .1;
	rotate_camera_lt (14,2);
        printf ("back up  millis %f \n",lmillis[2]);
        remember_action (B_CAMERA_UP,0,true);

}

void f_camera_dn ()
{
      lmillis [0] -= .1;
      rotate_camera_lt (0,0);
      printf ("front down  millis %f \n",lmillis[0]);
      remember_action (F_CAMERA_DN,0,true);

}

void b_camera_dn ()
{

        lmillis[2] -= .1;
        rotate_camera_gt (14,2);
        printf ("back down  millis %f \n",lmillis[2]);
        remember_action (B_CAMERA_DN,0,true);
}




void stop_motion ()
{
        printf ("stop motion\n");
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        remember_action (STOP_MOTION,0,true);
}

void ctr_speed (int speed){
	 softPwmWrite (in2, speed);
	if (lastAction != MV_LEFT && lastAction != MV_RGHT)
	    remember_action (lastAction,speed,true);
	else
	    remember_action (SPEED,speed,true);
}

void speed_up (){
        v_speed += 10;
 if (v_speed >   255)
         v_speed = 255;
 ctr_speed (v_speed);
}

void speed_down (){
        v_speed -= 10;
 if (v_speed <   1)
         v_speed = 1;
 ctr_speed (v_speed);
}
