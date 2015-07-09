#include <avr/io.h>
#include <avr/interrupt.h>

int dec;
char Select = 0, val;
int hall = 0, timeint = 0;

//PWM motor pins
int Motor1in = 21, Motor2in = 20, Motor3in = 19;
//Enable pins
int Motor1en = 10, Motor2en = 11, Motor3en = 12;
//Hall interrupt and read pins
int Hall1read = 0, Hall2read = 1, Hall3read = 2;
int spd = 10;
void setup() {
	Serial.begin(9600);
	DDRB = 0x7;
	PORTB = 0x7;
}

void loop() {
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
