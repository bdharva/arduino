int ledPin = 5;
int butPin = 12;
int buzPin = 10;
int potPin1 = 14;
int potPin2 = 16;

int frequency = 0;
float period = 0;
int volume = 0;
int pushed = 0;
int duty_on = 0;
int duty_off = 0;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(butPin, INPUT);
  pinMode(buzPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

}

void loop() {

  frequency = analogRead(potPin1);
  frequency = map(frequency, 0, 1023, 2, 10);
  
  period = 1000/2*frequency; // Gives on/off period in microseconds
  
  volume = analogRead(potPin2);
  volume = map(volume, 0, 1023, 0, 100);
  
  duty_on = volume*period/100;
  duty_off = (100-volume)*period/100;
  
  pushed = digitalRead(butPin);
  
  if (pushed == HIGH) {
  
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  
  } else {
  
    // Bit banging PWM ftw
    
    analogWrite(buzPin, 255);
    delayMicroseconds(duty_on);
    analogWrite(buzPin, 0);
    delayMicroseconds(duty_off);
    
  }
  
}
