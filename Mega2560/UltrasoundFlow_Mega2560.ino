#include <avr/io.h>
#include <avr/interrupt.h>
#define ICP4 49
#define ICP5 48
#define N1 13
#define N2 12

volatile unsigned int ICR4_temp = 0;
volatile unsigned int ICR5_temp = 0;
unsigned long timens4 = 0;
unsigned long timens5 =0;

void setup() {
  Serial.begin(9600);
  pinMode(ICP4, INPUT);
  pinMode(ICP5, INPUT);
  pinMode(N1, OUTPUT);
  pinMode(N2, OUTPUT);

  /*Timer 4 Input Capture Unit*/
  TCCR4A = B00000000;
  TCCR4B =  B01000000; //ICES4
  TCCR4C = B00000000;
  TIFR4 = TIFR4 | B00100000; //ICF4
  TIMSK4 = TIMSK4 | B00100000; //ICIE4
  /*Timer 5 Input Capture Unit*/
  TCCR5A = B00000000;
  TCCR5B =  B01000000; //ICES5
  TCCR5C = B00000000;
  TIFR5 = TIFR5 | B00100000; //ICF5
  TIMSK5 = TIMSK5 | B00100000; //ICIE5
  /*Timer 1 - CTC OCR1A Clear Timer on Compare Match*/
  TCCR1A = B00000000; 
  TCCR1B = B00001100; //WGM12,CS12,CS11,CS10,prescal_256
  TCCR1C = B00000000;
  TIFR1 = TIFR1 | B00000010;//OCF1A
  TIMSK1 = TIMSK1 | B00000010;//OCIE1A
  OCR1A = 15000; // Every 240 ms interup
  /*Timer 3 - CTC OCR3A Clear Timer on Compare Match*/
  TCCR3A = B00000000; 
  TCCR3B = B00001100; //WGM32,CS32,CS31,CS30,prescal_256
  TCCR3C = B00000000;
  TIFR3 = TIFR3 | B00000010;//OCF3A
  TIMSK3 = TIMSK3 | B00000010;//OCIE3A
  OCR3A = 15000; // Every 240 ms interup
  /**/
  TCNT4 = 0;
  TCNT1 = 0;
  TCNT5 = 0;
  TCNT3 = 0;
}

void loop() {
  cli();
      timens4 = ICR4_temp;
      timens4 = timens4 * 62.5;
      timens4 = timens4 / 1000; //microsecund
      timens5 = ICR5_temp;
      timens5 = timens5 * 62.5;
      timens5 = timens5 / 1000; //microsecund
      Serial.print(timens4);
      Serial.print(" / ");
      Serial.println(timens5);
  sei();
}

ISR(TIMER1_COMPA_vect) { //send 40hz I
  digitalWrite(N1, HIGH);
  delayMicroseconds(25);
  digitalWrite(N1, LOW);
  TCCR4B = TCCR4B | B00000001; //CS40 prescal 1, measurement.
}

ISR(TIMER4_CAPT_vect) { //receive I
  ICR4_temp = ICR4;
  TCCR4B &= ~(1<<CS40);
  TCNT4=0;
}

ISR(TIMER3_COMPA_vect) { //send 40hz II
  digitalWrite(N2, HIGH);
  delayMicroseconds(25);
  digitalWrite(N2, LOW);
  TCCR5B = TCCR5B | B00000001; //CS50 prescal 1, measurement.
}

ISR(TIMER5_CAPT_vect) { //receive II
  ICR5_temp = ICR5;
  TCCR5B &= ~(1<<CS50);
  TCNT5=0;
}
