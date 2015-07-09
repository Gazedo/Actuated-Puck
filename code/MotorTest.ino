#include <avr/io.h>
#include <avr/interrupt.h>

int dec;
char Select, val;
volatile int hall = 0, timeint = 0;
volatile int spd; // -255 up to 255, negative signifies reverse

//PWM motor pins
int Motor1in = 21, Motor2in = 20, Motor3in = 19;
//Enable pins
int Motor1en = 10, Motor2en = 11, Motor3en = 12;
//Hall interrupt and read pins
int Hall1read = 0, Hall2read = 1, Hall3read = 2;
void setup(){
    Serial.begin(9600);
    pinMode(Motor1in, OUTPUT);
    pinMode(Motor2in, OUTPUT);
    pinMode(Motor3in, OUTPUT);
    pinMode(Motor1en, OUTPUT);
    pinMode(Motor2en, OUTPUT);
    pinMode(Motor3en, OUTPUT);
    PCICR |= (1<<0);
    PCMSK0 |= (0x5);
    spd = 0;
    DDRB = 0x7;
    PORTB = 0x7;
}

void loop() {
  //sei();
  //Serial.println("before");
  Serial.print(timeint);
//  if (millis() - timeint >= 250) {
//    Serial.write("turned off motor");
//      PORTF &= 0xF8;
//      PORTD &= 0x1F;
//      PORTC = 0x0;
//  }
  if( Serial.available() == 4 ) {
    Select = Serial.read();
    dec = 0;
    for (int i=100; i>=1; i/=10) {
      val = Serial.read() - '0';
      dec += (int)val * i;
    }
  }
  else if (Serial.available() < 4  || Serial.available() > 4) {
    while(Serial.available()){
      byte h = Serial.read();
    }
  }
  
  switch (Select) {
  case 'n':
    spd = -1*(int)dec;
    break;
  case 'p':
    spd = (int)dec;
    break;
  }
  delay(100);
  PCIFR |= (1 << 0);
  Serial.println(spd,DEC);
}

ISR(PCINT0_vect){
	PORTF &= 0xF8;
        PORTD &= 0x1F;
        PORTC = 0x0;
	hall = PINB & 0x7;
	if ( spd > 0 ){ //Forward
		switch (hall) {
			case 5: // 101
				digitalWrite(Motor1en, HIGH);
				digitalWrite(Motor2en, HIGH);
				analogWrite(Motor1in, spd);
				break;
			case 4: // 100
				digitalWrite(Motor1en, HIGH);
				digitalWrite(Motor3en, HIGH);
				analogWrite(Motor1in, spd);
				break;
			case 6: // 110
				digitalWrite(Motor2en, HIGH);
				digitalWrite(Motor3en, HIGH);
				analogWrite(Motor2in, spd);
				break;
			case 2: // 010
				digitalWrite(Motor1en, HIGH);
				digitalWrite(Motor2en, HIGH);
				analogWrite(Motor2in, spd);
				break;
			case 3: // 011
				digitalWrite(Motor1en, HIGH);
				digitalWrite(Motor3en, HIGH);
				analogWrite(Motor3in, spd);
				break;
			case 1: // 001
				digitalWrite(Motor2en, HIGH);
				digitalWrite(Motor3en, HIGH);
				analogWrite(Motor3in, spd);
				break;
		}
	}
	else{ //Reverse
		switch (hall) {
			case 5: // 101
				digitalWrite(Motor1en, HIGH);
				digitalWrite(Motor2en, HIGH);
				analogWrite(Motor2in, spd);
				break;
			case 4: // 100 WRONG
				digitalWrite(Motor1en, HIGH);
				digitalWrite(Motor3en, HIGH);
				analogWrite(Motor3in, spd);
				break;
			case 6: // 110
				digitalWrite(Motor2en, HIGH);
				digitalWrite(Motor3en, HIGH);
				analogWrite(Motor3in, spd);
				break;
			case 2: // 010
				digitalWrite(Motor1en, HIGH);
				digitalWrite(Motor2en, HIGH);
				analogWrite(Motor1in, spd);
				break;
			case 3: // 011
				digitalWrite(Motor1en, HIGH);
				digitalWrite(Motor3en, HIGH);
				analogWrite(Motor1in, spd);
				break;
			case 1: // 001  WRONG
				digitalWrite(Motor2en, HIGH);
				digitalWrite(Motor3en, HIGH);
				analogWrite(Motor2in, spd);
				break;
		}
	}
}
