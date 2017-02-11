#include "pitches.h"

int potPin1 = 14;
int potPin2 = 2;
int ledPin = 13;
int inPin = 7;
int val1 = 0;
int val2 = 0;
int val3 = 0;
int buzzerPin = 10;
int derp = 0;
int counter = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(inPin, INPUT);
}

void loop() {
  val1 = analogRead(potPin1);
  val1 = map(val1, 0, 1023, 2, 10);
  derp = 1000/2*val1;
  val2 = analogRead(potPin2);
  val3 = digitalRead(inPin);
  if (val3 == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  } else {
    analogWrite(buzzerPin, 255);
    delayMicroseconds(derp);
    counter = counter+derp;
    analogWrite(buzzerPin, 0);
    delayMicroseconds(derp);
    counter = counter+derp;
  }
}
