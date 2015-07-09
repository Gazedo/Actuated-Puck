#include <vector_math.h>
#include <CommunicationUtils.h>
#include <FreeIMU.h>
#include <calibration.h>

//FreeIMU stuff
#include <EEPROM.h>
#include <ADXL345.h>
#include <ITG3200.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <SPI.h>
#include <DebugUtils.h>
//PID Library
#include <PID_v1.h>
//AVR stuff for change interupts
#include <avr/io.h>
#include <avr/interrupt.h>
//FreeIMU
//IMU declarations
FreeIMU imu = FreeIMU();
float vals[6];
double alpha, omega, theta;
double alpha_old, omega_old, theta_old;
char dec, Select;
//PID stuff
int time = 0, oldtime = 0, dt = 0;
double Setpoint = 0.00, Input = 0, Output;
double Kp = 0.5, Kd = 0.00, Ki = 0.00;
PID pid(&Input, &Output, &Setpoint, Kp, Kd, Ki, DIRECT);
int hall = 0;
int spd, val; // -255 up to 255, negative signifies reverse

//PWM motor pins
int Motor1in = 21, Motor2in = 20, Motor3in = 19;
//Enable pins
int Motor1en = 10, Motor2en = 11, Motor3en = 12;//pin 10 actually actautes pin 23
//Hall interrupt and read pins
int Hall1read = 0, Hall2read = 1, Hall3read = 2;
void setup() {
//Serial communication with openlog
	Serial.begin(9600);
	Serial1.begin(9600);
	Wire.begin();
//Pin definations
	pinMode(Motor1in, OUTPUT);
	pinMode(Motor2in, OUTPUT);
	pinMode(Motor3in, OUTPUT);
	pinMode(Motor1en, OUTPUT);
	pinMode(Motor2en, OUTPUT);
	pinMode(Motor3en, OUTPUT);
//setup hall input
	DDRB |= 0x7;
	PORTB |= 0x7;
//Init the imu
	delay(100);
	imu.init();
//Set mode on pid library
	pid.SetMode(AUTOMATIC);
//Output limits for pwm eventually
	pid.SetOutputLimits(-255.00, 255.00);
//Wait for everything to settle
	delay(100);
}

void loop() {
//Compute at 500 Hz
	time = millis();
	if (time - oldtime >= 2) {
		imu.getValues(vals);
//Get current forces felt
		dt = time - oldtime;
//Get only the z rotation velocity
		omega = vals[5];
		theta += ((omega + omega_old) / 2) * (dt / 1000.0);
//Set up PID inputs for compute
		Input = theta;
		pid.Compute();
		spd = Output;
		omega_old = omega;
		oldtime = time;
//Record current time stamp in Milliseconds since start
		Serial1.print(time);Serial1.print('\t');
//Record all current values
		for (int i = 0; i < 6; i++) {
			Serial1.print(vals[i], 4);
			Serial1.print('\t');
		}
		Serial1.println("");
	}
//Recieve Instructions in the form of a single character and 3 digit number for now.
//Could upgrade when for rev 3
//Possible upgrades would include confirmation of the command and updates on current orientation.
	if ( Serial1.available() == 4 ) {
		Select = Serial1.read();
		dec = 0;
		for (int i = 100; i >= 1; i /= 10) {
			val = Serial1.read() - '0';
			dec += (int)val * i;
		}
	}
	else if (Serial1.available() < 4  || Serial1.available() > 4) {
		while (Serial1.available()) {
			int h = Serial1.read();
		}
	}

//Decide on instructions
	switch (Select) {
	case 's':
		Setpoint = dec;
		break;
	case 'p':
		Kp = dec / 100;
		break;
	case 'd':
		Kd = dec / 100;
		break;
	case 'i':
		Ki = dec / 100;
		break;
	}
//Read the status of the pin registers using C++ 
	PORTF &= 0xF8;
	PORTD &= 0x1F;
	PORTC = 0x0;
//Mask 
	hall = PINB & 0x7;
//Use the hall variable to decide on the commutation sequence.
//This sequence was taken from the Maxon manual.
	if ( spd > 0 ) { //Forward
		switch (hall) {
		case 5: // 101
			PORTF |= 0x1;  //00000001
			PORTD |= 0x60; //01100000
			/* equivalent to:
			digitalWrite(Motor1en, HIGH);
			digitalWrite(Motor2en, HIGH);
			digitalWrite(Motor1in, HIGH);
			*/
			break;
		case 4: // 100
			PORTF |= 0x1;  //00000001
			PORTD |= 0xA0; //10100000
			/*
			digitalWrite(Motor1en, HIGH);
			digitalWrite(Motor3en, HIGH);
			digitalWrite(Motor1in, HIGH);
			*/
			break;
		case 6: // 110
			PORTF |= 0x2;  //00000010
			PORTD |= 0xC0; //11000000
			/*
			digitalWrite(Motor2en, HIGH);
			digitalWrite(Motor3en, HIGH);
			digitalWrite(Motor2in, HIGH);
			*/
			break;
		case 2: // 010
			PORTF |= 0x2;  //00000010
			PORTD |= 0x60; //01100000
			/*
			digitalWrite(Motor1en, HIGH);
			digitalWrite(Motor2en, HIGH);
			digitalWrite(Motor2in, HIGH);
			*/
			break;
		case 3: // 011
			PORTF |= 0x4;  //00000100
			PORTD |= 0xA0; //10100000
			/*
			digitalWrite(Motor1en, HIGH);
			digitalWrite(Motor3en, HIGH);
			digitalWrite(Motor3in, HIGH);
			*/
			break;
		case 1: // 001
			PORTF |= 0x4;  //00000100
			PORTD |= 0xC0; //11000000
			/*
			digitalWrite(Motor2en, HIGH);
			digitalWrite(Motor3en, HIGH);
			digitalWrite(Motor3in, HIGH);
			*/
			break;
		}
	}
	else if ( spd < 0) { //Reverse
		switch (hall) {
		case 5: // 101
			PORTF |= 0x2;  //00000010
			PORTD |= 0x60; //01100000
			/*
			digitalWrite(Motor1en, HIGH);
			digitalWrite(Motor2en, HIGH);
			digitalWrite(Motor2in, HIGH);
			*/
			break;
		case 4: // 100 WRONG
			PORTF |= 0x4;  //00000100
			PORTD |= 0xA0; //10100000
			/*
			digitalWrite(Motor1en, HIGH);
			digitalWrite(Motor3en, HIGH);
			digitalWrite(Motor3in, HIGH);
			*/
			break;
		case 6: // 110
			PORTF |= 0x4;  //00000100
			PORTD |= 0xC0; //11000000
			/*
			digitalWrite(Motor2en, HIGH);
			digitalWrite(Motor3en, HIGH);
			digitalWrite(Motor3in, HIGH);
			*/
			break;
		case 2: // 010
			PORTF |= 0x1;  //00000001
			PORTD |= 0x60; //01100000
			/*
			digitalWrite(Motor1en, HIGH);
			digitalWrite(Motor2en, HIGH);
			digitalWrite(Motor1in, HIGH);
			*/
			break;
		case 3: // 011
			PORTF |= 0x1;  //00000001
			PORTD |= 0xA0; //10100000
			/*
			digitalWrite(Motor1en, HIGH);
			digitalWrite(Motor3en, HIGH);
			digitalWrite(Motor1in, HIGH);
			*/
			break;
		case 1: // 001
			PORTF |= 0x2;  //00000010
			PORTD |= 0xC0; //11000000
			/*
			digitalWrite(Motor2en, HIGH);
			digitalWrite(Motor3en, HIGH);
			digitalWrite(Motor2in, HIGH);
			*/
			break;
		}
	}
	else {
		;
	}
}
