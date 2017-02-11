#define LED_A 3
#define LED_B 4
#define LED_C 5
#define LED_D 6

#define BUZ 10
#define BTN 12
#define POT 14

int highs[] = { LED_A, LED_B, LED_C, LED_D, LED_A, LED_B, LED_C, LED_D, LED_A, LED_B, LED_C, LED_D };
int lows[] = { LED_B, LED_A, LED_D, LED_C, LED_C, LED_D, LED_A, LED_B, LED_D, LED_C, LED_B, LED_A };

int target = 0;

void setup() {
  
  pinMode(LED_A, INPUT);
  pinMode(LED_B, INPUT);
  pinMode(LED_C, INPUT);
  pinMode(LED_D, INPUT);

  pinMode(BUZ, OUTPUT);
  pinMode(BTN, INPUT);
  pinMode(POT, INPUT);

}

void loop()
{
  if (digitalRead(BTN) == LOW) {
    if (target < 11) { target += 1; }
    else { target = 0; }
  }
  set_pins(target);
}

void set_pins(int target)
{
  // reset all the pins
  reset_pins();

  // set the high and low pins to output
  pinMode(highs[target], OUTPUT);
  pinMode(lows[target], OUTPUT);

  // set high pin to logic high, low to logic low
  digitalWrite(highs[target], HIGH);
  digitalWrite(lows[target],LOW);
}

void reset_pins()
{
  // start by ensuring all pins are at input and low
  pinMode(LED_A, INPUT); 
  pinMode(LED_B, INPUT);
  pinMode(LED_C, INPUT);
  pinMode(LED_D, INPUT);

  digitalWrite(LED_A, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_C, LOW);
  digitalWrite(LED_D, LOW);
}
