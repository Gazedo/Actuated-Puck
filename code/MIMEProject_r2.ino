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
volatile int hall = 0;
volatile int spd; // -255 up to 255, negative signifies reverse

//PWM motor pins
int Motor1in = 21, Motor2in = 20, Motor3in = 19;
//Enable pins
int Motor1en = 10, Motor2en = 11, Motor3en = 12;//pin 10 actually actautes pin 23
//Hall interrupt and read pins
int Hall1read = 0, Hall2read = 1, Hall3read = 2;
volatile int timeint = 0
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
  //Setup AVR interupts
  PCICR |= (1 << 0);
  PCMSK0 |= (0x5);
  //setup hall input
  DDRB |= 0x7;
  PORTB |= 0x7;
  //Init the imu
  delay(100);
  imu.init();
  //Set mode on pid library
  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(-255.00, 255.00);
  delay(100);
  interrupts();
}

void loop() {
  //Compute at 200 Hz
  time = millis();

  //In case motor gets stuck between commutation sequences, kill before motor driver death.
  if (time - timeint >= 50) {
    PORTF = 0x0;
    PORTD = 0x0;
    PORTC = 0x0;
  }
  if (time - oldtime >= 5) {
    Serial.print(time); Serial.print("  "); Serial.println(oldtime);
    imu.getValues(vals);
    noInterrupts();
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
    Serial1.print(time);
    //Record all current values
    for (int i = 0; i < 6; i++) {
      Serial1.print(vals[i], 4);
      Serial1.print('\t');
    }
    Serial1.println("");
    interrupts();
    PCIFR |= (1 << 0);
  }
  //Recieve Instructions in the form of a single character and 3 digit number for now.
  //Could upgrade
  if ( Serial1.available() >= 4 ) {
    Select = Serial1.read();
    dec = 0;
    for (int i = 100; i >= 1; i /= 10) {
      dec += (Serial1.read() - '0') * i;
    }
  }
  else if ( Serial1.available() < 4 ) {
    Serial1.flush(); // get rid of excess
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
}

ISR(PCINT0_vect) {
  //Looks like a long ISR but only 1 case is run each time one of the hall pins change. The rest is a decision structure.
  timeint = millis();
  hall = PINB & 0000111;
  PORTF = 0x0;
  PORTD = 0x0;
  PORTC = 0x0

  if ( spd > 0 ) { //Forward
    switch (hall) {
    case 5: // 101
      analogWrite(Motor1en, spd);//spd
      analogWrite(Motor2en, 255);
      digitalWrite(Motor1in, HIGH);
      break;
    case 4: // 100
      analogWrite(Motor1en, spd);//spd
      analogWrite(Motor3en, 255);
      digitalWrite(Motor1in, HIGH);
      break;
    case 6: // 110
      analogWrite(Motor2en, spd);//spd
      analogWrite(Motor3en, 255);
      digitalWrite(Motor2in, HIGH);
      break;
    case 2: // 010
      analogWrite(Motor1en, 255);
      analogWrite(Motor2en, spd);//spd
      digitalWrite(Motor2in, HIGH);
      break;
    case 3: // 011
      analogWrite(Motor1en, 255);
      analogWrite(Motor3en, spd);//spd
      digitalWrite(Motor3in, HIGH);
      break;
    case 1: // 001
      analogWrite(Motor2en, 255);
      analogWrite(Motor3en, spd);//spd
      digitalWrite(Motor3in, HIGH);
      break;
    }
  }
  else { //Reverse
    switch (hall) {
    case 5: // 101
      analogWrite(Motor1en, spd);
      analogWrite(Motor2en, 255);//spd
      digitalWrite(Motor2in, HIGH);
      break;
    case 4: // 100 WRONG
      analogWrite(Motor1en, spd);
      analogWrite(Motor3en, 255);//spd
      digitalWrite(Motor3in, HIGH);
      break;
    case 6: // 110
      analogWrite(Motor2en, spd);
      analogWrite(Motor3en, 255);//spd
      digitalWrite(Motor3in, HIGH);
      break;
    case 2: // 010
      analogWrite(Motor1en, spd);//spd
      analogWrite(Motor2en, 255);
      digitalWrite(Motor1in, HIGH);
      break;
    case 3: // 011
      analogWrite(Motor1en, spd);//spd
      analogWrite(Motor3en, 255);
      digitalWrite(Motor1in, HIGH);
      break;
    case 1: // 001
      analogWrite(Motor2en, spd);//spd
      analogWrite(Motor3en, 255);
      digitalWrite(Motor2in, HIGH);
      break;
    }
  }
}
