/*

PIEZO BUZZER TOOL

Author: Benjamin D. Harvatine
Started: Thursday, January 26, 2017
Updated: Monday, February 20, 2017

*/

#include "pitches.h"

#define BUZ 2

int melody[] = {G3, E4, C4};
int durations[] = {4, 4, 2};
int gaps[] = {5, 5, 3};
int gap = 5;

int i = 0;
int j = 0;

void setup() {

  pinMode(BUZ, OUTPUT);
  pinMode(13, OUTPUT);

  cli();
  
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0  = 0;
  TCCR0B |= (1 << WGM01);
  TIMSK0 |= (1 << OCIE0A);

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  TCCR1B |= (1 << WGM12);
  TIMSK1 |= (1 << OCIE1A);
  
  sei();
  
  set_duty_timer(melody[0]);
  set_duration_timer(durations[0]);

}

ISR(TIMER0_COMPA_vect){
  if (digitalRead(BUZ) == HIGH) {
    digitalWrite(BUZ, LOW);
  } else {
    digitalWrite(BUZ, HIGH);
  }
}

ISR(TIMER1_COMPA_vect){
  if (j == 0) {
    TCCR0B &= ~(1 << CS00);
    TCCR0B &= ~(1 << CS01);
    TCCR0B &= ~(1 << CS02);
    set_duration_timer(gaps[i]);
    j ++;
  } else if ( j == 1 && i < (sizeof(melody)/sizeof(int)-1)) {
    j = 2;
    set_duration_timer(gaps[i]);
    digitalWrite(13, LOW);
  } else {
    i = 0;
    j = 0;
    set_duty_timer(melody[i]);
    set_duration_timer(durations[i]);
    digitalWrite(13, HIGH);
  }
}

void set_duty_timer (int frequency) {
  
  OCR0A = (16*10^6)/(64*2*frequency)-1;
  TCCR0B |= (1 << CS01 | 1<<CS00);  
  
}

void set_duration_timer (int duration) {

  OCR1A = (16*10^6)/(64*duration)-1;
  TCCR1B |= (1 << CS12);  
  
}

void loop() {}
