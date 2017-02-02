#define LED_A 4
#define LED_B 5
#define LED_C 6

int interval = 1001;
int count = 0;

void setup() {
  
  pinMode(LED_A, INPUT);
  pinMode(LED_B, INPUT);
  pinMode(LED_C, INPUT);

}

void loop()
{
  // run through a sample loop, lighting each LED

  if(count >= interval && interval > 1) {
    count = 0;
    reset_pins();
    delay(interval);
    interval = interval-100;
  }

  // in turn and holding for half a second.
  set_pins(LED_A, LED_B);
  count = count + 1;
  delay(1);

  set_pins(LED_B, LED_A);
  count = count + 1;
  delay(1);

  set_pins(LED_C, LED_A); 
  count = count + 1;
  delay(1);

  set_pins(LED_A, LED_C);
  count = count + 1;
  delay(1);

  set_pins(LED_B, LED_C);
  count = count + 1;
  delay(1);

  set_pins(LED_C, LED_B);
  count = count + 1;
  delay(1);

}

void set_pins(int high_pin, int low_pin)
{
  // reset all the pins
  reset_pins();

  // set the high and low pins to output
  pinMode(high_pin, OUTPUT);
  pinMode(low_pin, OUTPUT);

  // set high pin to logic high, low to logic low
  digitalWrite(high_pin, HIGH);
  digitalWrite(low_pin,LOW);
}

void reset_pins()
{
  // start by ensuring all pins are at input and low
  pinMode(LED_A, INPUT); 
  pinMode(LED_B, INPUT);
  pinMode(LED_C, INPUT);

  digitalWrite(LED_A, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_C, LOW);
}
