#include <wiringPi.h>
#include <stdio.h>

const int in0 = 0; // pin 11
const int in2 = 2; // pin 13
const int in3 = 3; // pin 15
const int in4 = 4; // pin 16

void go_forward (){
	digitalWrite(in0, HIGH);
 	digitalWrite(in2, LOW);
  	digitalWrite(in3, HIGH);
    	digitalWrite(in4, LOW);

}

void go_back (){
	digitalWrite(in0, LOW);
 	digitalWrite(in2, HIGH);
  	digitalWrite(in3, LOW);
    	digitalWrite(in4, HIGH);
 }

void stop (){
  	digitalWrite(in0, LOW);
  	digitalWrite(in2, LOW);
  	digitalWrite(in3, LOW);
  	digitalWrite(in4, LOW);
}

void go_left (){

	digitalWrite(in0, HIGH);
 	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
    }

void go_right (){
	digitalWrite(in0, LOW);
 	digitalWrite(in2, HIGH);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);

}

int main(void)//putting void in the parameter list of a function is not how you define the return type of a function. proper syntax is return_type function_name (parameter list) {}
{

	wiringPiSetup();

        pinMode (in0, OUTPUT);
        pinMode (in2, OUTPUT);
        pinMode (in3, OUTPUT);
        pinMode (in4, OUTPUT);

	int angle_steps = (float)((5.0/90.0) * (200.0));
	stop ();
	for (int i=0;i<100;i++){
		//go_left ();
		delay (1);
	}

	return 0;
}
