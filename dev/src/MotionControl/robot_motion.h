#ifndef _ROBOT_MOTION_H_
#define _ROBOT_MOTION_H_
#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>
#include "pca9685.h"
#define ROTATE_DELAY  150
#define ROTATE_SPEED  500
#define INIT_SPEED  125
void go_forward (int speed);
void go_back (int speed);
void stop ();
void go_left (int speed);
void go_right (int speed);
int   motor_init ();
void ctr_speed (int speed);
void start_recording (int room_no);
void stop_recording ();
float rotate_camera_lt (int channel, int msindex);
float rotate_camera_gt (int channel,int msindex);
void f_camera_left ();
void f_camera_right ();
void f_camera_up ();
void f_camera_dn ();
void b_camera_left ();
void b_camera_right ();
void b_camera_up ();
void b_camera_dn ();
void stop_motion ();
#endif // _ROBOT_MOTION_H_

